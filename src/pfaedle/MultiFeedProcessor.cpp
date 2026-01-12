#include "pfaedle/MultiFeedProcessor.h"

#include <algorithm>
#include <dirent.h>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>

#ifdef LIBZIP_FOUND
#include <zip.h>
#endif

#include "ad/cppgtfs/Writer.h"
#include "pfaedle/gtfs/Writer.h"
#include "pfaedle/osm/OsmBuilder.h"
#include "pfaedle/router/Router.h"
#include "pfaedle/router/ShapeBuilder.h"
#include "pfaedle/router/Stats.h"
#include "util/geo/Geo.h"
#include "util/log/Log.h"

// Forward declare logic from PfaedleMain potentially?
// No, we reimplement clean logic here or call existing libraries.

namespace pfaedle {

using util::DEBUG;
using util::ERROR;
using util::INFO;
using util::WARN;

MultiFeedProcessor::MultiFeedProcessor(
    const config::Config &cfg, const std::vector<config::MotConfig> &motConfigs)
    : _cfg(cfg), _motConfigs(motConfigs) {}

int MultiFeedProcessor::run() {
  LOG(INFO) << "Starting Multi-Feed Processing Mode...";

  scanFeeds();

  if (_feedPaths.empty()) {
    LOG(ERROR) << "No GTFS feeds found in "
               << (_cfg.feedPaths.empty() ? "null" : _cfg.feedPaths[0]);
    return 1;
  }

  LOG(INFO) << "Found " << _feedPaths.size() << " feeds to process.";

  try {
    buildGraphs();
  } catch (const std::exception &e) {
    LOG(ERROR) << "Failed to build OSM graphs: " << e.what();
    return 1;
  }

  processFeeds();

  return 0;
}

void MultiFeedProcessor::scanFeeds() {
  _feedPaths.clear();

  for (const auto &path : _cfg.feedPaths) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0) {
      if (s.st_mode & S_IFDIR) {
        LOG(INFO) << "Scanning directory: " << path;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(path.c_str())) != NULL) {
          while ((ent = readdir(dir)) != NULL) {
            std::string fname = ent->d_name;
            if (fname.length() > 4 &&
                fname.substr(fname.length() - 4) == ".zip") {
              std::string fullPath = path;
              if (fullPath.back() != '/')
                fullPath += "/";
              fullPath += fname;
              _feedPaths.push_back(fullPath);
            }
          }
          closedir(dir);
        }
      } else {
        _feedPaths.push_back(path);
      }
    }
  }
  std::sort(_feedPaths.begin(), _feedPaths.end());
}

void MultiFeedProcessor::buildGraphs() {
  // 1. Calculate Union BBox
  pfaedle::osm::BBoxIdx unionBox(_cfg.boxPadding);

  LOG(INFO) << "Calculating Union Bounding Box across " << _feedPaths.size()
            << " feeds...";

  for (const auto &path : _feedPaths) {
    bool fastSuccess = false;
#ifdef LIBZIP_FOUND
    int err = 0;
    struct zip *z = zip_open(path.c_str(), 0, &err);
    if (z) {
      struct zip_file *zf = zip_fopen(z, "stops.txt", 0);
      if (zf) {
        LOG(INFO) << "Fast scanning stops.txt in " << path;

        struct StopCand {
          std::string id, name;
          double lat, lon;
        };
        std::vector<StopCand> allStops;
        std::vector<char> buffer(65536);
        std::string remainder;
        zip_int64_t bytesRead;
        bool headerParsed = false;
        int latIdx = -1, lonIdx = -1, nameIdx = -1, idIdx = -1;

        while ((bytesRead = zip_fread(zf, buffer.data(), buffer.size())) > 0) {
          remainder.append(buffer.data(), bytesRead);
          size_t pos = 0, newlinePos;
          while ((newlinePos = remainder.find('\n', pos)) !=
                 std::string::npos) {
            std::string line = remainder.substr(pos, newlinePos - pos);
            if (!line.empty() && line.back() == '\r')
              line.pop_back();

            std::vector<std::string> cols;
            std::string curCol;
            bool inQuote = false;
            for (char c : line) {
              if (c == '"')
                inQuote = !inQuote;
              else if (c == ',' && !inQuote) {
                cols.push_back(curCol);
                curCol.clear();
              } else
                curCol += c;
            }
            cols.push_back(curCol);

            if (!headerParsed) {
              for (int i = 0; i < (int)cols.size(); ++i) {
                std::string col = cols[i];
                if (i == 0 && col.size() > 3 &&
                    col.substr(0, 3) == "\xEF\xBB\xBF")
                  col = col.substr(3);
                if (col.size() >= 2 && col.front() == '"' && col.back() == '"')
                  col = col.substr(1, col.size() - 2);
                if (col == "stop_lat")
                  latIdx = i;
                if (col == "stop_lon")
                  lonIdx = i;
                if (col == "stop_name")
                  nameIdx = i;
                if (col == "stop_id")
                  idIdx = i;
              }
              headerParsed = true;
            } else if (latIdx != -1 && lonIdx != -1 && idIdx != -1) {
              if (latIdx < (int)cols.size() && lonIdx < (int)cols.size() &&
                  idIdx < (int)cols.size()) {
                try {
                  double lat = std::stod(cols[latIdx]);
                  double lon = std::stod(cols[lonIdx]);
                  if (lat >= -90.0 && lat <= 90.0 && lon >= -180.0 &&
                      lon <= 180.0) {
                    allStops.push_back(
                        {cols[idIdx],
                         nameIdx < (int)cols.size() ? cols[nameIdx] : "", lat,
                         lon});
                  }
                } catch (...) {
                }
              }
            }
            pos = newlinePos + 1;
          }
          remainder = remainder.substr(pos);
        }
        zip_fclose(zf);

        if (!allStops.empty()) {
          // Identify candidates (top 100 in each direction)
          std::unordered_map<std::string, int> candTrips;
          auto addCands = [&](auto cmp) {
            std::sort(allStops.begin(), allStops.end(), cmp);
            for (int i = 0; i < std::min((int)allStops.size(), 100); ++i)
              candTrips[allStops[i].id] = 0;
            for (int i = 0; i < std::min((int)allStops.size(), 100); ++i)
              candTrips[allStops[allStops.size() - 1 - i].id] = 0;
          };
          addCands([](const StopCand &a, const StopCand &b) {
            return a.lat < b.lat;
          });
          addCands([](const StopCand &a, const StopCand &b) {
            return a.lon < b.lon;
          });

          zf = zip_fopen(z, "stop_times.txt", 0);
          if (zf) {
            LOG(INFO) << "Verifying " << candTrips.size()
                      << " candidates in stop_times.txt...";
            remainder.clear();
            headerParsed = false;
            int stopIdIdx = -1;
            while ((bytesRead = zip_fread(zf, buffer.data(), buffer.size())) >
                   0) {
              remainder.append(buffer.data(), bytesRead);
              size_t pos = 0, newlinePos;
              while ((newlinePos = remainder.find('\n', pos)) !=
                     std::string::npos) {
                std::string line = remainder.substr(pos, newlinePos - pos);
                if (!line.empty() && line.back() == '\r')
                  line.pop_back();
                if (!headerParsed) {
                  std::vector<std::string> cols;
                  std::string curCol;
                  bool inQuote = false;
                  for (char c : line) {
                    if (c == '"')
                      inQuote = !inQuote;
                    else if (c == ',' && !inQuote) {
                      cols.push_back(curCol);
                      curCol.clear();
                    } else
                      curCol += c;
                  }
                  cols.push_back(curCol);
                  for (int i = 0; i < (int)cols.size(); ++i) {
                    std::string col = cols[i];
                    if (i == 0 && col.length() > 3 &&
                        col.substr(0, 3) == "\xEF\xBB\xBF")
                      col = col.substr(3);
                    if (col == "stop_id")
                      stopIdIdx = i;
                  }
                  headerParsed = true;
                } else if (stopIdIdx != -1) {
                  // Fast extract stop_id
                  size_t s = 0;
                  for (int i = 0; i < stopIdIdx; ++i) {
                    s = line.find(',', s);
                    if (s == std::string::npos)
                      break;
                    s++;
                  }
                  if (s != std::string::npos) {
                    size_t e = line.find(',', s);
                    std::string sid = line.substr(
                        s, e == std::string::npos ? std::string::npos : e - s);
                    if (!sid.empty() && sid.front() == '"' && sid.back() == '"')
                      sid = sid.substr(1, sid.size() - 2);
                    auto it = candTrips.find(sid);
                    if (it != candTrips.end())
                      it->second++;
                  }
                }
                pos = newlinePos + 1;
              }
              remainder = remainder.substr(pos);
            }
            zip_fclose(zf);
          }

          double minLat = 90, maxLat = -90, minLon = 180, maxLon = -180;
          std::string minLatStop, maxLatStop, minLonStop, maxLonStop;
          for (const auto &s : allStops) {
            auto it = candTrips.find(s.id);
            if (it != candTrips.end() && it->second >= 3) {
              unionBox.add(
                  util::geo::Box<double>(util::geo::DPoint(s.lon, s.lat),
                                         util::geo::DPoint(s.lon, s.lat)));
              if (s.lat < minLat) {
                minLat = s.lat;
                minLatStop = s.id + " / " + s.name + " (" +
                             std::to_string(it->second) + " trips)";
              }
              if (s.lat > maxLat) {
                maxLat = s.lat;
                maxLatStop = s.id + " / " + s.name + " (" +
                             std::to_string(it->second) + " trips)";
              }
              if (s.lon < minLon) {
                minLon = s.lon;
                minLonStop = s.id + " / " + s.name + " (" +
                             std::to_string(it->second) + " trips)";
              }
              if (s.lon > maxLon) {
                maxLon = s.lon;
                maxLonStop = s.id + " / " + s.name + " (" +
                             std::to_string(it->second) + " trips)";
              }
            }
          }
          LOG(INFO) << "Feed extremes: Lat " << minLat << " (" << minLatStop
                    << ") to " << maxLat << " (" << maxLatStop << "), Lon "
                    << minLon << " (" << minLonStop << ") to " << maxLon << " ("
                    << maxLonStop << ")";
        }
        fastSuccess = true;
      }
      zip_close(z);
    }
#endif

    if (!fastSuccess) {
      try {
        LOG(INFO) << "Full parsing feed for BBox: " << path;
        pfaedle::gtfs::Feed feed;
        ad::cppgtfs::Parser p(path);
        p.parse(&feed);

        pfaedle::router::ShapeBuilder::getGtfsBox(
            &feed, _cfg.mots, "", _cfg.dropShapes, &unionBox, 0, nullptr, 0);
      } catch (const std::exception &e) {
        LOG(WARN) << "Failed to parse feed for BBox: " << path << " - "
                  << e.what();
      }
    }
  }

  LOG(INFO) << "Union BBox: " << unionBox.getFullBox().getLowerLeft().getY()
            << "," << unionBox.getFullBox().getLowerLeft().getX() << " to "
            << unionBox.getFullBox().getUpperRight().getY() << ","
            << unionBox.getFullBox().getUpperRight().getX();

  // 2. Build Graphs per MotConfig
  for (auto &motCfg :
       const_cast<std::vector<config::MotConfig> &>(_motConfigs)) {
    // Use pointer as key
    config::MotConfig *key = &motCfg;
    if (_bundles.count(key))
      continue;

    GraphBundle bundle;
    bundle.graph = std::make_shared<pfaedle::trgraph::Graph>();
    bundle.restrictor = std::make_shared<pfaedle::osm::Restrictor>();

    std::string osmPath =
        motCfg.osmPath.empty() ? _cfg.osmPath : motCfg.osmPath;
    LOG(INFO) << "---- Building Graph for MOT Config ["
              << pfaedle::router::getMotStr(motCfg.mots) << "] using OSM "
              << osmPath << " ...";
    if (osmPath.empty()) {
      throw std::runtime_error("No OSM path provided for MOT Config");
    }

    pfaedle::osm::OsmBuilder osmBuilder;
    osmBuilder.read(osmPath, motCfg.osmBuildOpts, bundle.graph.get(), unionBox,
                    _cfg.gridSize, bundle.restrictor.get());

    // Build Indexes
    LOG(INFO) << "Building Spatial Index (Grids)...";
    bundle.eGrid = std::make_shared<pfaedle::trgraph::EdgeGrid>(
        _cfg.gridSize, _cfg.gridSize, unionBox.getFullBox(), false);
    bundle.nGrid = std::make_shared<pfaedle::trgraph::NodeGrid>(
        _cfg.gridSize, _cfg.gridSize, unionBox.getFullBox(), false);
    bundle.bbox = unionBox;

    // Populate Grids
    auto &eGrid = *bundle.eGrid;
    auto &nGrid = *bundle.nGrid;

    for (auto *n : bundle.graph->getNds()) {
      for (auto *e : n->getAdjListOut()) {
        if (e->pl().lvl() > motCfg.osmBuildOpts.maxSnapLevel)
          continue;
        if (e->pl().oneWay() == 2)
          continue;
        eGrid.add(*e->pl().getGeom(), e);
      }
    }

    for (auto *n : bundle.graph->getNds()) {
      if (n->pl().getSI()) {
        nGrid.add(*n->pl().getGeom(), n);
      }
    }

    _bundles[key] = bundle;
  }
}

void MultiFeedProcessor::buildIndex(GraphBundle &bundle,
                                    const config::Config &cfg) {
  // Not used inline
}

void MultiFeedProcessor::processFeeds() {
  // We want copy-on-write sharing of the large pre-built graphs. This is
  // achieved by fork()ing per feed *after* graphs are built.
  //
  // However, on macOS, forking a process that has used threads (even if joined)
  // is prone to crashes like "std::system_error: thread::join failed".
  // Therefore we only use fork() on Linux.
#if defined(__linux__)
  for (size_t feedIdx = 0; feedIdx < _feedPaths.size(); ++feedIdx) {
    const auto &path = _feedPaths[feedIdx];
    LOG(INFO) << "Processing Feed " << (feedIdx + 1) << "/" << _feedPaths.size()
              << ": " << path;

    pid_t pid = fork();

    if (pid == 0) {
      // Child
      try {
        processSingleFeed((int)feedIdx);
        _exit(0);
      } catch (const std::exception &e) {
        LOG(ERROR) << "Child process failed: " << e.what();
        _exit(1);
      }
    } else if (pid > 0) {
      // Parent
      int status;
      waitpid(pid, &status, 0);
      if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        LOG(INFO) << "Feed " << path << " processed successfully.";
      } else {
        LOG(ERROR) << "Feed " << path << " failed.";
      }
    } else {
      LOG(ERROR) << "Fork failed!";
    }
  }
#else
  for (size_t feedIdx = 0; feedIdx < _feedPaths.size(); ++feedIdx) {
    const auto &path = _feedPaths[feedIdx];
    LOG(INFO) << "Processing Feed " << (feedIdx + 1) << "/" << _feedPaths.size()
              << ": " << path;
    try {
      processSingleFeed((int)feedIdx);
      LOG(INFO) << "Feed " << path << " processed successfully.";
    } catch (const std::exception &e) {
      LOG(ERROR) << "Feed " << path << " failed: " << e.what();
    }
  }
#endif
}

void MultiFeedProcessor::processSingleFeed(int feedIdx) {
  std::string feedPath = _feedPaths[feedIdx];

  // Parse Feed
  pfaedle::gtfs::Feed feed;
  LOG(INFO) << "Parsing GTFS...";
  ad::cppgtfs::Parser p(feedPath);
  p.parse(&feed);

  // Prepare Output Path
  std::string outDir = _cfg.outputPath;
  if (_cfg.outputPath.empty()) {
    if (_cfg.inPlace) {
      size_t lastSlash = feedPath.rfind('/');
      if (lastSlash != std::string::npos) {
        outDir = feedPath.substr(0, lastSlash);
      } else {
        outDir = ".";
      }
    } else {
      outDir = "."; // Current path
    }
  }

  // ensure trailing slash
  if (outDir.back() != '/')
    outDir += "/";

  std::string fileName = feedPath;
  size_t lastSlash = feedPath.rfind('/');
  if (lastSlash != std::string::npos)
    fileName = feedPath.substr(lastSlash + 1);

  // Strip .zip extension if present
  if (fileName.length() > 4 &&
      fileName.substr(fileName.length() - 4) == ".zip") {
    fileName = fileName.substr(0, fileName.length() - 4);
  }

  // Ensure .gtfs suffix
  if (fileName.length() < 5 ||
      fileName.substr(fileName.length() - 5) != ".gtfs") {
    fileName += ".gtfs";
  }

  std::string outPath = outDir + fileName;

  // mkdir -p
  // Basic mkdir for now (posix mkdir)
  mkdir(outDir.c_str(), 0777);

  // Matching Logic
  for (auto &motCfg :
       const_cast<std::vector<config::MotConfig> &>(_motConfigs)) {

    // Determine used MOTs
    auto usedMots = pfaedle::router::motISect(motCfg.mots, _cfg.mots);
    if (!usedMots.size())
      continue;

    LOG(INFO) << "Matching MOTs: " << pfaedle::router::getMotStr(usedMots);

    // GET PRE-BUILT BUNDLE
    if (!_bundles.count(&motCfg)) {
      LOG(ERROR) << "No graph bundle for MOT Config!";
      continue;
    }
    auto &bundle = _bundles[&motCfg];

    pfaedle::router::FeedStops fStops =
        pfaedle::router::writeMotStops(&feed, usedMots, _cfg.shapeTripId);

    // Instantiate Classifier
    pfaedle::statsimiclassifier::StatsimiClassifier *statsimiClassifier;
    if (motCfg.routingOpts.statsimiMethod == "bts") {
      statsimiClassifier = new pfaedle::statsimiclassifier::BTSClassifier();
    } else if (motCfg.routingOpts.statsimiMethod == "jaccard") {
      statsimiClassifier = new pfaedle::statsimiclassifier::JaccardClassifier();
    } else if (motCfg.routingOpts.statsimiMethod == "jaccard-geodist") {
      statsimiClassifier =
          new pfaedle::statsimiclassifier::JaccardGeodistClassifier();
    } else if (motCfg.routingOpts.statsimiMethod == "ed") {
      statsimiClassifier = new pfaedle::statsimiclassifier::EDClassifier();
    } else if (motCfg.routingOpts.statsimiMethod == "ped") {
      statsimiClassifier = new pfaedle::statsimiclassifier::PEDClassifier();
    } else {
      LOG(ERROR) << "Unknown station similarity classifier "
                 << motCfg.routingOpts.statsimiMethod;
      exit(1);
    }

    // Instantiate Router
    using namespace pfaedle::router;
    Router *router = 0;
    if (motCfg.routingOpts.transPenMethod == "exp") {
      if (_cfg.noAStar)
        router = new RouterImpl<ExpoTransWeightNoHeur>();
      else
        router = new RouterImpl<ExpoTransWeight>();
    } else if (motCfg.routingOpts.transPenMethod == "distdiff") {
      if (_cfg.noAStar)
        router = new RouterImpl<DistDiffTransWeightNoHeur>();
      else
        router = new RouterImpl<DistDiffTransWeight>();
    } else if (motCfg.routingOpts.transPenMethod == "timenorm") {
      if (_cfg.noAStar)
        router = new RouterImpl<NormDistrTransWeightNoHeur>();
      else
        router = new RouterImpl<NormDistrTransWeight>();
    } else {
      LOG(ERROR) << "Unknown routing method "
                 << motCfg.routingOpts.transPenMethod;
      exit(1);
    }

    // ShapeBuilder
    pfaedle::router::ShapeBuilder shapeBuilder(
        &feed, usedMots, motCfg, bundle.graph.get(), &fStops,
        bundle.restrictor.get(), statsimiClassifier, router, _cfg,
        bundle.eGrid.get(), bundle.nGrid.get());

    pfaedle::netgraph::Graph ng;
    shapeBuilder.shapeify(&ng);

    delete router;
    delete statsimiClassifier;
  }

  // Write Output
  try {
    LOG(INFO) << "Writing output GTFS to " << outPath << " ...";
    pfaedle::gtfs::Writer w;
    w.setMapping({});
    w.write(&feed, outPath);
  } catch (const ad::cppgtfs::WriterException &ex) {
    LOG(ERROR) << "Could not write output GTFS feed: " << ex.what();
    throw;
  }
}
} // namespace pfaedle
// Implementation of trip-level caching for shape computation.

#include "pfaedle/router/TripCache.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <limits.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#include <utility>
#include <vector>

#include "pfaedle/Def.h"
#include "util/3rdparty/MurmurHash3.h"
#include "util/Misc.h"
#include "util/String.h"
#include "util/log/Log.h"

namespace pfaedle {
namespace router {
namespace {
constexpr uint32_t kTripCacheVersion = 3;
constexpr char kTripCacheMagic[4] = {'P', 'F', 'C', char('0' + kTripCacheVersion)};
constexpr uint64_t kCoordScale = 10000000ULL;  // ~1cm precision

constexpr int kMaxStartupLogs = 16;
constexpr int kMaxErrorLogs = 16;
std::atomic<int> g_lookupSampleCount{0};
std::atomic<int> g_errorSampleCount{0};

inline std::string canonicalPath(const std::string& path) {
  if (path.empty()) return path;
  char buf[PATH_MAX];
  if (::realpath(path.c_str(), buf)) {
    return std::string(buf);
  }
  return path;
}

std::string motSetKey(const pfaedle::router::MOTs& mots) {
  std::vector<int> entries;
  entries.reserve(mots.size());
  for (auto mot : mots) entries.push_back(static_cast<int>(mot));
  std::sort(entries.begin(), entries.end());
  std::ostringstream ss;
  for (size_t i = 0; i < entries.size(); ++i) {
    if (i) ss << ',';
    ss << entries[i];
  }
  return ss.str();
}

struct FileEntry {
  std::string path;
  uint64_t size;
  std::time_t mtime;
};

inline uint64_t packCoord(int32_t lat, int32_t lon) {
  return (static_cast<uint64_t>(static_cast<uint32_t>(lat)) << 32) |
         static_cast<uint32_t>(lon);
}

inline std::string toHex(uint64_t value, size_t width = 16) {
  std::ostringstream ss;
  ss << std::hex << std::setfill('0') << std::setw(static_cast<int>(width))
     << value;
  return ss.str();
}

inline std::string digestToHex(uint64_t lo, uint64_t hi) {
  return toHex(hi) + toHex(lo);
}

inline std::string fingerprintToHex(const uint64_t digest[2]) {
  return digestToHex(digest[0], digest[1]);
}

inline int32_t quantizeCoord(double coord) {
  return static_cast<int32_t>(
      std::llround(coord * static_cast<double>(kCoordScale)));
}

inline std::string joinPath(const std::string& base, const std::string& comp) {
  if (base.empty()) return comp;
  if (comp.empty()) return base;
  if (base.back() == '/') {
    return comp.front() == '/' ? base + comp.substr(1) : base + comp;
  }
  return comp.front() == '/' ? base + comp : base + '/' + comp;
}

inline std::string parentPath(const std::string& path) {
  if (path.empty()) return "";
  const auto pos = path.find_last_of('/');
  if (pos == std::string::npos) return "";
  if (pos == 0) return "/";
  return path.substr(0, pos);
}

inline bool ensureDirExists(const std::string& rawPath) {
  if (rawPath.empty() || rawPath == ".") return true;
  if (rawPath == "/") return true;

  std::string path = rawPath;
  while (path.size() > 1 && path.back() == '/') path.pop_back();
  if (path.empty()) path = ".";
  struct stat st;
  if (::stat(path.c_str(), &st) == 0) {
    return S_ISDIR(st.st_mode);
  }

  const auto pos = path.find_last_of('/');
  if (pos != std::string::npos) {
    const std::string parent = pos == 0 ? "/" : path.substr(0, pos);
    if (!ensureDirExists(parent)) return false;
  }

  if (::mkdir(path.c_str(), 0755) == 0) return true;
  if (errno == EEXIST) {
    return ::stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
  }
  LOG(WARN) << "Trip cache: unable to create directory " << path
            << ": " << std::strerror(errno);
  return false;
}

inline bool fileExists(const std::string& path) {
  struct stat st;
  return ::stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

inline uint64_t fileSize(const std::string& path) {
  struct stat st;
  if (::stat(path.c_str(), &st) != 0 || !S_ISREG(st.st_mode)) return 0;
  return static_cast<uint64_t>(st.st_size);
}

void collectFiles(const std::string& root, std::vector<FileEntry>* files,
                  uint64_t* total) {
  if (root.empty()) return;
  std::vector<std::string> stack{root};
  while (!stack.empty()) {
    const std::string current = stack.back();
    stack.pop_back();

    DIR* dir = ::opendir(current.c_str());
    if (!dir) continue;

    struct dirent* entry = nullptr;
    while ((entry = ::readdir(dir)) != nullptr) {
      if (std::strcmp(entry->d_name, ".") == 0 ||
          std::strcmp(entry->d_name, "..") == 0) {
        continue;
      }

      const std::string child = joinPath(current, entry->d_name);

      struct stat st;
      if (::lstat(child.c_str(), &st) != 0) continue;

      if (S_ISDIR(st.st_mode)) {
        stack.push_back(child);
      } else if (S_ISREG(st.st_mode)) {
        const uint64_t size = static_cast<uint64_t>(st.st_size);
        if (files) files->push_back({child, size, st.st_mtime});
        if (total) *total += size;
      }
    }

    ::closedir(dir);
  }
}

inline void updateFileTimestamp(const std::string& path) {
  if (::utime(path.c_str(), nullptr) != 0) {
    LOG(WARN) << "Trip cache: unable to update timestamp for " << path
              << ": " << std::strerror(errno);
  }
}
}  // namespace

TripCache::TripCache(const TripCacheOptions& options,
                     const trgraph::Graph* graph)
    : _enabled(options.enabled && graph),
      _maxBytes(options.maxBytes),
      _graph(graph) {
  if (!_enabled) return;

  if (options.graphHash.empty() || options.paramsHash.empty()) {
    _enabled = false;
    LOG(WARN) << "Trip cache disabled: missing graph or parameter fingerprint";
    return;
  }

  _root = joinPath(options.baseDir, "trip-cache");
  _root = joinPath(_root, std::string("v") + std::to_string(kTripCacheVersion));
  _root = joinPath(_root, options.graphHash);
  _root = joinPath(_root, options.paramsHash);

  LOG(INFO) << "Trip cache root " << _root << " (graph=" << options.graphHash
            << ", params=" << options.paramsHash << ")";

  if (!ensureDirExists(_root)) {
    _enabled = false;
    LOG(WARN) << "Trip cache disabled: unable to create directory " << _root;
    return;
  }

  rebuildEdgeIndex();
  _currentBytes = directorySize(_root);
}

void TripCache::rebuildEdgeIndex() {
  _edgeIndex.clear();
  if (!_graph) return;

  for (auto* node : _graph->getNds()) {
    for (auto* edge : node->getAdjListOut()) {
      const EdgeHash hashF = computeEdgeHash(edge);
      _edgeIndex[hashF].push_back(edge);
      // Also index reversed orientation to better match cached entries
      // regardless of traversal direction.
      // Hash the edge in reverse order using the dedicated overload.
      const EdgeHash hashR = computeEdgeHash(edge, /*reversed=*/true);
      // Prevent duplicate vector push when hashF==hashR (palindromic edges)
      if (!(hashR.lo == hashF.lo && hashR.hi == hashF.hi)) {
        _edgeIndex[hashR].push_back(edge);
      }
    }
  }
}

uint64_t TripCache::directorySize(const std::string& root) {
  uint64_t total = 0;
  collectFiles(root, nullptr, &total);
  return total;
}

TripCacheStats TripCache::stats() const {
  TripCacheStats s;
  s.hits = _hits.load();
  s.misses = _misses.load();
  s.stores = _stores.load();
  s.storeSkipped = _storeSkipped.load();
  s.errors = _errors.load();
  s.evictions = _evictions.load();
  s.bytesRead = _bytesRead.load();
  s.bytesWritten = _bytesWritten.load();
  return s;
}

std::string TripCache::hashString(const std::string& value) {
  uint64_t digest[2] = {0, 0};
  MurmurHash3_x64_128(value.data(), static_cast<int>(value.size()), 0x3f56167d,
                      digest);
  return fingerprintToHex(digest);
}

TripCache::EdgeHash TripCache::computeEdgeHash(const trgraph::Edge* edge) {
  // Default to forward orientation; see overload for reversed.
  return computeEdgeHash(edge, false);
}

TripCache::EdgeHash TripCache::computeEdgeHash(const trgraph::Edge* edge,
                                               bool reversed) {
  std::vector<uint64_t> buffer;
  buffer.reserve(16);

  const auto* fromNode = reversed ? edge->getTo() : edge->getFrom();
  const auto* toNode = reversed ? edge->getFrom() : edge->getTo();
  const auto* fromGeom = fromNode->pl().getGeom();
  const auto* toGeom = toNode->pl().getGeom();

  // Component ids are intentionally omitted: they depend on build order and
  // would make the hash unstable across runs even when the underlying graph is
  // identical.
  const auto& pl = edge->pl();
  const uint8_t lvl = pl.lvl();
  const uint8_t oneWay = pl.oneWay();
  const uint8_t restricted = pl.isRestricted() ? 1 : 0;
  const uint8_t revFlag = pl.isRev() ? 1 : 0;

  buffer.push_back(static_cast<uint64_t>(lvl));
  buffer.push_back(static_cast<uint64_t>(oneWay));
  buffer.push_back(static_cast<uint64_t>(restricted));
  buffer.push_back(static_cast<uint64_t>(revFlag));

  const LINE* geom = pl.getGeom();
  const uint32_t pointCount = geom ? static_cast<uint32_t>(geom->size()) : 0;
  buffer.push_back(static_cast<uint64_t>(pointCount));

  if (geom) {
    // Sample a few points in traversal order to reduce sensitivity to
    // segmentation variations while keeping determinism.
    const size_t samples = pointCount <= 5 ? pointCount : 5;
    for (size_t si = 0; si < samples; ++si) {
      size_t idx;
      if (pointCount <= 5) {
        idx = si;
      } else {
        static const double pos[5] = {0.0, 0.25, 0.5, 0.75, 1.0};
        idx = static_cast<size_t>(pos[si] * (pointCount - 1));
      }
      const auto& point = (*geom)[reversed ? (pointCount - 1 - idx) : idx];
      const int32_t lat = quantizeCoord(point.getY());
      const int32_t lon = quantizeCoord(point.getX());
      buffer.push_back(packCoord(lat, lon));
    }
  } else if (fromGeom && toGeom) {
    const int32_t latFrom = quantizeCoord(fromGeom->getY());
    const int32_t lonFrom = quantizeCoord(fromGeom->getX());
    const int32_t latTo = quantizeCoord(toGeom->getY());
    const int32_t lonTo = quantizeCoord(toGeom->getX());
    if (!reversed) {
      buffer.push_back(packCoord(latFrom, lonFrom));
      buffer.push_back(packCoord(latTo, lonTo));
    } else {
      buffer.push_back(packCoord(latTo, lonTo));
      buffer.push_back(packCoord(latFrom, lonFrom));
    }
  }

  uint64_t digest[2] = {0, 0};
  MurmurHash3_x64_128(buffer.data(),
                      static_cast<int>(buffer.size() * sizeof(uint64_t)),
                      0x51ed270b, digest);
  return EdgeHash{digest[0], digest[1]};
}

const trgraph::Edge* TripCache::resolveEdge(const EdgeHash& hash) const {
  auto it = _edgeIndex.find(hash);
  if (it == _edgeIndex.end() || it->second.empty()) return nullptr;
  return it->second.front();
}

std::string TripCache::entryPath(const std::string& key) const {
  std::string dir = _root;
  if (key.size() >= 2) dir = joinPath(dir, key.substr(0, 2));
  return joinPath(dir, key + ".bin");
}

bool TripCache::readEntry(const std::string& path, EdgeListHops* hops,
                          std::string* errorReason) {
  auto fail = [&](const std::string& msg) {
    if (errorReason) *errorReason = msg;
    return false;
  };
  std::ifstream ifs(path, std::ios::binary);
  if (!ifs.good()) return fail("open failed");

  char magic[4];
  ifs.read(magic, sizeof(magic));
  if (!ifs.good() ||
      std::memcmp(magic, kTripCacheMagic, sizeof(magic)) != 0) {
    return fail("invalid magic");
  }

  uint32_t version = 0;
  ifs.read(reinterpret_cast<char*>(&version), sizeof(version));
  if (!ifs.good() || version != kTripCacheVersion) {
    return fail("version mismatch");
  }

  uint32_t hopCount = 0;
  ifs.read(reinterpret_cast<char*>(&hopCount), sizeof(hopCount));
  if (!ifs.good()) return fail("hop count read failed");

  EdgeListHops out;
  out.reserve(hopCount);

  for (uint32_t i = 0; i < hopCount; ++i) {
    uint8_t hasStartEdge = 0;
    uint8_t hasEndEdge = 0;
    uint8_t hasStartPoint = 0;
    uint8_t hasEndPoint = 0;
    ifs.read(reinterpret_cast<char*>(&hasStartEdge), sizeof(hasStartEdge));
    ifs.read(reinterpret_cast<char*>(&hasEndEdge), sizeof(hasEndEdge));
    ifs.read(reinterpret_cast<char*>(&hasStartPoint), sizeof(hasStartPoint));
    ifs.read(reinterpret_cast<char*>(&hasEndPoint), sizeof(hasEndPoint));

    double progrStart = 0.0;
    double progrEnd = 0.0;
    ifs.read(reinterpret_cast<char*>(&progrStart), sizeof(progrStart));
    ifs.read(reinterpret_cast<char*>(&progrEnd), sizeof(progrEnd));

    double startX = 0, startY = 0, endX = 0, endY = 0;
    if (hasStartPoint) {
      ifs.read(reinterpret_cast<char*>(&startX), sizeof(startX));
      ifs.read(reinterpret_cast<char*>(&startY), sizeof(startY));
    }
    if (hasEndPoint) {
      ifs.read(reinterpret_cast<char*>(&endX), sizeof(endX));
      ifs.read(reinterpret_cast<char*>(&endY), sizeof(endY));
    }

    uint32_t edgeCount = 0;
    ifs.read(reinterpret_cast<char*>(&edgeCount), sizeof(edgeCount));
    if (!ifs.good()) return fail("edge count read failed");

    EdgeList edges;
    edges.reserve(edgeCount);
    for (uint32_t e = 0; e < edgeCount; ++e) {
      EdgeHash hash;
      ifs.read(reinterpret_cast<char*>(&hash.lo), sizeof(hash.lo));
      ifs.read(reinterpret_cast<char*>(&hash.hi), sizeof(hash.hi));
      if (!ifs.good()) return fail("edge hash read failed");
      const trgraph::Edge* resolved = resolveEdge(hash);
      if (!resolved) {
        std::ostringstream ss;
        ss << "unresolved edge hash=" << toHex(hash.hi) << toHex(hash.lo);
        return fail(ss.str());
      }
      edges.push_back(const_cast<trgraph::Edge*>(resolved));
    }

    EdgeHash startHash{0, 0};
    EdgeHash endHash{0, 0};
    ifs.read(reinterpret_cast<char*>(&startHash.lo), sizeof(startHash.lo));
    ifs.read(reinterpret_cast<char*>(&startHash.hi), sizeof(startHash.hi));
    ifs.read(reinterpret_cast<char*>(&endHash.lo), sizeof(endHash.lo));
    ifs.read(reinterpret_cast<char*>(&endHash.hi), sizeof(endHash.hi));
    if (!ifs.good()) return fail("start/end hash read failed");

    const trgraph::Edge* startEdge = nullptr;
    const trgraph::Edge* endEdge = nullptr;
    if (hasStartEdge) {
      startEdge = resolveEdge(startHash);
      if (!startEdge) {
        std::ostringstream ss;
        ss << "unresolved start hash=" << toHex(startHash.hi)
           << toHex(startHash.lo);
        return fail(ss.str());
      }
    }
    if (hasEndEdge) {
      endEdge = resolveEdge(endHash);
      if (!endEdge) {
        std::ostringstream ss;
        ss << "unresolved end hash=" << toHex(endHash.hi)
           << toHex(endHash.lo);
        return fail(ss.str());
      }
    }

    EdgeListHop hop;
    hop.edges = std::move(edges);
    hop.start = startEdge;
    hop.end = endEdge;
    hop.progrStart = progrStart;
    hop.progrEnd = progrEnd;
    if (hasStartPoint) hop.pointStart = POINT(startX, startY);
    if (hasEndPoint) hop.pointEnd = POINT(endX, endY);

    out.push_back(std::move(hop));
  }

  *hops = std::move(out);
  return true;
}

bool TripCache::writeEntry(const std::string& path, const EdgeListHops& hops) {
  std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
  if (!ofs.good()) return false;

  ofs.write(kTripCacheMagic, sizeof(kTripCacheMagic));
  ofs.write(reinterpret_cast<const char*>(&kTripCacheVersion),
            sizeof(kTripCacheVersion));
  const uint32_t hopCount = static_cast<uint32_t>(hops.size());
  ofs.write(reinterpret_cast<const char*>(&hopCount), sizeof(hopCount));

  for (const auto& hop : hops) {
    const uint8_t hasStartEdge = hop.start ? 1 : 0;
    const uint8_t hasEndEdge = hop.end ? 1 : 0;
    const uint8_t hasStartPoint = hop.start ? 0 : 1;
    const uint8_t hasEndPoint = hop.end ? 0 : 1;

    ofs.write(reinterpret_cast<const char*>(&hasStartEdge), sizeof(hasStartEdge));
    ofs.write(reinterpret_cast<const char*>(&hasEndEdge), sizeof(hasEndEdge));
    ofs.write(reinterpret_cast<const char*>(&hasStartPoint), sizeof(hasStartPoint));
    ofs.write(reinterpret_cast<const char*>(&hasEndPoint), sizeof(hasEndPoint));

    ofs.write(reinterpret_cast<const char*>(&hop.progrStart),
              sizeof(hop.progrStart));
    ofs.write(reinterpret_cast<const char*>(&hop.progrEnd),
              sizeof(hop.progrEnd));

    if (hasStartPoint) {
      double x = hop.pointStart.getX();
      double y = hop.pointStart.getY();
      ofs.write(reinterpret_cast<const char*>(&x), sizeof(x));
      ofs.write(reinterpret_cast<const char*>(&y), sizeof(y));
    }
    if (hasEndPoint) {
      double x = hop.pointEnd.getX();
      double y = hop.pointEnd.getY();
      ofs.write(reinterpret_cast<const char*>(&x), sizeof(x));
      ofs.write(reinterpret_cast<const char*>(&y), sizeof(y));
    }

    const uint32_t edgeCount = static_cast<uint32_t>(hop.edges.size());
    ofs.write(reinterpret_cast<const char*>(&edgeCount), sizeof(edgeCount));
    for (const auto* edge : hop.edges) {
      const EdgeHash hash = computeEdgeHash(edge);
      ofs.write(reinterpret_cast<const char*>(&hash.lo), sizeof(hash.lo));
      ofs.write(reinterpret_cast<const char*>(&hash.hi), sizeof(hash.hi));
    }

    EdgeHash startHash{0, 0};
    EdgeHash endHash{0, 0};
    if (hop.start) startHash = computeEdgeHash(hop.start);
    if (hop.end) endHash = computeEdgeHash(hop.end);
    ofs.write(reinterpret_cast<const char*>(&startHash.lo), sizeof(startHash.lo));
    ofs.write(reinterpret_cast<const char*>(&startHash.hi), sizeof(startHash.hi));
    ofs.write(reinterpret_cast<const char*>(&endHash.lo), sizeof(endHash.lo));
    ofs.write(reinterpret_cast<const char*>(&endHash.hi), sizeof(endHash.hi));
  }

  ofs.close();
  return ofs.good();
}

bool TripCache::lookup(const std::string& key, EdgeListHops* hops) {
  if (!_enabled) return false;

  std::lock_guard<std::mutex> lock(_mutex);
  const std::string path = entryPath(key);
  if (!fileExists(path)) {
    int sampleIdx = g_lookupSampleCount.load();
    if (sampleIdx < kMaxStartupLogs) {
      sampleIdx = g_lookupSampleCount.fetch_add(1);
      if (sampleIdx < kMaxStartupLogs) {
        LOG(INFO) << "Trip cache miss (no file) key=" << key
                  << " path=" << path;
      }
    }
    _misses.fetch_add(1);
    return false;
  }

  EdgeListHops tmp;
  std::string readError;
  if (!readEntry(path, &tmp, &readError)) {
    int sampleIdx = g_errorSampleCount.load();
    if (sampleIdx < kMaxErrorLogs) {
      sampleIdx = g_errorSampleCount.fetch_add(1);
      if (sampleIdx < kMaxErrorLogs) {
        LOG(WARN) << "Trip cache read failed key=" << key
                  << " path=" << path << " reason=" << readError;
      }
    }
    _errors.fetch_add(1);
    _misses.fetch_add(1);
    if (::remove(path.c_str()) != 0) {
      LOG(WARN) << "Trip cache: failed to remove corrupt entry " << path
                << ": " << std::strerror(errno);
    }
    return false;
  }

  *hops = std::move(tmp);
  const uint64_t size = fileSize(path);
  _bytesRead.fetch_add(size);
  _hits.fetch_add(1);
  updateFileTimestamp(path);
  int sampleIdx = g_lookupSampleCount.load();
  if (sampleIdx < kMaxStartupLogs) {
    sampleIdx = g_lookupSampleCount.fetch_add(1);
    if (sampleIdx < kMaxStartupLogs) {
      LOG(INFO) << "Trip cache hit key=" << key << " bytes=" << size
                << " path=" << path;
    }
  }
  return true;
}

void TripCache::store(const std::string& key, const EdgeListHops& hops) {
  if (!_enabled || key.empty()) return;

  std::lock_guard<std::mutex> lock(_mutex);
  const std::string path = entryPath(key);
  if (fileExists(path)) {
    _storeSkipped.fetch_add(1);
    updateFileTimestamp(path);
    return;
  }

  const std::string parent = parentPath(path);
  if (!parent.empty() && !ensureDirExists(parent)) {
    _errors.fetch_add(1);
    return;
  }

  if (!writeEntry(path, hops)) {
    _errors.fetch_add(1);
    return;
  }

  const uint64_t size = fileSize(path);
  _bytesWritten.fetch_add(size);
  _stores.fetch_add(1);
  _currentBytes += size;
  updateFileTimestamp(path);
  pruneIfNeeded();
}

void TripCache::remove(const std::string& key) {
  if (!_enabled) return;
  std::lock_guard<std::mutex> lock(_mutex);
  const std::string path = entryPath(key);
  if (!fileExists(path)) return;

  const uint64_t size = fileSize(path);
  if (::remove(path.c_str()) != 0) {
    LOG(WARN) << "Trip cache: failed to remove entry " << path
              << ": " << std::strerror(errno);
    return;
  }

  if (_currentBytes >= size) _currentBytes -= size;
}

void TripCache::pruneIfNeeded() {
  if (_maxBytes == 0 || _currentBytes <= _maxBytes) return;

  std::vector<FileEntry> entries;
  uint64_t total = 0;
  collectFiles(_root, &entries, &total);

  if (total <= _maxBytes) {
    _currentBytes = total;
    return;
  }

  std::sort(entries.begin(), entries.end(),
            [](const FileEntry& a, const FileEntry& b) {
              if (a.mtime == b.mtime) return a.path < b.path;
              return a.mtime < b.mtime;
            });

  for (const auto& entry : entries) {
    if (total <= _maxBytes) break;
    if (::remove(entry.path.c_str()) == 0) {
      total -= entry.size;
      _evictions.fetch_add(1);
    } else {
      LOG(WARN) << "Trip cache: failed to prune entry " << entry.path
                << ": " << std::strerror(errno);
    }
  }

  _currentBytes = total;
}

std::string TripCache::computeGraphFingerprint(const trgraph::Graph* graph) {
  if (!graph) return "0";
  std::vector<EdgeHash> hashes;
  hashes.reserve(graph->getNds().size() * 4);

  for (auto* node : graph->getNds()) {
    for (auto* edge : node->getAdjListOut()) {
      hashes.push_back(computeEdgeHash(edge));
    }
  }

  std::sort(hashes.begin(), hashes.end(), [](const EdgeHash& a, const EdgeHash& b) {
    if (a.hi == b.hi) return a.lo < b.lo;
    return a.hi < b.hi;
  });

  std::vector<uint64_t> buffer;
  buffer.reserve(hashes.size() * 2);
  for (const auto& hash : hashes) {
    buffer.push_back(hash.lo);
    buffer.push_back(hash.hi);
  }

  uint64_t digest[2] = {0, 0};
  MurmurHash3_x64_128(buffer.data(),
                      static_cast<int>(buffer.size() * sizeof(uint64_t)),
                      0x27487642, digest);
  return fingerprintToHex(digest);
}

std::string TripCache::computeGraphFingerprint(
    const config::Config& cfg, const config::MotConfig& motCfg,
    const std::vector<std::string>& cfgPaths,
    const std::string& paramsHash) {
  UNUSED(cfgPaths);
  std::ostringstream ss;
  ss << "params=" << paramsHash << '\n';
  ss << "mot=" << motSetKey(motCfg.mots) << '\n';

  const std::string osmPath = canonicalPath(cfg.osmPath);
  ss << "osm_path=" << osmPath << '\n';
  if (!osmPath.empty()) {
    struct stat st;
    if (::stat(osmPath.c_str(), &st) == 0) {
      ss << "osm_size=" << static_cast<unsigned long long>(st.st_size)
         << '\n';
      ss << "osm_mtime=" << static_cast<unsigned long long>(st.st_mtime)
         << '\n';
    }
  }

  ss << std::setprecision(12);
  ss << "grid_size=" << cfg.gridSize << '\n';
  ss << "box_padding=" << cfg.boxPadding << '\n';

  return hashString(ss.str());
}

std::string TripCache::computeParamsFingerprint(
    const config::Config& cfg, const config::MotConfig& motCfg,
    const std::vector<std::string>& cfgPaths) {
  std::ostringstream ss;
  ss << "gaussian=" << cfg.gaussianNoise << '\n'
     << "no_fast=" << cfg.noFastHops << '\n'
     << "no_astar=" << cfg.noAStar << '\n'
     << "no_trie=" << cfg.noTrie << '\n'
     << "trans_method=" << motCfg.routingOpts.transPenMethod << '\n'
     << "em_method=" << motCfg.routingOpts.emPenMethod << '\n'
     << "statsimi=" << motCfg.routingOpts.statsimiMethod << '\n'
     << "full_turn_punish=" << motCfg.routingOpts.fullTurnPunishFac << '\n'
     << "full_turn_angle=" << motCfg.routingOpts.fullTurnAngle << '\n'
     << "line_unmatched=" << motCfg.routingOpts.lineUnmatchedPunishFact
     << '\n'
     << "line_from_unmatched="
     << motCfg.routingOpts.lineNameFromUnmatchedPunishFact << '\n'
     << "line_to_unmatched="
     << motCfg.routingOpts.lineNameToUnmatchedPunishFact << '\n'
     << "no_lines=" << motCfg.routingOpts.noLinesPunishFact << '\n'
     << "platform_pen=" << motCfg.routingOpts.platformUnmatchedPen << '\n'
     << "station_pen=" << motCfg.routingOpts.stationUnmatchedPen << '\n'
     << "station_dist_pen=" << motCfg.routingOpts.stationDistPenFactor
     << '\n'
     << "non_station_pen=" << motCfg.routingOpts.nonStationPen << '\n'
     << "turn_restr_cost=" << motCfg.routingOpts.turnRestrCost << '\n'
     << "transition_pen=" << motCfg.routingOpts.transitionPen << '\n'
     << "use_stations=" << motCfg.routingOpts.useStations << '\n'
     << "pop_reach_edge=" << motCfg.routingOpts.popReachEdge << '\n'
     << "no_self_hops=" << motCfg.routingOpts.noSelfHops << '\n'
     << "mot_cfg_param=" << cfg.motCfgParam << '\n'
     << "trans_weight=" << motCfg.transWeight << '\n';

  for (const auto& path : cfgPaths) {
    std::ifstream ifs(path);
    if (!ifs.good()) continue;
    ss << "#file:" << path << '\n' << ifs.rdbuf() << '\n';
  }

  return hashString(ss.str());
}

std::string TripCache::computeTripKey(const gtfs::Trip* trip) {
  if (!trip) return "0";

  std::ostringstream ss;
  const auto& serviceId = trip->getService();
  const auto& shapeId = trip->getShape();
  ss << "trip=" << trip->getId() << '|'
    << "service=" << serviceId << '|'
    << "block=" << trip->getBlockId() << '|'
    << "shape=" << shapeId << '|'
     << "headsign=" << trip->getHeadsign() << '|'
     << "short=" << trip->getShortname() << '|';
  const auto route = trip->getRoute();
  const auto agency = route->getAgency();
  ss << "route=" << route->getId() << '|'
     << "agency=" << (agency ? agency->getId() : "") << '|'
     << "type=" << static_cast<int>(route->getType()) << '|'
     << "dir=" << static_cast<int>(trip->getDirection()) << '|';

  ss << "stops=";
  for (const auto& st : trip->getStopTimes()) {
    const auto stop = st.getStop();
    ss << stop->getId();
    ss << '@' << stop->getPlatformCode();
    ss << ';';
    if (stop->getId().empty()) {
      ss << std::fixed << std::setprecision(6) << stop->getLat() << ','
         << stop->getLng() << ';' << stop->getName() << ';';
    }
  }

  return hashString(ss.str());
}

unsigned TripCache::deriveDeterministicSeed(const std::string& graphHash,
                                            const std::string& paramsHash) {
  auto mixComponent = [](const std::string& hex) -> unsigned {
    if (hex.empty()) return 0u;
    unsigned valueHi = 0u;
    unsigned valueLo = 0u;
    const size_t hiLen = std::min<size_t>(8, hex.size());
    std::stringstream ss;
    ss << std::hex << hex.substr(0, hiLen);
    ss >> valueHi;
    if (hex.size() >= 8) {
      ss.clear();
      ss.str("");
      ss << std::hex << hex.substr(hex.size() - 8);
      ss >> valueLo;
    }
    return valueHi ^ valueLo;
  };

  unsigned seed = 0x811C9DC5u;
  auto mix = [&](const std::string& component) {
    unsigned value = mixComponent(component);
    seed ^= value + 0x9e3779b9u + (seed << 6) + (seed >> 2);
  };

  mix(graphHash);
  mix(paramsHash);
  if (seed == 0u) seed = 0x9747b28cu;
  return seed;
}

}  // namespace router
}  // namespace pfaedle

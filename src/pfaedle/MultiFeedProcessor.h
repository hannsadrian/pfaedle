
#ifndef PFAEDLE_MULTIFEEDPROCESSOR_H_
#define PFAEDLE_MULTIFEEDPROCESSOR_H_

#include "pfaedle/config/MotConfig.h"
#include "pfaedle/config/PfaedleConfig.h"
#include "pfaedle/osm/OsmBuilder.h"
#include "pfaedle/osm/Restrictor.h"
#include "pfaedle/trgraph/Graph.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace pfaedle {

struct GraphBundle {
  std::shared_ptr<pfaedle::trgraph::Graph> graph;
  std::shared_ptr<pfaedle::osm::Restrictor> restrictor;
  std::shared_ptr<pfaedle::trgraph::EdgeGrid> eGrid;
  std::shared_ptr<pfaedle::trgraph::NodeGrid> nGrid;
  pfaedle::osm::BBoxIdx bbox;

  GraphBundle() : bbox(0) {}
};

class MultiFeedProcessor {
public:
  MultiFeedProcessor(const config::Config &cfg,
                     const std::vector<config::MotConfig> &motConfigs);

  // Main entry point
  int run();

private:
  config::Config _cfg;
  std::vector<config::MotConfig> _motConfigs;
  std::vector<std::string> _feedPaths;

  // We may build multiple graph bundles per MOT config if the GTFS stop
  // coordinates form multiple far-apart clusters (to avoid huge OSM bboxes).
  std::map<config::MotConfig *, std::vector<GraphBundle>> _bundles;

  void scanFeeds();
  void buildGraphs();
  void processFeeds();
  void processSingleFeed(int feedIdx);

  // Helper to extract index building logic
  void buildIndex(GraphBundle &bundle, const config::Config &cfg);
};

} // namespace pfaedle

#endif

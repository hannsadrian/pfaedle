// Copyright 2020
// Author: Patrick Brosi

#include <cstdint>
#include <filesystem>
#include <getopt.h>
#include <unistd.h>

#define private public
#include "pfaedle/router/Router.h"
#undef private
#define private private

#include "pfaedle/config/ConfigReader.h"
#include "pfaedle/osm/Restrictor.h"
#include "pfaedle/router/TripCache.h"
#include "pfaedle/trgraph/Graph.h"

using pfaedle::osm::Restrictor;
using pfaedle::router::CostMatrix;
using pfaedle::router::EdgeCandGroup;
using pfaedle::router::ExpoTransWeight;
using pfaedle::router::LayerCostsDAG;
using pfaedle::router::RouterImpl;
using pfaedle::router::RoutingAttrs;
using pfaedle::router::RoutingOpts;
using util::approx;

// _____________________________________________________________________________
uint32_t cmGet(const CostMatrix& m, size_t i, size_t j) {
  for (const auto& e : m) {
    if (e.first.first == i && e.first.second == j) return e.second;
  }

  return -1;
}

// _____________________________________________________________________________
int main(int argc, char** argv) {
  UNUSED(argc);
  UNUSED(argv);
  RouterImpl<ExpoTransWeight> router;

  RoutingAttrs rAttrs;
  RoutingOpts rOpts;
  Restrictor restr;
  LayerCostsDAG initCosts;

  // to make sure we always underestimate the cost in the heuristic for testing
  pfaedle::trgraph::NodePL::comps.emplace_back(
      pfaedle::trgraph::Component{9999999});

  // build transit graph
  pfaedle::trgraph::Graph g;
  auto a = g.addNd(POINT{0, 0});
  auto b = g.addNd(POINT{0, 10});
  auto c = g.addNd(POINT{10, 0});
  auto d = g.addNd(POINT{20, 0});

  a->pl().setComp(1);
  b->pl().setComp(1);
  c->pl().setComp(1);
  d->pl().setComp(1);

  auto eA = g.addEdg(a, c);
  auto eB = g.addEdg(b, c);
  auto eC = g.addEdg(c, d);

  eA->pl().setCost(10);
  eB->pl().setCost(6);
  eC->pl().setCost(100);

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});
    tos.push_back({eC, 0, 0, {}, 0, {}});

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hops(froms, tos, &costM, &dists, rAttrs, rOpts, restr, &c, maxTime);

    TEST(cmGet(costM, 0, 0), ==, approx(10));
    TEST(cmGet(costM, 1, 0), ==, approx(6));
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});
    tos.push_back({eC, 0, 0.5, {}, 0, {}});

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hops(froms, tos, &costM, &dists, rAttrs, rOpts, restr, &c, maxTime);

    TEST(cmGet(costM, 0, 0), ==, approx(50 + 10));
    TEST(cmGet(costM, 1, 0), ==, approx(50 + 6));
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eB, 0, 2.0 / 3.0, {}, 0, {}});
    tos.push_back({eC, 0, 0, {}, 0, {}});

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hops(froms, tos, &costM, &dists, rAttrs, rOpts, restr, &c, maxTime);

    TEST(cmGet(costM, 0, 0), ==, approx(5));
    TEST(cmGet(costM, 1, 0), ==, approx(2));
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eB, 0, 2.0 / 3.0, {}, 0, {}});
    tos.push_back({eC, 0, 0.9, {}, 0, {}});

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hops(froms, tos, &costM, &dists, rAttrs, rOpts, restr, &c, maxTime);

    TEST(cmGet(costM, 0, 0), ==, approx(90 + 5));
    TEST(cmGet(costM, 1, 0), ==, approx(90 + 2));
  }

  // with hopsfast
  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});
    tos.push_back({eC, 0, 0, {}, 0, {}});

    LayerCostsDAG initCost{0, 0};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    TEST(cmGet(costM, 0, 0), >=, maxTime);
    TEST(cmGet(costM, 1, 0), ==, approx(6));
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});
    tos.push_back({eC, 0, 0.5, {}, 0, {}});

    LayerCostsDAG initCost{0, 0};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    TEST(cmGet(costM, 0, 0), >=, maxTime);
    TEST(cmGet(costM, 1, 0), ==, approx(50 + 6));
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eB, 0, 2.0 / 3.0, {}, 0, {}});
    tos.push_back({eC, 0, 0, {}, 0, {}});

    LayerCostsDAG initCost{0, 0};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    TEST(cmGet(costM, 0, 0), >=, maxTime);
    TEST(cmGet(costM, 1, 0), ==, approx(2));
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eB, 0, 2.0 / 3.0, {}, 0, {}});
    tos.push_back({eC, 0, 0.9, {}, 0, {}});

    LayerCostsDAG initCost{0, 0};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    TEST(cmGet(costM, 0, 0), >=, maxTime);
    TEST(cmGet(costM, 1, 0), ==, approx(90 + 2));
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});
    tos.push_back({eC, 0, 0, {}, 0, {}});

    LayerCostsDAG initCost{0, 0};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    TEST(cmGet(costM, 0, 0), ==, approx(5));
    TEST(cmGet(costM, 1, 0), >=, maxTime);
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eA, 0, 0, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});
    tos.push_back({eC, 0, 0, {}, 0, {}});

    LayerCostsDAG initCost{6, 0, 20};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    // we also get this, because the edge is the same!
    TEST(cmGet(costM, 0, 0), ==, approx(5));
    TEST(cmGet(costM, 1, 0), ==, approx(10));
    TEST(cmGet(costM, 2, 0), >=, maxTime);
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eA, 0, 0, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});
    tos.push_back({eC, 0, 1, {}, 0, {}});

    LayerCostsDAG initCost{6, 0, 20};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    // we also get this, because the edge is the same!
    TEST(cmGet(costM, 0, 0), ==, approx(5 + 100));
    TEST(cmGet(costM, 1, 0), ==, approx(10 + 100));
    TEST(cmGet(costM, 2, 0), >=, maxTime);
  }

  {
    EdgeCandGroup froms, tos;
    CostMatrix costM, dists;
    froms.push_back({eA, 0, 0.5, {}, 0, {}});
    froms.push_back({eA, 0, 0, {}, 0, {}});
    froms.push_back({eB, 0, 0, {}, 0, {}});

    tos.push_back({eC, 0, 1, {}, 0, {}});
    tos.push_back({eC, 0, 0.5, {}, 0, {}});

    LayerCostsDAG initCost{6, 0, 20};

    double maxTime = 9999;

    pfaedle::router::HopCache c;

    router.hopsFast(froms, tos, initCost, &costM, rAttrs, rOpts, restr, &c,
                    maxTime);

    // we also get this, because the edge is the same!
    TEST(cmGet(costM, 0, 0), ==, approx(5 + 100));
    TEST(cmGet(costM, 1, 0), ==, approx(10 + 100));
    TEST(cmGet(costM, 0, 1), ==, approx(5 + 50));
    TEST(cmGet(costM, 1, 1), ==, approx(10 + 50));
    TEST(cmGet(costM, 2, 0), >=, maxTime);
    TEST(cmGet(costM, 2, 1), >=, maxTime);
  }

  {
    optind = 0;
    char arg0[] = "pfaedle";
    char arg1[] = "--trip-cache";
    char arg2[] = "--trip-cache-dir";
    char arg3[] = "/tmp";
    char arg4[] = "--trip-cache-max-bytes";
    char arg5[] = "4096";
    char* argvCfg[] = {arg0, arg1, arg2, arg3, arg4, arg5};
    int argcCfg = sizeof(argvCfg) / sizeof(argvCfg[0]);

    pfaedle::config::Config cfg;
    pfaedle::config::ConfigReader::read(&cfg, argcCfg, argvCfg);

    TEST(cfg.tripCache.enabled, ==, true);
    TEST(cfg.tripCache.directory, ==, std::string(arg3));
    TEST(cfg.tripCache.maxBytes, ==, (uint64_t)4096);
  }

  {
    optind = 0;
    char arg0[] = "pfaedle";
    char arg1[] = "--trip-cache";
    char* argvCfg[] = {arg0, arg1};
    int argcCfg = sizeof(argvCfg) / sizeof(argvCfg[0]);

    pfaedle::config::Config cfg;
    pfaedle::config::ConfigReader::read(&cfg, argcCfg, argvCfg);

    TEST(cfg.tripCache.enabled, ==, true);
    TEST(cfg.tripCache.directory, ==, cfg.dbgOutputPath);
  }

  {
    namespace fs = std::filesystem;
    fs::path cacheRoot = fs::temp_directory_path() / "pfaedle-trip-cache-test";
    if (fs::exists(cacheRoot)) fs::remove_all(cacheRoot);

    pfaedle::trgraph::Graph graph;
    auto* n1 = graph.addNd(POINT{0, 0});
    auto* n2 = graph.addNd(POINT{1, 0});
    auto* edge = graph.addEdg(n1, n2);

    pfaedle::router::TripCacheOptions opts;
    opts.enabled = true;
    opts.baseDir = cacheRoot.string();
    opts.maxBytes = 0;
    opts.graphHash =
        pfaedle::router::TripCache::computeGraphFingerprint(&graph);
    opts.paramsHash = "test-params";

    pfaedle::router::TripCache cache(opts, &graph);
    TEST(cache.enabled(), ==, true);

    pfaedle::router::EdgeListHop hop;
    hop.edges.push_back(edge);
    hop.start = edge;
    hop.end = edge;
    hop.progrStart = 0.0;
    hop.progrEnd = 1.0;

    pfaedle::router::EdgeListHops hops{hop};
    std::string key = "trip-cache-test-key";
    cache.store(key, hops);

    pfaedle::router::EdgeListHops loaded;
    bool hit = cache.lookup(key, &loaded);
    TEST(hit, ==, true);
    TEST(loaded.size(), ==, (size_t)1);
    TEST(loaded.front().edges.size(), ==, (size_t)1);
    TEST(loaded.front().edges.front(), ==, edge);

    auto stats = cache.stats();
    TEST(stats.hits, ==, (size_t)1);
    TEST(stats.stores, ==, (size_t)1);

    fs::remove_all(cacheRoot);
  }

  exit(0);
}

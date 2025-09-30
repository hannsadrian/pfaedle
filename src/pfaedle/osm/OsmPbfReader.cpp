#include "pfaedle/osm/OsmPbfReader.h"

#include <stdexcept>
#include <string>

#include "util/log/Log.h"
#include "pfaedle/_config.h"
#include "pfaedle/osm/OsmBuilder.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <unistd.h>
#include <functional>

#ifdef OSMIUM_ENABLED
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/io/pbf_output.hpp>
#include <osmium/io/xml_output.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
// More includes as needed during full implementation
#endif

using pfaedle::osm::BBoxIdx;
using pfaedle::osm::OsmPbfReader;
using pfaedle::trgraph::Graph;

namespace {
#ifdef OSMIUM_ENABLED
struct BboxHandler : public osmium::handler::Handler {
  const BBoxIdx& bbox;
  BboxHandler(const BBoxIdx& b) : bbox(b) {}
  // Placeholder: In a full implementation, we'd collect nodes in bbox and build edges.
};
#endif
}

void OsmPbfReader::read(const std::string& path, const pfaedle::osm::OsmReadOpts& opts,
                        Graph* g, const BBoxIdx& bbox, double gridSize,
                        pfaedle::osm::Restrictor* res) {
  if (!bbox.size()) return;
#ifndef OSMIUM_ENABLED
  throw std::runtime_error("PBF support not available: libosmium/protozero headers not found at build time");
#else
  // Native libosmium streaming reader
  using pfaedle::osm::OsmFilter;
  using pfaedle::osm::OsmIdSet;
  using pfaedle::osm::AttrMap;
  using pfaedle::osm::RelMap;
  using pfaedle::osm::RelLst;
  using pfaedle::osm::Restrictions;
  using pfaedle::osm::Restriction;
  using pfaedle::osm::OsmRel;
  using pfaedle::osm::OsmWay;
  using pfaedle::osm::OsmNode;
  using pfaedle::osm::KeyVal;

  OsmFilter filter(opts);

  // Pass 1: Nodes — collect bbox membership, nohup flags, and station/blocker metadata
  std::unordered_set<pfaedle::osm::osmid> bbox_nodes;
  std::unordered_set<pfaedle::osm::osmid> nohup_nodes;
  std::unordered_set<pfaedle::osm::osmid> blocker_nodes;
  std::unordered_set<pfaedle::osm::osmid> turncycle_nodes;
  struct StationMeta { util::geo::Point<double> pt; AttrMap attrs; };
  std::unordered_map<pfaedle::osm::osmid, StationMeta> station_nodes;

  {
    osmium::io::Reader reader_nodes(path, osmium::osm_entity_bits::node);
    while (osmium::memory::Buffer buffer = reader_nodes.read()) {
      for (auto& item : buffer) {
        if (item.type() != osmium::item_type::node) continue;
        const osmium::Node& n = static_cast<const osmium::Node&>(item);
        if (!n.location()) continue;
        double y = n.location().lat();
        double x = n.location().lon();
        const auto id = static_cast<pfaedle::osm::osmid>(n.id());
        if (bbox.contains(util::geo::Point<double>(x, y))) {
          bbox_nodes.insert(id);
        }
        // Build minimal AttrMap for filter checks (station/blocker/turncycle/nohup)
        AttrMap attrs;
        for (const auto& tag : n.tags()) {
          attrs[tag.key()] = tag.value();
          if (filter.nohup(tag.key(), tag.value())) nohup_nodes.insert(id);
        }
        if (!attrs.empty()) {
          if (filter.station(attrs)) {
            station_nodes.emplace(id, StationMeta{util::geo::Point<double>(x, y), std::move(attrs)});
          } else {
            if (filter.blocker(attrs)) blocker_nodes.insert(id);
            if (filter.turnCycle(attrs)) turncycle_nodes.insert(id);
          }
        }
      }
    }
    reader_nodes.close();
  }

  // Pass 2: Relations — keep/drop, entity mappings, and restrictions
  RelLst rels;
  RelMap node_rels, way_rels;
  Restrictions raw_rests;
  std::set<size_t> flat_rels;  // alias for rels.flat
  {
    osmium::io::Reader reader_rels(path, osmium::osm_entity_bits::relation);
    while (osmium::memory::Buffer buffer = reader_rels.read()) {
      for (auto& item : buffer) {
        if (item.type() != osmium::item_type::relation) continue;
        const osmium::Relation& r = static_cast<const osmium::Relation&>(item);
        AttrMap attrs;
        for (const auto& tag : r.tags()) attrs[tag.key()] = tag.value();
        uint64_t keepFlags = filter.keep(attrs, OsmFilter::REL);
        uint64_t dropFlags = filter.drop(attrs, OsmFilter::REL);
        if (attrs.size() && keepFlags && !dropFlags) {
          size_t idx = rels.rels.size();
          rels.rels.push_back(attrs);
          if (keepFlags & pfaedle::osm::REL_NO_DOWN) {
            rels.flat.insert(idx);
          }
          for (const auto& m : r.members()) {
            if (m.type() == osmium::item_type::node) {
              node_rels[static_cast<pfaedle::osm::osmid>(m.ref())].push_back(idx);
            } else if (m.type() == osmium::item_type::way) {
              way_rels[static_cast<pfaedle::osm::osmid>(m.ref())].push_back(idx);
            }
          }

          // restriction processing (similar to OsmBuilder::readRestr)
          if (attrs.count("type") && attrs.find("type")->second == std::string("restriction")) {
            bool pos = filter.posRestr(attrs);
            bool neg = filter.negRestr(attrs);
            if (pos || neg) {
              pfaedle::osm::osmid from = 0, to = 0, via = 0;
              for (const auto& m : r.members()) {
                if (m.type() == osmium::item_type::way && m.role() == std::string("from")) from = m.ref();
                if (m.type() == osmium::item_type::way && m.role() == std::string("to")) to = m.ref();
              }
              for (const auto& m : r.members()) {
                if (m.type() == osmium::item_type::node && m.role() == std::string("via")) { via = m.ref(); break; }
              }
              if (from && to && via) {
                if (pos) raw_rests.pos[via].push_back(Restriction{from, to});
                if (neg) raw_rests.neg[via].push_back(Restriction{from, to});
              }
            }
          }
        }
      }
    }
    reader_rels.close();
  }

  // Pass 2b: Ways — pre-scan to collect node IDs needed for kept ways that touch the bbox
  pfaedle::osm::OsmBuilder helper;  // to reuse protected helpers where needed
  std::unordered_set<pfaedle::osm::osmid> needed_nodes;
  {
    osmium::io::Reader reader_ways(path, osmium::osm_entity_bits::way);
    while (osmium::memory::Buffer buffer = reader_ways.read()) {
      for (auto& item : buffer) {
        if (item.type() != osmium::item_type::way) continue;
        const osmium::Way& w = static_cast<const osmium::Way&>(item);
        AttrMap wattrs;
        for (const auto& tag : w.tags()) wattrs[tag.key()] = tag.value();
        bool keep = false;
        if (!wattrs.empty()) {
          if (helper.relKeep(w.id(), way_rels, rels.flat)) keep = true;
          else if (filter.keep(wattrs, OsmFilter::WAY) && !filter.drop(wattrs, OsmFilter::WAY)) keep = true;
        }
        if (!keep) continue;
        bool touches_bbox = false;
        for (const auto& nr : w.nodes()) {
          if (bbox_nodes.count(static_cast<pfaedle::osm::osmid>(nr.ref()))) { touches_bbox = true; break; }
        }
        if (!touches_bbox) continue;
        for (const auto& nr : w.nodes()) {
          needed_nodes.insert(static_cast<pfaedle::osm::osmid>(nr.ref()));
        }
      }
    }
    reader_ways.close();
  }

  // Pass 3: Nodes+Ways — build graph using NodeLocationsForWays (index only needed nodes)
  std::unordered_map<pfaedle::osm::osmid, pfaedle::trgraph::Node*> nodes_map;
  std::unordered_map<pfaedle::osm::osmid, std::vector<pfaedle::trgraph::Node*>> mult_nodes;
  pfaedle::router::NodeSet orphan_stations;

  // Use FlexMem (in-RAM) for broad compatibility; memory is controlled by only indexing needed nodes
  using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
  using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

  auto process_restr = [&](pfaedle::osm::osmid nid, pfaedle::osm::osmid wid,
                           pfaedle::trgraph::Edge* e, pfaedle::trgraph::Node* n) {
    if (raw_rests.pos.count(nid)) {
      for (const auto& kv : raw_rests.pos.find(nid)->second) {
        if (kv.eFrom == wid) {
          e->pl().setRestricted();
          res->add(e, kv.eTo, n, true);
        } else if (kv.eTo == wid) {
          e->pl().setRestricted();
          res->relax(wid, n, e);
        }
      }
    }
    if (raw_rests.neg.count(nid)) {
      for (const auto& kv : raw_rests.neg.find(nid)->second) {
        if (kv.eFrom == wid) {
          e->pl().setRestricted();
          res->add(e, kv.eTo, n, false);
        } else if (kv.eTo == wid) {
          e->pl().setRestricted();
          res->relax(wid, n, e);
        }
      }
    }
  };

  {
    // Limit lifetime of the location index/handler to this pass so memory is released before post-processing
    index_type index;
    location_handler_type location_handler(index);
    location_handler.ignore_errors();
    osmium::io::Reader reader(path, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way);
    while (osmium::memory::Buffer buffer = reader.read()) {
      for (auto& item : buffer) {
        switch (item.type()) {
          case osmium::item_type::node: {
            const osmium::Node& n = static_cast<const osmium::Node&>(item);
            // Only index nodes we actually need for kept, bbox-touching ways
            if (needed_nodes.count(static_cast<pfaedle::osm::osmid>(n.id()))) {
              location_handler.node(n);
            }
            break;
          }
          case osmium::item_type::way: {
            osmium::Way& w = const_cast<osmium::Way&>(static_cast<const osmium::Way&>(item));
            AttrMap wattrs;
            for (const auto& tag : w.tags()) wattrs[tag.key()] = tag.value();

            // Keep/drop decision similar to XML path
            bool keep = false;
            if (!wattrs.empty()) {
              if (helper.relKeep(w.id(), way_rels, rels.flat)) keep = true;
              else if (filter.keep(wattrs, OsmFilter::WAY) && !filter.drop(wattrs, OsmFilter::WAY)) keep = true;
            }
            if (!keep) break;

            // Check if any node is in bbox using precomputed bbox_nodes
            bool touches_bbox = false;
            for (const auto& nr : w.nodes()) {
              if (bbox_nodes.count(static_cast<pfaedle::osm::osmid>(nr.ref()))) { touches_bbox = true; break; }
            }
            if (!touches_bbox) break;

            // Resolve node locations only for kept + bbox-touching ways
            location_handler.way(w);

            // Build edges
            pfaedle::trgraph::Node* last = nullptr;
            pfaedle::osm::osmid lastnid = 0;
            for (const auto& nr : w.nodes()) {
              if (!nr.location()) continue;
              const auto nid = static_cast<pfaedle::osm::osmid>(nr.ref());
              double y = nr.location().lat();
              double x = nr.location().lon();
              pfaedle::trgraph::Node* cur = nullptr;
              if (nohup_nodes.count(nid)) {
                cur = g->addNd();
                cur->pl().setGeom(util::geo::Point<double>(x, y));
                mult_nodes[nid].push_back(cur);
              } else {
                auto it = nodes_map.find(nid);
                if (it == nodes_map.end()) {
                  cur = g->addNd();
                  cur->pl().setGeom(util::geo::Point<double>(x, y));
                  nodes_map.emplace(nid, cur);
                } else {
                  cur = it->second;
                }
              }

              if (last) {
                auto e = g->addEdg(last, cur, pfaedle::trgraph::EdgePL());
                if (e) {
                  e->pl().setLvl(filter.level(wattrs));
                  if (filter.oneway(wattrs)) e->pl().setOneWay(1);
                  if (filter.onewayrev(wattrs)) e->pl().setOneWay(2);
                  // Restrictions based on via node
                  process_restr(nid, static_cast<pfaedle::osm::osmid>(w.id()), e, cur);
                  process_restr(lastnid, static_cast<pfaedle::osm::osmid>(w.id()), e, last);
                }
              }
              last = cur;
              lastnid = nid;
            }
            break;
          }
          default:
            break;
        }
      }
    }
    reader.close();
  }
  // We won't need the set of needed node IDs anymore; release memory
  needed_nodes.clear();
  needed_nodes.rehash(0);

  // Apply station info, blockers, and orphan stations
  auto apply_node_flags = [&](pfaedle::osm::osmid nid, pfaedle::trgraph::Node* n) {
    if (blocker_nodes.count(nid)) n->pl().setBlocker();
    if (turncycle_nodes.count(nid)) n->pl().setTurnCycle();
  };

  for (const auto& kv : station_nodes) {
    auto nid = kv.first;
    const auto& meta = kv.second;
    // compute StatInfo via OsmBuilder helper to follow existing rules
    auto si = helper.getStatInfo(nid, meta.attrs, node_rels, rels, opts);
    if (!si.isNull()) {
      if (nodes_map.count(nid)) {
        nodes_map[nid]->pl().setSI(si);
        apply_node_flags(nid, nodes_map[nid]);
      }
      if (mult_nodes.count(nid)) {
        for (auto* n : mult_nodes[nid]) { n->pl().setSI(si); apply_node_flags(nid, n); }
      }
      if (!nodes_map.count(nid) && !mult_nodes.count(nid)) {
        auto* tmp = g->addNd(pfaedle::trgraph::NodePL(meta.pt));
        tmp->pl().setSI(si);
        orphan_stations.insert(tmp);
      }
    }
  }
  // Release station metadata
  station_nodes.clear();
  station_nodes.rehash(0);

  // Post-processing: identical sequence to XML path
  {
    pfaedle::osm::NodeGrid ng = pfaedle::osm::OsmBuilder::buildNodeIdx(
        g, gridSize, bbox.getFullBox(), false);
    pfaedle::osm::OsmBuilder::fixGaps(g, &ng);
  }

  pfaedle::osm::OsmBuilder::snapStats(opts, g, bbox, gridSize, res, orphan_stations);
  pfaedle::osm::OsmBuilder::collapseEdges(g);
  pfaedle::osm::OsmBuilder::writeGeoms(g, opts);
  pfaedle::osm::OsmBuilder::deleteOrphNds(g, opts);
  (void)pfaedle::osm::OsmBuilder::writeComps(g, opts);
  pfaedle::osm::OsmBuilder::simplifyGeoms(g);
  pfaedle::osm::OsmBuilder::writeODirEdgs(g, res);
  pfaedle::osm::OsmBuilder::writeOneWayPens(g, opts);
  if (opts.noLinesPunishFact != 1.0) {
    pfaedle::osm::OsmBuilder::writeNoLinePens(g, opts);
  }
  pfaedle::osm::OsmBuilder::writeSelfEdgs(g);

  // Release large temporary structures early
  {
    // Clear relation and restriction structures now that they are applied
    RelLst().rels.swap(rels.rels);
    rels.flat.clear();
    node_rels.clear();
    way_rels.clear();
    raw_rests.pos.clear();
    raw_rests.neg.clear();
  }
#endif
}

void OsmPbfReader::convertToPbf(const std::string& inPath,
                                const std::string& outPath) {
#ifndef OSMIUM_ENABLED
  throw std::runtime_error("PBF conversion not available: libosmium/protozero headers not found at build time");
#else
  osmium::io::Reader reader(inPath);
  osmium::io::Header header;
  header.set("generator", std::string("pfaedle/") + VERSION_FULL);
  osmium::io::Writer writer(outPath, header, osmium::io::overwrite::allow);
  osmium::apply(reader, std::ref(writer));
  writer.close();
  reader.close();
#endif
}

void OsmPbfReader::convertToXml(const std::string& inPath, const std::string& outPath) {
#ifndef OSMIUM_ENABLED
  throw std::runtime_error("XML conversion not available: libosmium/protozero headers not found at build time");
#else
  osmium::io::Reader reader(inPath);
  osmium::io::Header header;
  header.set("generator", std::string("pfaedle/") + VERSION_FULL);
  // Rely on .osm extension in outPath to select XML format
  osmium::io::Writer writer(outPath, header, osmium::io::overwrite::allow);
  osmium::apply(reader, std::ref(writer));
  writer.close();
  reader.close();
#endif
}

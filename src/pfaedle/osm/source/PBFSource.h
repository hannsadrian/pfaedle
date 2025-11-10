// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef PFAEDLE_OSM_SOURCE_PBFSOURCE_H_
#define PFAEDLE_OSM_SOURCE_PBFSOURCE_H_

#include <memory>
#include <vector>
#include <string>
#include <osmium/io/any_input.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/relation.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>

#include "pfaedle/osm/source/OsmSource.h"
#include "pfaedle/osm/Osm.h"
#include "util/geo/Geo.h"

namespace pfaedle {
namespace osm {
namespace source {

class PBFSource : public OsmSource {
 public:
  using LocationIndex = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
  
  PBFSource(const std::string& path);
  virtual ~PBFSource();
  
  virtual const OsmSourceNode* nextNode();
  virtual const OsmSourceAttr nextAttr();
  virtual const OsmSourceWay* nextWay();
  virtual uint64_t nextMemberNode();
  virtual const OsmSourceRelationMember* nextMember();
  virtual const OsmSourceRelation* nextRel();
  virtual bool cont() { return true; }

  virtual void seekNodes();
  virtual void seekWays();
  virtual void seekRels();

  virtual util::geo::Box<double> getBounds();

  virtual std::string decode(const char* str) const;
  virtual std::string decode(const std::string& str) const;
  
  // Build location index for nodes in the given bounding box
  void buildLocationIndex(const util::geo::Box<double>& bbox);
  
  // Check if a node location is available in the index
  bool hasNodeLocation(osmid id) const;
  
  // Get node location from the index
  bool getNodeLocation(osmid id, double* lat, double* lon) const;

 private:
  std::string _path;
  std::unique_ptr<osmium::io::Reader> _reader;
  osmium::memory::Buffer _buffer;
  osmium::memory::Buffer::iterator _bufferIt;
  
  const osmium::Node* _curNode;
  const osmium::Way* _curWay;
  const osmium::Relation* _curRel;
  
  osmium::TagList::const_iterator _curTagIt;
  osmium::TagList::const_iterator _curTagEnd;
  osmium::WayNodeList::const_iterator _curWayNodeIt;
  osmium::WayNodeList::const_iterator _curWayNodeEnd;
  osmium::RelationMemberList::const_iterator _curRelMemberIt;
  osmium::RelationMemberList::const_iterator _curRelMemberEnd;
  
  OsmSourceNode _retNode;
  OsmSourceWay _retWay;
  OsmSourceRelation _retRel;
  OsmSourceRelationMember _retMember;
  OsmSourceAttr _retAttr;
  
  util::geo::Box<double> _bbox;
  
  // Location index for fast node coordinate lookup
  std::unique_ptr<LocationIndex> _locationIndex;
  
  void readNextBuffer();
  void advanceToNextEntity(osmium::item_type type);
  void initReader();
};

}  // namespace source
}  // namespace osm
}  // namespace pfaedle

#endif  // PFAEDLE_OSM_SOURCE_PBFSOURCE_H_

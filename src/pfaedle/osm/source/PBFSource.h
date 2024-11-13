// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef PFAEDLE_OSM_SOURCE_PBFSOURCE_H_
#define PFAEDLE_OSM_SOURCE_PBFSOURCE_H_

#include "pfaedle/osm/source/OsmSource.h"


namespace pfaedle {
namespace osm {
namespace source {

class PBFSource : public OsmSource {
 public:
  PBFSource(const std::string& path);
  virtual const OsmSourceNode* nextNode();
  virtual const OsmSourceAttr nextAttr();
  virtual const OsmSourceWay* nextWay();
  virtual uint64_t nextMemberNode();
  virtual const OsmSourceRelationMember* nextMember();
  virtual const OsmSourceRelation* nextRel();
  virtual void cont();

  virtual void seekNodes();
  virtual void seekWays();
  virtual void seekRels();

  virtual util::geo::Box<double> getBounds();

  virtual std::string decode(const char* str) const;
  virtual std::string decode(const std::string& str) const;
 private:
  std::string _path;
};

}  // namespace source
}  // namespace osm
}  // namespace pfaedle

#endif

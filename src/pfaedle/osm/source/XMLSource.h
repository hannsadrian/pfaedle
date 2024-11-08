// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef PFAEDLE_OSM_SOURCE_XMLSOURCE_H_
#define PFAEDLE_OSM_SOURCE_XMLSOURCE_H_

#include "pfaedle/osm/source/OsmSource.h"
#include "pfxml/pfxml.h"


namespace pfaedle {
namespace osm {
namespace source {

class XMLSource : public OsmSource {
 public:
  XMLSource(const std::string& path);
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
 private:
  std::string _path;
  OsmSourceNode _curNode;
  OsmSourceWay _curWay;
  OsmSourceRelation _curRel;
  OsmSourceRelationMember _curMember;

  pfxml::file _xml;

  bool _inNodeBlock = false;
  bool _inWayBlock = false;
  bool _inRelBlock = false;
};

}  // namespace source
}  // namespace osm
}  // namespace pfaedle

#endif

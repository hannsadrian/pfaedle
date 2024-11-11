// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include "pfaedle/osm/source/XMLSource.h"
#include "pfxml/pfxml.h"
#include "util/Misc.h"

using pfaedle::osm::source::XMLSource;
using pfaedle::osm::source::OsmSourceNode;
using pfaedle::osm::source::OsmSourceWay;
using pfaedle::osm::source::OsmSourceRelation;
using pfaedle::osm::source::OsmSourceRelationMember;
using pfaedle::osm::source::OsmSourceAttr;

// _____________________________________________________________________________
XMLSource::XMLSource(const std::string& path) : _path(path), _xml(path) {
}

// _____________________________________________________________________________
const OsmSourceNode* XMLSource::nextNode() {
  do {
    const pfxml::tag& cur = _xml.get();

    if (_xml.level() != 2) continue;
    if (!_inNodeBlock && strcmp(cur.name, "node") == 0) _inNodeBlock = true;

    if (_inNodeBlock) {
      // block ended
      if (strcmp(cur.name, "node")) return 0;
      _curNode.lon = util::atof(cur.attr("lon"), 7);
      _curNode.lat = util::atof(cur.attr("lat"), 7);

      _curNode.id = util::atoul(cur.attr("id"));

      return &_curNode;
    }

  } while (_xml.next());

  return 0;
}

// _____________________________________________________________________________
void XMLSource::seekNodes() {
  _xml.reset();
  while (_xml.next() && strcmp(_xml.get().name, "node")) {}
}

// _____________________________________________________________________________
void XMLSource::seekWays() {
  _xml.reset();
  while (_xml.next() && strcmp(_xml.get().name, "way")) {}
}

// _____________________________________________________________________________
void XMLSource::seekRels() {
  _xml.reset();
  while (_xml.next() && strcmp(_xml.get().name, "relation")) {}
}

// _____________________________________________________________________________
void XMLSource::cont() {
  _xml.next();
}

// _____________________________________________________________________________
const OsmSourceWay* XMLSource::nextWay() {
  do {
    const pfxml::tag& cur = _xml.get();

    if (_xml.level() != 2) continue;
    if (!_inWayBlock && strcmp(cur.name, "way") == 0) _inWayBlock = true;

    if (_inWayBlock) {
      // block ended
      if (strcmp(cur.name, "way")) return 0;
      _curWay.id = util::atoul(cur.attr("id"));

      return &_curWay;
    }

  } while (_xml.next());

  return 0;
}

// _____________________________________________________________________________
const OsmSourceRelationMember* XMLSource::nextMember() {
  do {
    const pfxml::tag& cur = _xml.get();

    if (_xml.level() != 3) return 0;

    if (strcmp(cur.name, "member") == 0) {
      _curMember.id = util::atoul(cur.attr("ref"));
      _curMember.type = 0;
      if (strcmp(cur.attr("type"), "way") == 0) _curMember.type = 1;
      else if (strcmp(cur.attr("type"), "relation") == 0) _curMember.type = 2;
      _curMember.role = cur.attr("role");
      if (!_curMember.role) _curMember.role = "";
      return &_curMember;
    } else {
      return 0;
    }
  } while (_xml.next());

  return 0;
}

// _____________________________________________________________________________
uint64_t XMLSource::nextMemberNode() {
  do {
    const pfxml::tag& cur = _xml.get();

    if (_xml.level() != 3) return 0;

    if (strcmp(cur.name, "nd") == 0) {
      return util::atoul(cur.attr("ref"));
    } else {
      return 0;
    }
  } while (_xml.next());

  return 0;
}

// _____________________________________________________________________________
const OsmSourceRelation* XMLSource::nextRel() {
  do {
    const pfxml::tag& cur = _xml.get();

    if (_xml.level() != 2) continue;
    if (!_inRelBlock && strcmp(cur.name, "relation") == 0) _inRelBlock = true;

    if (_inRelBlock) {
      // block ended
      if (strcmp(cur.name, "relation")) return 0;
      _curRel.id = util::atoul(cur.attr("id"));

      return &_curRel;
    }

  } while (_xml.next());

  return 0;
}

// _____________________________________________________________________________
const OsmSourceAttr XMLSource::nextAttr() {
  do {
    const pfxml::tag& cur = _xml.get();

    if (_xml.level() != 3) return {0, 0};

    if (strcmp(cur.name, "tag") == 0) {
      return {cur.attr("k"), cur.attr("v")};
    } else {
      return {0, 0};
    }
  } while (_xml.next());

  return {0, 0};
}

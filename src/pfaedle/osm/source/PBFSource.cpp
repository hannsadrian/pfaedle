// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include "pfaedle/osm/source/PBFSource.h"
#include "util/Misc.h"

using pfaedle::osm::source::PBFSource;
using pfaedle::osm::source::OsmSourceNode;
using pfaedle::osm::source::OsmSourceWay;
using pfaedle::osm::source::OsmSourceRelation;
using pfaedle::osm::source::OsmSourceRelationMember;
using pfaedle::osm::source::OsmSourceAttr;

// _____________________________________________________________________________
PBFSource::PBFSource(const std::string& path) : _path(path) {
  std::cout << "Init PBF source" << std::endl;
}

// _____________________________________________________________________________
const OsmSourceNode* PBFSource::nextNode() {
  return 0;
}

// _____________________________________________________________________________
void PBFSource::seekNodes() {
}

// _____________________________________________________________________________
void PBFSource::seekWays() {
}

// _____________________________________________________________________________
void PBFSource::seekRels() {
}

// _____________________________________________________________________________
void PBFSource::cont() {
}

// _____________________________________________________________________________
const OsmSourceWay* PBFSource::nextWay() {
  return 0;
}

// _____________________________________________________________________________
const OsmSourceRelationMember* PBFSource::nextMember() {
  return 0;
}

// _____________________________________________________________________________
uint64_t PBFSource::nextMemberNode() {
  return 0;
}

// _____________________________________________________________________________
const OsmSourceRelation* PBFSource::nextRel() {
  return 0;
}

// _____________________________________________________________________________
const OsmSourceAttr PBFSource::nextAttr() {
}


// _____________________________________________________________________________
util::geo::Box<double> PBFSource::getBounds() {
}

// _____________________________________________________________________________
std::string PBFSource::decode(const char* str) const {
  return str;  // TODO
}

// _____________________________________________________________________________
std::string PBFSource::decode(const std::string& str) const {
  return str;  // TODO
}

// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_LEVEL_H_
#define AD_CPPGTFS_GTFS_LEVEL_H_

#include <string>
#include "flat/Level.h"

using std::exception;
using std::string;

namespace ad {
namespace cppgtfs {
namespace gtfs {

class Level {
 public:
  typedef Level* Ref;
  static std::string getId(Ref r) { return r->getId(); }

  Level() {}

  Level(const std::string& id, double index, const std::string& name)
      : _id(id),
        _name(name),
        _index(index) {}

  const std::string& getName() const { return _name; }

  double getIndex() const { return _index; }

  const std::string& getId() const { return _id; }

  flat::Level getFlat() const {
    flat::Level r;
    r.id = _id;
    r.name = _name;
    r.index = _index;
    return r;
  }

  // TODO(patrick): implement setters

 private:
  std::string _id, _name;
  double _index;
};

}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_LEVEL_H_

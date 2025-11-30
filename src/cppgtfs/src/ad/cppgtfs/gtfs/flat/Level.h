// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FLAT_LEVEL_H_
#define AD_CPPGTFS_GTFS_FLAT_LEVEL_H_

#include <string>

using std::exception;
using std::string;

namespace ad {
namespace cppgtfs {
namespace gtfs {
namespace flat {

struct LevelFlds {
  size_t levelNameFld;
  size_t levelIdFld;
  size_t levelIndexFld;
};

struct Level {
  std::string id, name;
  double index;
};

}  // namespace flat
}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_FLAT_LEVEL_H_

// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FLAT_PATHWAY_H_
#define AD_CPPGTFS_GTFS_FLAT_PATHWAY_H_

#include <stdint.h>

#include <string>

using std::exception;
using std::string;

namespace ad {
namespace cppgtfs {
namespace gtfs {
namespace flat {

struct PathwayFlds {
  size_t pathwayIdFld;
  size_t fromStopIdFld;
  size_t toStopIdFld;
  size_t pathwayModeFld;
  size_t isBidirectionalFld;
  size_t lengthFld;
  size_t traverselTimeFld;
  size_t stairCountFld;
  size_t maxSlopeFld;
  size_t minWidthFld;
  size_t signPostedAsFld;
  size_t reversedSignPostedAsFld;
};

struct Pathway {
  std::string id;
  std::string from_stop_id;
  std::string to_stop_id;
  uint8_t pathway_mode;
  bool is_bidirectional;
  double length;
  int64_t traversal_time;
  int64_t stair_count;
  double max_slope;
  double min_width;
  std::string signposted_as;
  std::string reversed_signposted_as;
};

}  // namespace flat
}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_FLAT_PATHWAY_H_

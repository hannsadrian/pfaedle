// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FLAT_STOP_H_
#define AD_CPPGTFS_GTFS_FLAT_STOP_H_

#include <stdint.h>

#include <cassert>
#include <string>
#include <vector>

namespace ad {
namespace cppgtfs {
namespace gtfs {
namespace flat {

struct StopFlds {
  size_t stopIdFld;
  size_t stopNameFld;
  size_t stopLatFld;
  size_t stopLonFld;
  size_t parentStationFld;
  size_t stopCodeFld;
  size_t stopDescFld;
  size_t zoneIdFld;
  size_t stopUrlFld;
  size_t stopTimezoneFld;
  size_t wheelchairBoardingFld;
  size_t locationTypeFld;
  size_t platformCodeFld;
  size_t levelIdFld;
  std::vector<size_t> addHeaders;
};

struct Stop {
  enum LOCATION_TYPE : uint8_t {
    STOP = 0,
    STATION = 1,
    STATION_ENTRANCE = 2,
    GENERIC_NODE = 3,
    BOARDING_AREA = 4
  };

  enum WHEELCHAIR_BOARDING : uint8_t {
    NO_INFORMATION = 0,
    BOARDING_POSSIBLE = 1,
    BOARDING_NOT_POSSIBLE = 2
  };

  std::string id, code, name, desc, zone_id, stop_url, stop_timezone,
      platform_code, parent_station, level_id;
  float lat, lng;
  Stop::WHEELCHAIR_BOARDING wheelchair_boarding;
  Stop::LOCATION_TYPE location_type;
};

}  // namespace flat
}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_FLAT_STOP_H_

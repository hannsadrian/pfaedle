// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FLAT_TRANSFER_H_
#define AD_CPPGTFS_GTFS_FLAT_TRANSFER_H_

#include <stdint.h>

#include <string>
#include "../StopTime.h"

namespace ad {
namespace cppgtfs {
namespace gtfs {
namespace flat {

struct TransfersFlds {
  size_t fromStopIdFld;
  size_t toStopIdFld;
  size_t fromRouteIdFld;
  size_t toRouteIdFld;
  size_t fromTripIdFld;
  size_t toTripIdFld;
  size_t transferTypeFld;
  size_t minTransferTimeFld;
};

struct Transfer {
  enum TYPE : uint8_t {
    RECOMMENDED = 0,
    TIMED = 1,
    MIN_TIME = 2,
    NO_TRANSFER = 3,
    IN_SEAT = 4,
    NO_IN_SEAT = 5
  };

  std::string fromStop;
  std::string toStop;
  std::string fromRoute;
  std::string toRoute;
  std::string fromTrip;
  std::string toTrip;
  TYPE type;
  int32_t tTime;
};

}  // namespace flat
}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_FLAT_TRANSFER_H_

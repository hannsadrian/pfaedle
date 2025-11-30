// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FLAT_ATTRIBUTION_H_
#define AD_CPPGTFS_GTFS_FLAT_ATTRIBUTION_H_

#include <stdint.h>

#include <string>

namespace ad {
namespace cppgtfs {
namespace gtfs {
namespace flat {

struct AttributionsFlds {
  size_t attributionIdFld;
  size_t agencyIdFld;
  size_t routeIdFld;
  size_t tripIdFld;
  size_t organizationNameFld;
  size_t isProducerFld;
  size_t isOperatorFld;
  size_t isAuthorityFld;
  size_t attributionUrlFld;
  size_t attributionMailFld;
  size_t attributionPhoneFld;
  // std::vector<size_t> addHeaders;
};

struct Attribution {
  enum TYPE : uint8_t { NOT_ROLE = 0, ROLE = 1 };

  std::string attributionId;
  std::string agencyId;
  std::string routeId;
  std::string tripId;
  std::string organizationName;
  TYPE isProducer;
  TYPE isOperator;
  TYPE isAuthority;
  std::string attributionUrl;
  std::string attributionEmail;
  std::string attributionPhone;
};

}  // namespace flat
}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_FLAT_ATTRIBUTION_H_

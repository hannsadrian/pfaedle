// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_ATTRIBUTION_H_
#define AD_CPPGTFS_GTFS_ATTRIBUTION_H_

#include <stdint.h>

#include <string>

#include "Agency.h"
#include "Route.h"
#include "Trip.h"
#include "flat/Attribution.h"

using std::exception;
using std::string;

namespace ad {
namespace cppgtfs {
namespace gtfs {
template <typename StopT, template <typename> class StopTimeT,
          typename ServiceT, typename RouteT, typename ShapeT>
class Attribution {
 public:
  typedef flat::Attribution::TYPE TYPE;
  Attribution() {}

  Attribution(const std::string& attributionId, Agency* agency, Route* route,
              TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* trip,
              const std::string& organizationName, TYPE isProducer,
              TYPE isOperator, TYPE isAuthority,
              const std::string& attributionUrl,
              const std::string& attributionEmail,
              const std::string& attributionPhone)
      : _attributionId(attributionId),
        _agency(agency),
        _route(route),
        _trip(trip),
        _organizationName(organizationName),
        _isProducer(isProducer),
        _isOperator(isOperator),
        _isAuthority(isAuthority),
        _attributionUrl(attributionUrl),
        _attributionEmail(attributionEmail),
        _attributionPhone(attributionPhone) {}

  Route* getRoute() const { return _route; }

  Agency* getAgency() const { return _agency; }

  TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* getTrip() const {
    return _trip;
  }

  TYPE getIsProducer() const { return _isProducer; }
  TYPE getIsOperator() const { return _isOperator; }
  TYPE getIsAuthority() const { return _isAuthority; }

  const std::string& getAttributionId() const { return _attributionId; }
  const std::string& getAttributionUrl() const { return _attributionUrl; }
  const std::string& getAttributionEmail() const { return _attributionEmail; }
  const std::string& getAttributionPhone() const { return _attributionPhone; }

  flat::Attribution getFlat() const {
    std::string agency, route, trip;

    if (getAgency()) agency = getAgency()->getId();
    if (getRoute()) route = getRoute()->getId();
    if (getTrip()) trip = getTrip()->getId();

    return flat::Attribution{_attributionId,
                             agency,
                             route,
                             trip,
                             _organizationName,
                             _isProducer,
                             _isOperator,
                             _isAuthority,
                             _attributionUrl,
                             _attributionEmail,
                             _attributionPhone};
  }

  // TODO(patrick): implement setters

 private:
  std::string _attributionId;
  Agency* _agency;
  Route* _route;
  TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* _trip;
  const std::string& _organizationName;
  TYPE _isProducer;
  TYPE _isOperator;
  TYPE _isAuthority;
  const std::string& _attributionUrl;
  const std::string& _attributionEmail;
  const std::string& _attributionPhone;
};

}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_ATTRIBUTION_H_

// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_TRANSFER_H_
#define AD_CPPGTFS_GTFS_TRANSFER_H_

#include <stdint.h>

#include <string>

#include "Route.h"
#include "Stop.h"
#include "Trip.h"
#include "flat/Transfer.h"

using std::exception;
using std::string;

namespace ad {
namespace cppgtfs {
namespace gtfs {
template <typename StopT, template <typename> class StopTimeT,
          typename ServiceT, typename RouteT, typename ShapeT>
class Transfer {
 public:
  typedef flat::Transfer::TYPE TYPE;
  Transfer() {}

  Transfer(Stop* fromStop, Stop* toStop, Route* fromRoute, Route* toRoute,
           TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* fromTrip,
           TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* toTrip, TYPE type,
           int32_t tTime)
      : _fromStop(fromStop),
        _toStop(toStop),
        _fromRoute(fromRoute),
        _toRoute(toRoute),
        _fromTrip(fromTrip),
        _toTrip(toTrip),
        _type(type),
        _tTime(tTime) {}

  Stop* getFromStop() const { return _fromStop; }

  Stop* getToStop() const { return _toStop; }

  Route* getFromRoute() const { return _fromRoute; }

  Route* getToRoute() const { return _toRoute; }

  TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* getFromTrip() const {
    return _fromTrip;
  }

  TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* getToTrip() const {
    return _toTrip;
  }

  TYPE getType() const { return _type; }

  int32_t getMinTransferTime() const { return _tTime; }

  flat::Transfer getFlat() const {
    std::string fromStop, toStop, fromRoute, toRoute, fromTrip, toTrip;

    if (getFromStop()) fromStop = getFromStop()->getId();
    if (getToStop()) toStop = getToStop()->getId();

    if (getFromRoute()) fromRoute = getFromRoute()->getId();
    if (getToRoute()) toRoute = getToRoute()->getId();

    if (getFromTrip()) fromTrip = getFromTrip()->getId();
    if (getToTrip()) toTrip = getToTrip()->getId();

    return flat::Transfer{fromStop, toStop, fromRoute, toRoute,
                          fromTrip, toTrip, getType(), getMinTransferTime()};
  }

  // TODO(patrick): implement setters

 private:
  StopT* _fromStop;
  StopT* _toStop;
  RouteT* _fromRoute;
  RouteT* _toRoute;
  TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* _fromTrip;
  TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* _toTrip;
  TYPE _type;
  int32_t _tTime;
};

}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_TRANSFER_H_

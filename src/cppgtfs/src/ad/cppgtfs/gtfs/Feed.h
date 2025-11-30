// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FEED_H_
#define AD_CPPGTFS_GTFS_FEED_H_

#include <stdint.h>

#include <iterator>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "Agency.h"
#include "ContContainer.h"
#include "Container.h"
#include "Fare.h"
#include "Level.h"
#include "Pathway.h"
#include "Route.h"
#include "Service.h"
#include "Shape.h"
#include "Stop.h"
#include "Transfer.h"
#include "Trip.h"
#include "Attribution.h"
#include "Translation.h"

#define FEEDTPL                                                                \
  template <typename AgencyT, typename RouteT, typename StopT,                 \
            typename ServiceT, template <typename> class StopTimeT,            \
            typename ShapeT, template <typename> class FareT, typename LevelT, \
            typename PathwayT, template <typename> class AContainerT,          \
            template <typename> class RContainerT,                             \
            template <typename> class SContainerT,                             \
            template <typename> class StContainerT,                            \
            template <typename> class TContainerT,                             \
            template <typename> class ShContainerT,                            \
            template <typename> class FContainerT,                             \
            template <typename> class LContainerT,                             \
            template <typename> class PContainerT>
#define FEEDB                                                               \
  FeedB<AgencyT, RouteT, StopT, ServiceT, StopTimeT, ShapeT, FareT, LevelT, \
        PathwayT, AContainerT, RContainerT, SContainerT, StContainerT,      \
        TContainerT, ShContainerT, FContainerT, LContainerT, PContainerT>

namespace ad {
namespace cppgtfs {
namespace gtfs {

typedef std::unordered_map<std::string,
                           std::unordered_map<std::string, std::string>>
    AddFlds;

FEEDTPL
class FeedB {
  typedef AContainerT<AgencyT> Agencies;
  typedef StContainerT<StopT> Stops;
  typedef RContainerT<RouteT> Routes;
  typedef TContainerT<TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>> Trips;
  typedef ShContainerT<ShapeT> Shapes;
  typedef SContainerT<ServiceT> Services;
  typedef FContainerT<FareT<RouteT>> Fares;
  typedef LContainerT<LevelT> Levels;
  typedef PContainerT<PathwayT> Pathways;
  typedef std::vector<Transfer<StopT, StopTimeT, ServiceT, RouteT, ShapeT>>
      Transfers;
  typedef std::vector<Attribution<StopT, StopTimeT, ServiceT, RouteT, ShapeT>>
      Attributions;
  typedef std::vector<Translation> Translations;
  typedef std::set<std::string> Zones;

 public:
  FeedB()
      : _maxLat(std::numeric_limits<double>::lowest()),
        _maxLon(std::numeric_limits<double>::lowest()),
        _minLat(std::numeric_limits<double>::max()),
        _minLon(std::numeric_limits<double>::max()) {}

  const Agencies& getAgencies() const;
  Agencies& getAgencies();

  const Stops& getStops() const;
  Stops& getStops();

  const Routes& getRoutes() const;
  Routes& getRoutes();

  const Trips& getTrips() const;
  Trips& getTrips();

  const Shapes& getShapes() const;
  Shapes& getShapes();
  const Services& getServices() const;
  Services& getServices();

  const Transfers& getTransfers() const;
  Transfers& getTransfers();

  const Attributions& getAttributions() const;
  Attributions& getAttributions();

  const Translations& getTranslations() const;
  Translations& getTranslations();

  const Zones& getZones() const;
  Zones& getZones();

  const Fares& getFares() const;
  Fares& getFares();

  const Levels& getLevels() const;
  Levels& getLevels();

  const Pathways& getPathways() const;
  Pathways& getPathways();

  const std::string& getPublisherName() const;
  const std::string& getPublisherUrl() const;
  const std::string& getLang() const;
  const std::string& getVersion() const;
  const ServiceDate& getStartDate() const;
  const ServiceDate& getEndDate() const;
  const std::string& getContactEmail() const;
  const std::string& getContactUrl() const;
  const std::string& getDefaultLang() const;

  void setPublisherName(const std::string& name);
  void setPublisherUrl(const std::string& url);
  void setLang(const std::string& lang);
  void setVersion(const std::string& version);
  void setStartDate(const ServiceDate& start);
  void setEndDate(const ServiceDate& end);
  void setContactEmail(const std::string& email);
  void setContactUrl(const std::string& url);
  void setDefaultLang(const std::string& lang);

  void updateBox(double lat, double lon);
  double getMinLat() const;
  double getMinLon() const;
  double getMaxLat() const;
  double getMaxLon() const;

  const std::string& getPath() const { return _path; }
  void setPath(const std::string& p) { _path = p; }

  void addTripAddFld(const std::string& id, const std::string& name,
                     const std::string& val);
  void addRouteAddFld(const std::string& id, const std::string& name,
                      const std::string& val);
  void addStopAddFld(const std::string& id, const std::string& name,
                     const std::string& val);
  void addAgencyAddFld(const std::string& id, const std::string& name,
                       const std::string& val);

  const AddFlds& getTripAddFlds() const { return _tripAddFields; }
  const AddFlds& getRouteAddFlds() const { return _routeAddFields; }
  const AddFlds& getStopAddFlds() const { return _stopAddFields; }
  const AddFlds& getAgencyAddFlds() const { return _agencyAddFields; }

 private:
  Agencies _agencies;
  Stops _stops;
  Routes _routes;
  Trips _trips;
  Shapes _shapes;
  Services _services;
  Transfers _transfers;
  Attributions _attributions;
  Translations _translations;
  Zones _zones;
  Fares _fares;
  Levels _levels;
  Pathways _pathways;

  double _maxLat, _maxLon, _minLat, _minLon;

  std::string _publisherName, _publisherUrl, _lang, _version, _path,
      _contactMail, _contactUrl, _defaultLang;
  ServiceDate _startDate, _endDate;

  AddFlds _tripAddFields;
  AddFlds _routeAddFields;
  AddFlds _stopAddFields;
  AddFlds _agencyAddFields;
};

typedef FeedB<Agency, Route, Stop, Service, StopTime, Shape, Fare, Level,
              Pathway, Container, Container, Container, Container, Container,
              Container, Container, Container, Container>
    Feed;
typedef FeedB<Agency, Route, Stop, Service, StopTime, Shape, Fare, Level,
              Pathway, ContContainer, ContContainer, ContContainer,
              ContContainer, ContContainer, ContContainer, ContContainer,
              ContContainer, ContContainer>
    ContFeed;

#include "Feed.tpp"

}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_FEED_H_

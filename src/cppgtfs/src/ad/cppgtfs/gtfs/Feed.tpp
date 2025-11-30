// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Agencies& FEEDB::getAgencies() const { return _agencies; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Agencies& FEEDB::getAgencies() { return _agencies; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Levels& FEEDB::getLevels() const { return _levels; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Levels& FEEDB::getLevels() { return _levels; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Pathways& FEEDB::getPathways() const { return _pathways; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Pathways& FEEDB::getPathways() { return _pathways; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Stops& FEEDB::getStops() const { return _stops; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Stops& FEEDB::getStops() { return _stops; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Routes& FEEDB::getRoutes() const { return _routes; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Routes& FEEDB::getRoutes() { return _routes; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Trips& FEEDB::getTrips() const { return _trips; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Trips& FEEDB::getTrips() { return _trips; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Shapes& FEEDB::getShapes() const { return _shapes; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Shapes& FEEDB::getShapes() { return _shapes; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Services& FEEDB::getServices() const { return _services; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Services& FEEDB::getServices() { return _services; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Transfers& FEEDB::getTransfers() const {
  return _transfers;
}

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Transfers& FEEDB::getTransfers() { return _transfers; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Attributions& FEEDB::getAttributions() const {
  return _attributions;
}

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Attributions& FEEDB::getAttributions() { return _attributions; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Translations& FEEDB::getTranslations() const {
  return _translations;
}

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Translations& FEEDB::getTranslations() { return _translations; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Zones& FEEDB::getZones() const { return _zones; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Zones& FEEDB::getZones() { return _zones; }

// ____________________________________________________________________________
FEEDTPL
const typename FEEDB::Fares& FEEDB::getFares() const { return _fares; }

// ____________________________________________________________________________
FEEDTPL
typename FEEDB::Fares& FEEDB::getFares() { return _fares; }

// ____________________________________________________________________________
FEEDTPL
const std::string& FEEDB::getPublisherName() const { return _publisherName; }

// ____________________________________________________________________________
FEEDTPL
const std::string& FEEDB::getPublisherUrl() const { return _publisherUrl; }

// ____________________________________________________________________________
FEEDTPL
const std::string& FEEDB::getLang() const { return _lang; }

// ____________________________________________________________________________
FEEDTPL
const std::string& FEEDB::getVersion() const { return _version; }

// ____________________________________________________________________________
FEEDTPL
const ServiceDate& FEEDB::getStartDate() const { return _startDate; }

// ____________________________________________________________________________
FEEDTPL
const ServiceDate& FEEDB::getEndDate() const { return _endDate; }

// ____________________________________________________________________________
FEEDTPL
const std::string& FEEDB::getContactEmail() const { return _contactMail; }

// ____________________________________________________________________________
FEEDTPL
const std::string& FEEDB::getContactUrl() const { return _contactUrl; }

// ____________________________________________________________________________
FEEDTPL
const std::string& FEEDB::getDefaultLang() const { return _defaultLang; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setDefaultLang(const std::string& l) { _defaultLang = l; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setContactEmail(const std::string& email) { _contactMail = email; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setContactUrl(const std::string& url) { _contactUrl = url; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setPublisherName(const std::string& name) { _publisherName = name; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setPublisherUrl(const std::string& url) { _publisherUrl = url; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setLang(const std::string& lang) { _lang = lang; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setVersion(const std::string& version) { _version = version; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setStartDate(const ServiceDate& start) { _startDate = start; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::setEndDate(const ServiceDate& end) { _endDate = end; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::updateBox(double lat, double lon) {
  if (lat > _maxLat) _maxLat = lat;
  if (lon > _maxLon) _maxLon = lon;
  if (lat < _minLat) _minLat = lat;
  if (lon < _minLon) _minLon = lon;
}

// ____________________________________________________________________________
FEEDTPL
double FEEDB::getMinLat() const { return _minLat; }

// ____________________________________________________________________________
FEEDTPL
double FEEDB::getMinLon() const { return _minLon; }

// ____________________________________________________________________________
FEEDTPL
double FEEDB::getMaxLat() const { return _maxLat; }

// ____________________________________________________________________________
FEEDTPL
double FEEDB::getMaxLon() const { return _maxLon; }

// ____________________________________________________________________________
FEEDTPL
void FEEDB::addTripAddFld(const std::string& id, const std::string& name,
                          const std::string& val) {
  _tripAddFields[name][id] = val;
}

// ____________________________________________________________________________
FEEDTPL
void FEEDB::addRouteAddFld(const std::string& id, const std::string& name,
                           const std::string& val) {
  _routeAddFields[name][id] = val;
}

// ____________________________________________________________________________
FEEDTPL
void FEEDB::addStopAddFld(const std::string& id, const std::string& name,
                          const std::string& val) {
  _stopAddFields[name][id] = val;
}

// ____________________________________________________________________________
FEEDTPL
void FEEDB::addAgencyAddFld(const std::string& id, const std::string& name,
                            const std::string& val) {
  _agencyAddFields[name][id] = val;
}

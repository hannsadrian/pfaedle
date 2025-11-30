// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

// ____________________________________________________________________________
FEEDTPL bool Parser::parse(gtfs::FEEDB* targetFeed) const {
  std::ifstream fs;
  std::string curFile;

  targetFeed->setPath(_path);

  parseFeedInfo(targetFeed);
  parseAgencies(targetFeed);
  parseLevels(targetFeed);
  parseStops(targetFeed);
  parseRoutes(targetFeed);
  parseCalendar(targetFeed);
  parseCalendarDates(targetFeed);
  parseShapes(targetFeed);
  parseTrips(targetFeed);
  parseStopTimes(targetFeed);
  parseFrequencies(targetFeed);
  parseTransfers(targetFeed);
  parseAttributions(targetFeed);
  parseFareAttributes(targetFeed);
  parseFareRules(targetFeed);
  parsePathways(targetFeed);
  parseTranslations(targetFeed);

  return true;
}

// ____________________________________________________________________________
inline gtfs::flat::TranslationFlds Parser::getTranslationFlds(CsvParser* csvp) {
  gtfs::flat::TranslationFlds t;
  t.tableFld = csvp->getFieldIndex("table_name");
  t.fieldNameFld = csvp->getFieldIndex("field_name");
  t.languageFld = csvp->getFieldIndex("language");
  t.translationFld = csvp->getFieldIndex("translation");
  t.recordIdFld = csvp->getOptFieldIndex("record_id");
  t.recordSubIdFld = csvp->getOptFieldIndex("record_sub_id");
  t.fieldValueFld = csvp->getOptFieldIndex("field_value");

  return t;
}

// ____________________________________________________________________________
inline bool Parser::nextTranslation(
    CsvParser* csvp, gtfs::flat::Translation* t,
    const gtfs::flat::TranslationFlds& flds) const {
  if (csvp->readNextLine()) {
    std::string table = getString(*csvp, flds.tableFld, "");
    if (table == "agency")
      t->table = gtfs::flat::Translation::TABLE::AGENCY;
    else if (table == "stops")
      t->table = gtfs::flat::Translation::TABLE::STOPS;
    else if (table == "routes")
      t->table = gtfs::flat::Translation::TABLE::ROUTES;
    else if (table == "trips")
      t->table = gtfs::flat::Translation::TABLE::TRIPS;
    else if (table == "stop_times")
      t->table = gtfs::flat::Translation::TABLE::STOP_TIMES;
    else if (table == "pathways")
      t->table = gtfs::flat::Translation::TABLE::PATHWAYS;
    else if (table == "levels")
      t->table = gtfs::flat::Translation::TABLE::LEVELS;
    else if (table == "feed_info")
      t->table = gtfs::flat::Translation::TABLE::FEED_INFO;
    else if (table == "attributions")
      t->table = gtfs::flat::Translation::TABLE::ATTRIBUTIONS;
    else {
      std::stringstream msg;
      msg << "'" << table << "' is not a valid table";
      throw ParserException(msg.str(), "table_name", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    t->fieldName = getString(*csvp, flds.fieldNameFld, "");
    t->language = getString(*csvp, flds.languageFld, "");
    t->translation = getString(*csvp, flds.translationFld, "");
    t->recordId = getString(*csvp, flds.recordIdFld, "");
    t->recordSubId = getString(*csvp, flds.recordSubIdFld, "");
    t->fieldValue = getString(*csvp, flds.fieldValueFld, "");

    if (t->fieldName.size() == 0) {
      std::stringstream msg;
      msg << "'field_name' is a required field";
      throw ParserException(msg.str(), "field_name", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (t->language.size() == 0) {
      std::stringstream msg;
      msg << "'language' is a required field";
      throw ParserException(msg.str(), "language", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseTranslations(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::Translation a;
  auto flds = getTranslationFlds(csvp);

  while (nextTranslation(csvp, &a, flds)) {
    ad::cppgtfs::gtfs::Translation t(a.table, a.fieldName, a.language,
                                     a.translation, a.recordId, a.recordSubId,
                                     a.fieldValue);
    targetFeed->getTranslations().push_back(t);
  }
}

// ____________________________________________________________________________
inline gtfs::flat::AttributionsFlds Parser::getAttributionsFlds(
    CsvParser* csvp) {
  gtfs::flat::AttributionsFlds t;
  t.attributionIdFld = csvp->getOptFieldIndex("attribution_id");
  t.agencyIdFld = csvp->getOptFieldIndex("agency_id");
  t.routeIdFld = csvp->getOptFieldIndex("route_id");
  t.tripIdFld = csvp->getOptFieldIndex("trip_id");
  t.organizationNameFld = csvp->getFieldIndex("organization_name");
  t.isProducerFld = csvp->getOptFieldIndex("is_producer");
  t.isOperatorFld = csvp->getOptFieldIndex("is_operator");
  t.isAuthorityFld = csvp->getOptFieldIndex("is_authority");
  t.attributionUrlFld = csvp->getOptFieldIndex("attribution_url");
  t.attributionMailFld = csvp->getOptFieldIndex("attribution_email");
  t.attributionPhoneFld = csvp->getOptFieldIndex("attribution_phone");

  // for (size_t i = 0; i < csvp->getNumHeaders(); i++) {
  // if (i == t.attributionIdFld || i == t.agencyIdFld || i == t.routeIdFld ||
  // i == t.tripIdFld || i == t.organizationNameFld ||
  // i == t.isProducerFld || i == t.isOperatorFld || i == t.isAuthorityFld ||
  // i == t.attributionUrlFld || i == t.attributionMailFld ||
  // i == t.attributionPhoneFld) {
  // continue;
  // }
  // t.addHeaders.push_back(i);
  // }

  return t;
}

// ____________________________________________________________________________
inline bool Parser::nextAttribution(
    CsvParser* csvp, gtfs::flat::Attribution* a,
    const gtfs::flat::AttributionsFlds& flds) const {
  if (csvp->readNextLine()) {
    a->attributionId = getString(*csvp, flds.attributionIdFld, "");
    a->agencyId = getString(*csvp, flds.agencyIdFld, "");
    a->routeId = getString(*csvp, flds.routeIdFld, "");
    a->tripId = getString(*csvp, flds.tripIdFld, "");
    a->organizationName = getString(*csvp, flds.organizationNameFld, "");
    a->isProducer = static_cast<gtfs::flat::Attribution::TYPE>(
        getRangeInteger(*csvp, flds.isProducerFld, 0, 1, 0));
    a->isOperator = static_cast<gtfs::flat::Attribution::TYPE>(
        getRangeInteger(*csvp, flds.isOperatorFld, 0, 1, 0));
    a->isAuthority = static_cast<gtfs::flat::Attribution::TYPE>(
        getRangeInteger(*csvp, flds.isAuthorityFld, 0, 1, 0));
    a->attributionUrl = getString(*csvp, flds.attributionUrlFld, "");
    a->attributionEmail = getString(*csvp, flds.attributionMailFld, "");
    a->attributionPhone = getString(*csvp, flds.attributionPhoneFld, "");
    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseAttributions(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::Attribution a;
  auto flds = getAttributionsFlds(csvp);

  while (nextAttribution(csvp, &a, flds)) {
    typename AgencyT::Ref agency = 0;
    RouteT* route = 0;
    TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* trip = 0;

    if (a.agencyId.size()) {
      agency = targetFeed->getAgencies().get(a.agencyId);
      if (!agency) {
        std::stringstream msg;
        msg << "no agency with id '" << a.agencyId
            << "' defined in agency.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "agency_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (a.tripId.size()) {
      trip = targetFeed->getTrips().get(a.tripId);
      if (!trip) {
        std::stringstream msg;
        msg << "no trip with id '" << a.tripId
            << "' defined in trips.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "trip_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (a.routeId.size()) {
      route = targetFeed->getRoutes().get(a.routeId);
      if (!route) {
        std::stringstream msg;
        msg << "no route with id '" << a.routeId
            << "' defined in routes.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "route_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    ad::cppgtfs::gtfs::Attribution<StopT, StopTimeT, ServiceT, RouteT, ShapeT>
        t(a.attributionId, agency, route, trip, a.organizationName,
          a.isProducer, a.isOperator, a.isAuthority, a.attributionUrl,
          a.attributionEmail, a.attributionPhone);
    targetFeed->getAttributions().push_back(t);
  }
}

// ____________________________________________________________________________
inline gtfs::flat::TransfersFlds Parser::getTransfersFlds(CsvParser* csvp) {
  gtfs::flat::TransfersFlds t;
  t.fromStopIdFld = csvp->getOptFieldIndex("from_stop_id");
  t.toStopIdFld = csvp->getOptFieldIndex("to_stop_id");
  t.fromRouteIdFld = csvp->getOptFieldIndex("from_route_id");
  t.toRouteIdFld = csvp->getOptFieldIndex("to_route_id");
  t.fromTripIdFld = csvp->getOptFieldIndex("from_trip_id");
  t.toTripIdFld = csvp->getOptFieldIndex("to_trip_id");
  t.transferTypeFld = csvp->getFieldIndex("transfer_type");
  t.minTransferTimeFld = csvp->getOptFieldIndex("min_transfer_time");
  return t;
}

// ____________________________________________________________________________
inline bool Parser::nextTransfer(CsvParser* csvp, gtfs::flat::Transfer* t,
                                 const gtfs::flat::TransfersFlds& flds) const {
  if (csvp->readNextLine()) {
    t->fromStop = getString(*csvp, flds.fromStopIdFld, "");
    t->toStop = getString(*csvp, flds.toStopIdFld, "");
    t->fromRoute = getString(*csvp, flds.fromRouteIdFld, "");
    t->toRoute = getString(*csvp, flds.toRouteIdFld, "");
    t->fromTrip = getString(*csvp, flds.fromTripIdFld, "");
    t->toTrip = getString(*csvp, flds.toTripIdFld, "");
    t->type = static_cast<gtfs::flat::Transfer::TYPE>(
        getRangeInteger(*csvp, flds.transferTypeFld, 0, 5, 0));
    t->tTime =
        getRangeInteger(*csvp, flds.minTransferTimeFld, 0, UINT32_MAX, -1);
    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseTransfers(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::Transfer ft;
  auto flds = getTransfersFlds(csvp);

  while (nextTransfer(csvp, &ft, flds)) {
    StopT* fromStop = 0;
    StopT* toStop = 0;

    RouteT* fromRoute = 0;
    RouteT* toRoute = 0;

    TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* fromTrip = 0;
    TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* toTrip = 0;

    if (ft.fromStop.size()) {
      fromStop = targetFeed->getStops().get(ft.fromStop);
      if (!fromStop) {
        std::stringstream msg;
        msg << "no stop with id '" << ft.fromStop
            << "' defined in stops.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "from_stop_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (ft.toStop.size()) {
      toStop = targetFeed->getStops().get(ft.toStop);
      if (!toStop) {
        std::stringstream msg;
        msg << "no stop with id '" << ft.toStop
            << "' defined in stops.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "to_stop_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (ft.fromRoute.size()) {
      fromRoute = targetFeed->getRoutes().get(ft.fromRoute);
      if (!fromRoute) {
        std::stringstream msg;
        msg << "no route with id '" << ft.fromRoute
            << "' defined in routes.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "from_route_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (ft.toRoute.size()) {
      toRoute = targetFeed->getRoutes().get(ft.toRoute);
      if (!toRoute) {
        std::stringstream msg;
        msg << "no route with id '" << ft.toRoute
            << "' defined in routes.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "to_route_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (ft.fromTrip.size()) {
      fromTrip = targetFeed->getTrips().get(ft.fromTrip);
      if (!fromTrip) {
        std::stringstream msg;
        msg << "no trip with id '" << ft.fromTrip
            << "' defined in trips.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "from_trip_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (ft.toTrip.size()) {
      toTrip = targetFeed->getTrips().get(ft.toTrip);
      if (!toTrip) {
        std::stringstream msg;
        msg << "no trip with id '" << ft.toTrip
            << "' defined in trips.txt, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "to_trip_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    Transfer<StopT, StopTimeT, ServiceT, RouteT, ShapeT> t(
        fromStop, toStop, fromRoute, toRoute, fromTrip, toTrip, ft.type,
        ft.tTime);
    targetFeed->getTransfers().push_back(t);
  }
}

// ____________________________________________________________________________
inline gtfs::flat::FrequencyFlds Parser::getFrequencyFlds(CsvParser* csvp) {
  gtfs::flat::FrequencyFlds r;
  r.tripIdFld = csvp->getFieldIndex("trip_id");
  r.startTimeFld = csvp->getFieldIndex("start_time");
  r.endTimeFld = csvp->getFieldIndex("end_time");
  r.headwaySecsFld = csvp->getFieldIndex("headway_secs");
  r.exactTimesFld = csvp->getOptFieldIndex("exact_times");
  return r;
}

// ____________________________________________________________________________
inline bool Parser::nextFrequency(CsvParser* csvp, gtfs::flat::Frequency* r,
                                  const gtfs::flat::FrequencyFlds& flds) const {
  if (csvp->readNextLine()) {
    r->tripId = getString(*csvp, flds.tripIdFld);
    r->startTime = getTime(*csvp, flds.startTimeFld);
    r->endTime = getTime(*csvp, flds.endTimeFld);
    r->headwaySecs = getRangeInteger(*csvp, flds.headwaySecsFld, 0, UINT32_MAX);
    r->exactTimes = getRangeInteger(*csvp, flds.exactTimesFld, 0, 1, 0);
    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFrequencies(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::Frequency ff;
  auto flds = getFrequencyFlds(csvp);

  while (nextFrequency(csvp, &ff, flds)) {
    gtfs::Frequency f(ff.startTime, ff.endTime, ff.headwaySecs, ff.exactTimes);

    auto trip = targetFeed->getTrips().get(ff.tripId);
    if (!trip) {
      std::stringstream msg;
      msg << "trip '" << ff.tripId << "' not found.";
      throw ParserException(msg.str(), "trip_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    trip->addFrequency(f);
  }
}

// ____________________________________________________________________________
inline gtfs::flat::FareFlds Parser::getFareFlds(CsvParser* csvp) {
  gtfs::flat::FareFlds f;
  f.fareIdFld = csvp->getFieldIndex("fare_id");
  f.priceFld = csvp->getFieldIndex("price");
  f.currencyTypeFld = csvp->getFieldIndex("currency_type");
  f.paymentMethodFld = csvp->getFieldIndex("payment_method");
  f.transfersFld = csvp->getFieldIndex("transfers");
  f.agencyFld = csvp->getOptFieldIndex("agency");
  f.transferDurationFld = csvp->getOptFieldIndex("transfer_duration");
  return f;
}

// ____________________________________________________________________________
inline bool Parser::nextFare(CsvParser* csvp, gtfs::flat::Fare* t,
                             const gtfs::flat::FareFlds& flds) const {
  if (csvp->readNextLine()) {
    t->id = getString(*csvp, flds.fareIdFld);
    t->price = getDouble(*csvp, flds.priceFld);
    t->currencyType = getString(*csvp, flds.currencyTypeFld);
    t->paymentMethod = static_cast<typename gtfs::flat::Fare::PAYMENT_METHOD>(
        getRangeInteger(*csvp, flds.paymentMethodFld, 0, 1));
    t->numTransfers = static_cast<typename gtfs::flat::Fare::NUM_TRANSFERS>(
        getRangeInteger(*csvp, flds.transfersFld, 0, 3, 3));
    t->agency = getString(*csvp, flds.agencyFld, "");
    t->duration =
        getRangeInteger(*csvp, flds.transferDurationFld, 0, INT64_MAX, -1);
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFareAttributes(gtfs::FEEDB* targetFeed,
                                 CsvParser* csvp) const {
  gtfs::flat::Fare ff;
  auto flds = getFareFlds(csvp);

  while (nextFare(csvp, &ff, flds)) {
    typename AgencyT::Ref agency = typename AgencyT::Ref();

    if (!ff.agency.empty()) {
      agency = targetFeed->getAgencies().get(ff.agency);
      if (!agency) {
        std::stringstream msg;
        msg << "no agency with id '" << ff.agency << "' defined, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "agency_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    targetFeed->getFares().add(FareT<RouteT>(ff.id, ff.price, ff.currencyType,
                                             ff.paymentMethod, ff.numTransfers,
                                             agency, ff.duration));
  }
}

// ____________________________________________________________________________
inline gtfs::flat::FareRuleFlds Parser::getFareRuleFlds(CsvParser* csvp) {
  gtfs::flat::FareRuleFlds f;
  f.fareIdFld = csvp->getFieldIndex("fare_id");
  f.routeIdFld = csvp->getOptFieldIndex("route_id");
  f.originIdFld = csvp->getOptFieldIndex("origin_id");
  f.destinationIdFld = csvp->getOptFieldIndex("destination_id");
  f.containsIdFld = csvp->getOptFieldIndex("contains_id");
  return f;
}

// ____________________________________________________________________________
inline bool Parser::nextFareRule(CsvParser* csvp, gtfs::flat::FareRule* t,
                                 const gtfs::flat::FareRuleFlds& flds) const {
  if (csvp->readNextLine()) {
    t->fare = getString(*csvp, flds.fareIdFld);
    t->route = getString(*csvp, flds.routeIdFld, "");
    t->originZone = getString(*csvp, flds.originIdFld, "");
    t->destZone = getString(*csvp, flds.destinationIdFld, "");
    t->containsZone = getString(*csvp, flds.containsIdFld, "");
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFareRules(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::FareRule fr;
  auto flds = getFareRuleFlds(csvp);

  while (nextFareRule(csvp, &fr, flds)) {
    Fare<RouteT>* fare = targetFeed->getFares().get(fr.fare);
    RouteT* route = targetFeed->getRoutes().get(fr.route);

    if (!fare) {
      std::stringstream msg;
      msg << "no fare with id '" << fr.fare << "' defined, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "fare_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (!fr.route.empty() && !route) {
      std::stringstream msg;
      msg << "no route with id '" << fr.route << "' defined, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "route_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (!fr.originZone.empty() &&
        !targetFeed->getZones().count(fr.originZone)) {
      std::stringstream msg;
      msg << "no zone with id '" << fr.originZone
          << "' defined in stops.txt, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "origin_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (!fr.destZone.empty() && !targetFeed->getZones().count(fr.destZone)) {
      std::stringstream msg;
      msg << "no zone with id '" << fr.destZone
          << "' defined in stops.txt, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "destination_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (!fr.containsZone.empty() &&
        !targetFeed->getZones().count(fr.containsZone)) {
      std::stringstream msg;
      msg << "no zone with id '" << fr.containsZone
          << "' defined in stops.txt, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "contains_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    FareRule<RouteT> r(route, fr.originZone, fr.destZone, fr.containsZone);
    fare->addFareRule(r);
  }

  targetFeed->getFares().finalize();
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFeedInfo(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  size_t feedPublisherNameFld = csvp->getFieldIndex("feed_publisher_name");
  size_t feedPublisherUrlFld = csvp->getFieldIndex("feed_publisher_url");
  size_t feedLangFld = csvp->getOptFieldIndex("feed_lang");
  size_t feedStartDateFld = csvp->getOptFieldIndex("feed_start_date");
  size_t feedEndDateFld = csvp->getOptFieldIndex("feed_end_date");
  size_t feedVersionFld = csvp->getOptFieldIndex("feed_version");
  size_t feedContactEmailFld = csvp->getOptFieldIndex("feed_contact_email");
  size_t feedContactUrlFld = csvp->getOptFieldIndex("feed_contact_url");
  size_t defaultLangFld = csvp->getOptFieldIndex("default_lang");

  while (csvp->readNextLine()) {
    targetFeed->setPublisherName(getString(*csvp, feedPublisherNameFld));
    targetFeed->setPublisherUrl(getString(*csvp, feedPublisherUrlFld));
    targetFeed->setLang(getString(*csvp, feedLangFld, ""));
    targetFeed->setVersion(getString(*csvp, feedVersionFld, ""));
    targetFeed->setStartDate(getServiceDate(*csvp, feedStartDateFld, false));
    targetFeed->setEndDate(getServiceDate(*csvp, feedEndDateFld, false));
    targetFeed->setContactEmail(getString(*csvp, feedContactEmailFld, ""));
    targetFeed->setContactUrl(getString(*csvp, feedContactUrlFld, ""));
    targetFeed->setDefaultLang(getString(*csvp, defaultLangFld, ""));
  }
}

// ____________________________________________________________________________
inline gtfs::flat::AgencyFlds Parser::getAgencyFlds(CsvParser* csvp) {
  gtfs::flat::AgencyFlds r;
  r.agencyNameFld = csvp->getFieldIndex("agency_name");
  r.agencyUrlFld = csvp->getFieldIndex("agency_url");
  r.agencyTimezoneFld = csvp->getFieldIndex("agency_timezone");
  r.agencyEmailFld = csvp->getOptFieldIndex("agency_email");
  r.agencyFareUrlFld = csvp->getOptFieldIndex("agency_fare_url");
  r.agencyLangFld = csvp->getOptFieldIndex("agency_lang");
  r.agencyPhoneFld = csvp->getOptFieldIndex("agency_phone");
  r.agencyIdFld = csvp->getOptFieldIndex("agency_id");

  for (size_t i = 0; i < csvp->getNumHeaders(); i++) {
    if (i == r.agencyNameFld || i == r.agencyUrlFld ||
        i == r.agencyTimezoneFld || i == r.agencyEmailFld ||
        i == r.agencyFareUrlFld || i == r.agencyLangFld ||
        i == r.agencyPhoneFld || i == r.agencyIdFld) {
      continue;
    }
    r.addHeaders.push_back(i);
  }

  return r;
}

// ____________________________________________________________________________
inline bool Parser::nextAgency(CsvParser* csvp, gtfs::flat::Agency* a,
                               const gtfs::flat::AgencyFlds& flds) const {
  if (csvp->readNextLine()) {
    a->id = getString(*csvp, flds.agencyIdFld, "");
    a->name = getString(*csvp, flds.agencyNameFld);
    a->url = getString(*csvp, flds.agencyUrlFld);
    a->timezone = getString(*csvp, flds.agencyTimezoneFld);
    a->lang = getString(*csvp, flds.agencyLangFld, "");
    a->phone = getString(*csvp, flds.agencyPhoneFld, "");
    a->fare_url = getString(*csvp, flds.agencyFareUrlFld, "");
    a->agency_email = getString(*csvp, flds.agencyEmailFld, "");

    return true;
  }

  return false;
}

// ____________________________________________________________________________
inline bool Parser::nextLevel(CsvParser* csvp, gtfs::flat::Level* a,
                              const gtfs::flat::LevelFlds& flds) const {
  if (csvp->readNextLine()) {
    a->id = getString(*csvp, flds.levelIdFld);
    a->name = getString(*csvp, flds.levelNameFld, "");
    a->index = getDouble(*csvp, flds.levelIndexFld);

    return true;
  }

  return false;
}

// ____________________________________________________________________________
inline gtfs::flat::LevelFlds Parser::getLevelFlds(CsvParser* csvp) {
  gtfs::flat::LevelFlds r;
  r.levelIdFld = csvp->getFieldIndex("level_id");
  r.levelIndexFld = csvp->getFieldIndex("level_index");
  r.levelNameFld = csvp->getOptFieldIndex("level_name");
  return r;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseLevels(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  typename LevelT::Ref a = (typename LevelT::Ref());
  gtfs::flat::Level fa;
  auto flds = getLevelFlds(csvp);

  while (nextLevel(csvp, &fa, flds)) {
    if ((typename LevelT::Ref()) ==
        (a = targetFeed->getLevels().add(
             gtfs::Level(fa.id, fa.index, fa.name)))) {
      std::stringstream msg;
      msg << "'level_id' must be dataset unique. Collision with id '" << fa.id
          << "')";
      throw ParserException(msg.str(), "level_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }
  }

  targetFeed->getLevels().finalize();
}

// ____________________________________________________________________________
inline bool Parser::nextPathway(CsvParser* csvp, gtfs::flat::Pathway* a,
                                const gtfs::flat::PathwayFlds& flds) const {
  if (csvp->readNextLine()) {
    a->id = getString(*csvp, flds.pathwayIdFld);
    a->from_stop_id = getString(*csvp, flds.fromStopIdFld);
    a->to_stop_id = getString(*csvp, flds.toStopIdFld);
    a->pathway_mode = getRangeInteger(*csvp, flds.pathwayModeFld, 1, 7);
    a->is_bidirectional = getRangeInteger(*csvp, flds.isBidirectionalFld, 0, 1);
    a->length = getDouble(*csvp, flds.lengthFld, -1);
    a->traversal_time =
        getRangeInteger(*csvp, flds.traverselTimeFld, 0, INT64_MAX, -1);
    a->stair_count =
        getRangeInteger(*csvp, flds.stairCountFld, -INT64_MAX, INT64_MAX, 0);
    a->max_slope = getDouble(*csvp, flds.maxSlopeFld, 0);
    a->min_width = getDouble(*csvp, flds.minWidthFld, -1);
    a->signposted_as = getString(*csvp, flds.signPostedAsFld, "");
    a->reversed_signposted_as =
        getString(*csvp, flds.reversedSignPostedAsFld, "");

    return true;
  }

  return false;
}

// ____________________________________________________________________________
inline gtfs::flat::PathwayFlds Parser::getPathwayFlds(CsvParser* csvp) {
  gtfs::flat::PathwayFlds r;
  r.pathwayIdFld = csvp->getFieldIndex("pathway_id");
  r.fromStopIdFld = csvp->getFieldIndex("from_stop_id");
  r.toStopIdFld = csvp->getFieldIndex("to_stop_id");
  r.pathwayModeFld = csvp->getFieldIndex("pathway_mode");
  r.isBidirectionalFld = csvp->getFieldIndex("is_bidirectional");
  r.lengthFld = csvp->getOptFieldIndex("length");
  r.traverselTimeFld = csvp->getOptFieldIndex("traversal_time");
  r.stairCountFld = csvp->getOptFieldIndex("stair_count");
  r.maxSlopeFld = csvp->getOptFieldIndex("max_slope");
  r.minWidthFld = csvp->getOptFieldIndex("min_width");
  r.signPostedAsFld = csvp->getOptFieldIndex("signposted_as");
  r.reversedSignPostedAsFld = csvp->getOptFieldIndex("reversed_signposted_as");
  return r;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parsePathways(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  typename PathwayT::Ref a = (typename PathwayT::Ref());
  gtfs::flat::Pathway fa;
  auto flds = getPathwayFlds(csvp);

  while (nextPathway(csvp, &fa, flds)) {
    StopT* fromStop = 0;
    StopT* toStop = 0;

    fromStop = targetFeed->getStops().get(fa.from_stop_id);
    toStop = targetFeed->getStops().get(fa.to_stop_id);

    if (!fromStop) {
      std::stringstream msg;
      msg << "no stop with id '" << fa.from_stop_id
          << "' defined in stops.txt, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "from_stop_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (!toStop) {
      std::stringstream msg;
      msg << "no stop with id '" << fa.to_stop_id
          << "' defined in stops.txt, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "to_stop_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if ((typename PathwayT::Ref()) ==
        (a = targetFeed->getPathways().add(gtfs::Pathway(
             fa.id, fromStop, toStop, fa.pathway_mode, fa.is_bidirectional,
             fa.length, fa.traversal_time, fa.stair_count, fa.max_slope,
             fa.min_width, fa.signposted_as, fa.reversed_signposted_as)))) {
      std::stringstream msg;
      msg << "'pathway_id' must be dataset unique. Collision with id '" << fa.id
          << "')";
      throw ParserException(msg.str(), "pathway_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }
  }

  targetFeed->getPathways().finalize();
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseAgencies(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  typename AgencyT::Ref a = (typename AgencyT::Ref());
  gtfs::flat::Agency fa;
  auto flds = getAgencyFlds(csvp);

  while (nextAgency(csvp, &fa, flds)) {
    if ((typename AgencyT::Ref()) ==
        (a = targetFeed->getAgencies().add(
             gtfs::Agency(fa.id, fa.name, fa.url, fa.timezone, fa.lang,
                          fa.phone, fa.fare_url, fa.agency_email)))) {
      std::stringstream msg;
      msg << "'agency_id' must be dataset unique. Collision with id '" << fa.id
          << "')";
      throw ParserException(msg.str(), "agency_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (_parseAdditionalFields) {
      // additional fields
      for (size_t fid : flds.addHeaders) {
        const auto& val = getString(*csvp, fid, "");
        if (val.size())
          targetFeed->addAgencyAddFld(fa.id, csvp->getFieldName(fid), val);
      }
    }
  }

  if ((typename AgencyT::Ref()) == a) {
    throw ParserException(
        "the feed has no agency defined."
        " This is a required field.",
        "", 1, csvp->getReadablePath());
  }

  targetFeed->getAgencies().finalize();
}

// ____________________________________________________________________________
inline gtfs::flat::StopFlds Parser::getStopFlds(CsvParser* csvp) {
  gtfs::flat::StopFlds r;
  r.stopIdFld = csvp->getFieldIndex("stop_id");
  r.stopNameFld = csvp->getFieldIndex("stop_name");
  r.stopLatFld = csvp->getFieldIndex("stop_lat");
  r.stopLonFld = csvp->getFieldIndex("stop_lon");
  r.parentStationFld = csvp->getOptFieldIndex("parent_station");
  r.stopCodeFld = csvp->getOptFieldIndex("stop_code");
  r.stopDescFld = csvp->getOptFieldIndex("stop_desc");
  r.zoneIdFld = csvp->getOptFieldIndex("zone_id");
  r.stopUrlFld = csvp->getOptFieldIndex("stop_url");
  r.stopTimezoneFld = csvp->getOptFieldIndex("stop_timezone");
  r.wheelchairBoardingFld = csvp->getOptFieldIndex("wheelchair_boarding");
  r.locationTypeFld = csvp->getOptFieldIndex("location_type");
  r.platformCodeFld = csvp->getOptFieldIndex("platform_code");
  r.levelIdFld = csvp->getOptFieldIndex("level_id");

  for (size_t i = 0; i < csvp->getNumHeaders(); i++) {
    if (i == r.stopIdFld || i == r.stopNameFld || i == r.stopLatFld ||
        i == r.stopLonFld || i == r.parentStationFld || i == r.stopCodeFld ||
        i == r.stopDescFld || i == r.zoneIdFld || i == r.stopUrlFld ||
        i == r.stopTimezoneFld || i == r.wheelchairBoardingFld ||
        i == r.locationTypeFld || i == r.platformCodeFld || i == r.levelIdFld) {
      continue;
    }
    r.addHeaders.push_back(i);
  }

  return r;
}

// ____________________________________________________________________________
inline bool Parser::nextStop(CsvParser* csvp, gtfs::flat::Stop* s,
                             const gtfs::flat::StopFlds& flds) const {
  if (csvp->readNextLine()) {
    s->location_type = static_cast<gtfs::flat::Stop::LOCATION_TYPE>(
        getRangeInteger(*csvp, flds.locationTypeFld, 0, 4, 0));
    s->id = getString(*csvp, flds.stopIdFld);
    s->code = getString(*csvp, flds.stopCodeFld, "");
    if (s->location_type < 3) {
      s->name = getString(*csvp, flds.stopNameFld);
    } else {
      s->name = getString(*csvp, flds.stopNameFld, "");
    }
    s->desc = getString(*csvp, flds.stopDescFld, "");
    s->zone_id = getString(*csvp, flds.zoneIdFld, "");
    s->stop_url = getString(*csvp, flds.stopUrlFld, "");
    s->stop_timezone = getString(*csvp, flds.stopTimezoneFld, "");
    s->platform_code = getString(*csvp, flds.platformCodeFld, "");
    s->parent_station = getString(*csvp, flds.parentStationFld, "");
    if (s->location_type < 3) {
      s->lat = getDouble(*csvp, flds.stopLatFld);
      s->lng = getDouble(*csvp, flds.stopLonFld);

      if (s->lat > 90 || s->lat < -90)
        throw ParserException("latitude must be between -90.0 and 90.0",
                              "stop_lat", csvp->getCurLine(),
                              csvp->getReadablePath());

      if (s->lng > 180 || s->lng < -180)
        throw ParserException("longitude must be between -180.0 and 180.0",
                              "stop_lon", csvp->getCurLine(),
                              csvp->getReadablePath());
    } else {
      s->lat = getDouble(*csvp, flds.stopLatFld, 99999);
      s->lng = getDouble(*csvp, flds.stopLonFld, 99999);
    }
    s->wheelchair_boarding = static_cast<gtfs::flat::Stop::WHEELCHAIR_BOARDING>(
        getRangeInteger(*csvp, flds.wheelchairBoardingFld, 0, 2, 0));
    s->level_id = getString(*csvp, flds.levelIdFld, "");

    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseStops(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  std::map<std::string, std::pair<size_t, std::string> > parentStations;

  gtfs::flat::Stop fs;
  auto flds = getStopFlds(csvp);

  while (nextStop(csvp, &fs, flds)) {
    targetFeed->updateBox(fs.lat, fs.lng);
    Level* level = 0;

    if (!fs.level_id.empty()) {
      level = targetFeed->getLevels().get(fs.level_id);
      if (!level) {
        std::stringstream msg;
        msg << "no stop with id '" << fs.level_id << "' defined, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "level_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    const StopT& s =
        StopT(fs.id, fs.code, fs.name, fs.desc, fs.lat, fs.lng, fs.zone_id,
              fs.stop_url, fs.location_type, 0, fs.stop_timezone,
              fs.wheelchair_boarding, fs.platform_code, level);

    if (!fs.parent_station.empty()) {
      if (fs.location_type == gtfs::flat::Stop::LOCATION_TYPE::STATION) {
        throw ParserException(
            "a stop with location_type 'station' (1) cannot"
            " have a parent station",
            "parent_station", csvp->getCurLine(), csvp->getReadablePath());
      }

      parentStations[s.getId()] =
          std::pair<size_t, std::string>(csvp->getCurLine(), fs.parent_station);
    }

    targetFeed->getZones().insert(fs.zone_id);

    if (!targetFeed->getStops().add(s)) {
      std::stringstream msg;
      msg << "'stop_id' must be dataset unique. Collision with id '"
          << s.getId() << "')";
      throw ParserException(msg.str(), "stop_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (_parseAdditionalFields) {
      // additional fields
      for (size_t fid : flds.addHeaders) {
        const auto& val = getString(*csvp, fid, "");
        if (val.size())
          targetFeed->addStopAddFld(fs.id, csvp->getFieldName(fid), val);
      }
    }
  }

  targetFeed->getStops().finalize();

  // second pass to resolve parentStation pointers
  for (const auto& ps : parentStations) {
    StopT* parentStation = 0;
    parentStation = targetFeed->getStops().get(ps.second.second);
    if (!parentStation) {
      std::stringstream msg;
      msg << "no stop with id '" << ps.second.second << "' defined, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "parent_station", ps.second.first,
                            csvp->getReadablePath());
    } else {
      targetFeed->getStops().get(ps.first)->setParentStation(parentStation);
    }
  }
}

// ____________________________________________________________________________
inline gtfs::flat::RouteFlds Parser::getRouteFlds(CsvParser* csvp) {
  gtfs::flat::RouteFlds r;
  r.routeIdFld = csvp->getFieldIndex("route_id");
  r.routeLongNameFld = csvp->getOptFieldIndex("route_long_name");
  r.routeShortNameFld = csvp->getOptFieldIndex("route_short_name");
  r.routeTypeFld = csvp->getFieldIndex("route_type");
  r.routeUrlFld = csvp->getOptFieldIndex("route_url");
  r.routeDescFld = csvp->getOptFieldIndex("route_desc");
  r.agencyIdFld = csvp->getOptFieldIndex("agency_id");
  r.routeColorFld = csvp->getOptFieldIndex("route_color");
  r.routeTextColorFld = csvp->getOptFieldIndex("route_text_color");
  r.routeSortOrderFld = csvp->getOptFieldIndex("route_sort_order");
  r.continuousDropOffFld = csvp->getOptFieldIndex("continuous_drop_off");
  r.continuousPickupFld = csvp->getOptFieldIndex("continuous_pickup");

  for (size_t i = 0; i < csvp->getNumHeaders(); i++) {
    if (i == r.routeIdFld || i == r.routeLongNameFld ||
        i == r.routeShortNameFld || i == r.routeTypeFld || i == r.routeUrlFld ||
        i == r.routeDescFld || i == r.agencyIdFld || i == r.routeColorFld ||
        i == r.routeTextColorFld || i == r.routeSortOrderFld ||
        i == r.continuousDropOffFld || i == r.continuousPickupFld) {
      continue;
    }
    r.addHeaders.push_back(i);
  }
  return r;
}

// ____________________________________________________________________________
inline bool Parser::nextRoute(CsvParser* csvp, gtfs::flat::Route* r,
                              const gtfs::flat::RouteFlds& flds) const {
  if (csvp->readNextLine()) {
    r->id = getString(*csvp, flds.routeIdFld);
    r->agency = getString(*csvp, flds.agencyIdFld, "");
    r->short_name = getString(*csvp, flds.routeShortNameFld, "");
    r->long_name = getString(*csvp, flds.routeLongNameFld, "");
    r->desc = getString(*csvp, flds.routeDescFld, "");
    r->type = getRouteType(*csvp, flds.routeTypeFld,
                           getRangeInteger(*csvp, flds.routeTypeFld, 0, 1702));
    r->url = getString(*csvp, flds.routeUrlFld, "");
    r->color = getColorFromHexString(*csvp, flds.routeColorFld, "");
    r->text_color = getColorFromHexString(*csvp, flds.routeTextColorFld, "");
    r->sort_order = getRangeInteger(*csvp, flds.routeSortOrderFld, 0,
                                    std::numeric_limits<int64_t>::max(),
                                    std::numeric_limits<int64_t>::max());
    r->continuous_drop_off =
        getRangeInteger(*csvp, flds.continuousDropOffFld, 0, 3, 1);
    r->continuous_pickup =
        getRangeInteger(*csvp, flds.continuousPickupFld, 0, 3, 1);
    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseRoutes(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::Route fr;
  auto flds = getRouteFlds(csvp);

  while (nextRoute(csvp, &fr, flds)) {
    typename AgencyT::Ref routeAgency = 0;

    if (!fr.agency.empty()) {
      routeAgency = targetFeed->getAgencies().get(fr.agency);
      if ((typename AgencyT::Ref()) == routeAgency) {
        std::stringstream msg;
        msg << "no agency with id '" << fr.agency << "' defined, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "agency_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    if (!targetFeed->getRoutes().add(
            RouteT(fr.id, routeAgency, fr.short_name, fr.long_name, fr.desc,
                   fr.type, fr.url, fr.color, fr.text_color, fr.sort_order,
                   fr.continuous_pickup, fr.continuous_drop_off))) {
      std::stringstream msg;
      msg << "'route_id' must be dataset unique. Collision with id '" << fr.id
          << "')";
      throw ParserException(msg.str(), "route_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (_parseAdditionalFields) {
      // additional fields
      for (size_t fid : flds.addHeaders) {
        const auto& val = getString(*csvp, fid, "");
        if (val.size())
          targetFeed->addRouteAddFld(fr.id, csvp->getFieldName(fid), val);
      }
    }
  }

  targetFeed->getRoutes().finalize();
}

// ____________________________________________________________________________
inline gtfs::flat::CalendarFlds Parser::getCalendarFlds(CsvParser* csvp) {
  gtfs::flat::CalendarFlds c;
  c.serviceIdFld = csvp->getFieldIndex("service_id");
  c.mondayFld = csvp->getFieldIndex("monday");
  c.tuesdayFld = csvp->getFieldIndex("tuesday");
  c.wednesdayFld = csvp->getFieldIndex("wednesday");
  c.thursdayFld = csvp->getFieldIndex("thursday");
  c.fridayFld = csvp->getFieldIndex("friday");
  c.saturdayFld = csvp->getFieldIndex("saturday");
  c.sundayFld = csvp->getFieldIndex("sunday");
  c.startDateFld = csvp->getFieldIndex("start_date");
  c.endDateFld = csvp->getFieldIndex("end_date");
  return c;
}

// ____________________________________________________________________________
inline bool Parser::nextCalendar(CsvParser* csvp, gtfs::flat::Calendar* c,
                                 const gtfs::flat::CalendarFlds& flds) const {
  if (csvp->readNextLine()) {
    c->id = getString(*csvp, flds.serviceIdFld);
    c->serviceDays = (getRangeInteger(*csvp, flds.mondayFld, 0, 1)) |
                     (getRangeInteger(*csvp, flds.tuesdayFld, 0, 1) << 1) |
                     (getRangeInteger(*csvp, flds.wednesdayFld, 0, 1) << 2) |
                     (getRangeInteger(*csvp, flds.thursdayFld, 0, 1) << 3) |
                     (getRangeInteger(*csvp, flds.fridayFld, 0, 1) << 4) |
                     (getRangeInteger(*csvp, flds.saturdayFld, 0, 1) << 5) |
                     (getRangeInteger(*csvp, flds.sundayFld, 0, 1) << 6);
    c->begin = getServiceDate(*csvp, flds.startDateFld, true);
    c->end = getServiceDate(*csvp, flds.endDateFld, true);

    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseCalendar(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::Calendar fc;
  auto flds = getCalendarFlds(csvp);

  while (nextCalendar(csvp, &fc, flds)) {
    if ((typename ServiceT::Ref()) ==
        targetFeed->getServices().add(
            ServiceT(fc.id, fc.serviceDays, fc.begin, fc.end))) {
      std::stringstream msg;
      msg << "'service_id' must be unique in calendars.txt. Collision with id '"
          << fc.id << "')";
      throw ParserException(msg.str(), "service_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }
  }
}

// ____________________________________________________________________________
inline gtfs::flat::CalendarDateFlds Parser::getCalendarDateFlds(
    CsvParser* csvp) {
  gtfs::flat::CalendarDateFlds c;
  c.serviceIdFld = csvp->getFieldIndex("service_id");
  c.exceptionTypeFld = csvp->getFieldIndex("exception_type");
  c.dateFld = csvp->getFieldIndex("date");
  return c;
}

// ____________________________________________________________________________
inline bool Parser::nextCalendarDate(
    CsvParser* csvp, gtfs::flat::CalendarDate* c,
    const gtfs::flat::CalendarDateFlds& flds) const {
  if (csvp->readNextLine()) {
    c->id = getString(*csvp, flds.serviceIdFld);
    c->date = getServiceDate(*csvp, flds.dateFld, true);
    c->type = static_cast<gtfs::flat::CalendarDate::EXCEPTION_TYPE>(
        getRangeInteger(*csvp, flds.exceptionTypeFld, 1, 2));

    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseCalendarDates(gtfs::FEEDB* targetFeed,
                                CsvParser* csvp) const {
  gtfs::flat::CalendarDate fc;
  auto flds = getCalendarDateFlds(csvp);

  while (nextCalendarDate(csvp, &fc, flds)) {
    ServiceT* e = targetFeed->getServices().get(fc.id);

    if (!e) {
      targetFeed->getServices().add(ServiceT(fc.id));
      e = targetFeed->getServices().get(fc.id);
    }

    if (e) e->addException(fc.date, fc.type);
  }

  targetFeed->getServices().finalize();
}

// ____________________________________________________________________________
inline gtfs::flat::TripFlds Parser::getTripFlds(CsvParser* csvp) {
  gtfs::flat::TripFlds t;
  t.shapeIdFld = csvp->getOptFieldIndex("shape_id");
  t.tripIdFld = csvp->getFieldIndex("trip_id");
  t.serviceIdFld = csvp->getFieldIndex("service_id");
  t.routeIdFld = csvp->getFieldIndex("route_id");
  t.blockIdFld = csvp->getOptFieldIndex("block_id");
  t.tripHeadsignFld = csvp->getOptFieldIndex("trip_headsign");
  t.tripShortNameFld = csvp->getOptFieldIndex("trip_short_name");
  t.bikesAllowedFld = csvp->getOptFieldIndex("bikes_allowed");
  t.wheelchairAccessibleFld = csvp->getOptFieldIndex("wheelchair_accessible");
  t.directionIdFld = csvp->getOptFieldIndex("direction_id");

  for (size_t i = 0; i < csvp->getNumHeaders(); i++) {
    if (i == t.shapeIdFld || i == t.tripIdFld || i == t.serviceIdFld ||
        i == t.routeIdFld || i == t.blockIdFld || i == t.tripHeadsignFld ||
        i == t.tripShortNameFld || i == t.bikesAllowedFld ||
        i == t.wheelchairAccessibleFld || i == t.directionIdFld) {
      continue;
    }
    t.addHeaders.push_back(i);
  }

  return t;
}

// ____________________________________________________________________________
inline bool Parser::nextTrip(CsvParser* csvp, gtfs::flat::Trip* c,
                             const gtfs::flat::TripFlds& flds) const {
  if (csvp->readNextLine()) {
    c->id = getString(*csvp, flds.tripIdFld);
    c->route = getString(*csvp, flds.routeIdFld);
    c->service = getString(*csvp, flds.serviceIdFld);
    c->headsign = getString(*csvp, flds.tripHeadsignFld, "");
    c->short_name = getString(*csvp, flds.tripShortNameFld, "");
    c->dir = static_cast<gtfs::flat::Trip::DIRECTION>(
        getRangeInteger(*csvp, flds.directionIdFld, 0, 1, 2));
    c->block_id = getString(*csvp, flds.blockIdFld, "");
    c->shape = getString(*csvp, flds.shapeIdFld, "");
    c->wc = static_cast<gtfs::flat::Trip::WC_BIKE_ACCESSIBLE>(
        getRangeInteger(*csvp, flds.wheelchairAccessibleFld, 0, 2, 0)),
    c->ba = static_cast<gtfs::flat::Trip::WC_BIKE_ACCESSIBLE>(
        getRangeInteger(*csvp, flds.bikesAllowedFld, 0, 2, 0));
    return true;
  }
  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseTrips(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::Trip ft;
  auto flds = getTripFlds(csvp);

  while (nextTrip(csvp, &ft, flds)) {
    RouteT* tripRoute = 0;

    tripRoute = targetFeed->getRoutes().get(ft.route);
    if (!tripRoute) {
      std::stringstream msg;
      msg << "no route with id '" << ft.route << "' defined, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "route_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    typename ShapeT::Ref tripShape = (typename ShapeT::Ref());

    if (!ft.shape.empty()) {
      tripShape = targetFeed->getShapes().getRef(ft.shape);
      if (tripShape == (typename ShapeT::Ref())) {
        std::stringstream msg;
        msg << "no shape with id '" << ft.shape << "' defined, cannot "
            << "reference here.";
        throw ParserException(msg.str(), "shape_id", csvp->getCurLine(),
                              csvp->getReadablePath());
      }
    }

    typename ServiceT::Ref tripService =
        targetFeed->getServices().getRef(ft.service);

    if ((typename ServiceT::Ref()) == tripService) {
      std::stringstream msg;
      msg << "no service with id '" << ft.service << "' defined, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "service_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (typename TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>::Ref() ==
        targetFeed->getTrips().add(
            TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>(
                ft.id, tripRoute, tripService, ft.headsign, ft.short_name,
                ft.dir, ft.block_id, tripShape, ft.wc, ft.ba))) {
      std::stringstream msg;
      msg << "'trip_id' must be dataset unique. Collision with id '"
          << getString(*csvp, flds.tripIdFld) << "')";
      throw ParserException(msg.str(), "trip_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (_parseAdditionalFields) {
      // additional fields
      for (size_t fid : flds.addHeaders) {
        const auto& val = getString(*csvp, fid, "");
        if (val.size())
          targetFeed->addTripAddFld(ft.id, csvp->getFieldName(fid), val);
      }
    }
  }

  targetFeed->getTrips().finalize();
}

// ____________________________________________________________________________
inline gtfs::flat::ShapeFlds Parser::getShapeFlds(CsvParser* csvp) {
  gtfs::flat::ShapeFlds s;
  s.shapeIdFld = csvp->getFieldIndex("shape_id");
  s.shapePtSequenceFld = csvp->getFieldIndex("shape_pt_sequence");
  s.shapePtLonFld = csvp->getFieldIndex("shape_pt_lon");
  s.shapePtLatFld = csvp->getFieldIndex("shape_pt_lat");
  s.shapeDistTraveledFld = csvp->getOptFieldIndex("shape_dist_traveled");
  return s;
}

// ____________________________________________________________________________
inline bool Parser::nextShapePoint(CsvParser* csvp, gtfs::flat::ShapePoint* c,
                                   const gtfs::flat::ShapeFlds& flds) const {
  if (csvp->readNextLine()) {
    c->id = getString(*csvp, flds.shapeIdFld);
    c->lat = getDouble(*csvp, flds.shapePtLatFld);
    c->lng = getDouble(*csvp, flds.shapePtLonFld);
    c->seq = getRangeInteger(*csvp, flds.shapePtSequenceFld, 0, UINT32_MAX);
    c->travelDist = -1;  // using -1 as a null value here

    if (flds.shapeDistTraveledFld < csvp->getNumColumns()) {
      if (!getString(*csvp, flds.shapeDistTraveledFld, "").empty()) {
        c->travelDist = getDouble(*csvp, flds.shapeDistTraveledFld);
        if (c->travelDist < -0.01) {  // TODO(patrick): better double comp
          throw ParserException(
              "negative values not supported for distances"
              " (value was: " +
                  std::to_string(c->travelDist),
              "shape_dist_traveled", csvp->getCurLine(),
              csvp->getReadablePath());
        }
      }
    }

    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseStops(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/stops.txt";
  try {
    auto csvp = getCsvParser("stops.txt");
    if (!csvp->isGood()) fileNotFound(curFile);
    parseStops(targetFeed, csvp.get());
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseRoutes(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/routes.txt";
  try {
    auto csvp = getCsvParser("routes.txt");
    if (!csvp->isGood()) fileNotFound(curFile);
    parseRoutes(targetFeed, csvp.get());
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseCalendar(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/calendar.txt";
  try {
    auto csvp = getCsvParser("calendar.txt");
    if (csvp->isGood()) {
      parseCalendar(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseCalendarDates(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/calendar_dates.txt";
  try {
    auto csvp = getCsvParser("calendar_dates.txt");
    if (csvp->isGood()) {
      parseCalendarDates(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFeedInfo(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/feed_info.txt";

  try {
    auto csvp = getCsvParser("feed_info.txt");
    if (csvp->isGood()) {
      parseFeedInfo(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parsePathways(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/pathways.txt";
  try {
    auto csvp = getCsvParser("pathways.txt");
    if (csvp->isGood()) {
      parsePathways(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseLevels(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/levels.txt";
  try {
    auto csvp = getCsvParser("levels.txt");
    if (csvp->isGood()) {
      parseLevels(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseAgencies(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/agency.txt";
  try {
    auto csvp = getCsvParser("agency.txt");
    if (!csvp->isGood()) fileNotFound(curFile);
    parseAgencies(targetFeed, csvp.get());
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseShapes(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/shapes.txt";
  try {
    auto csvp = getCsvParser("shapes.txt");
    if (csvp->isGood()) {
      parseShapes(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseTrips(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/trips.txt";
  try {
    auto csvp = getCsvParser("trips.txt");
    if (!csvp->isGood()) fileNotFound(curFile);
    parseTrips(targetFeed, csvp.get());
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseStopTimes(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/stop_times.txt";
  try {
    auto csvp = getCsvParser("stop_times.txt");
    if (!csvp->isGood()) fileNotFound(curFile);
    parseStopTimes(targetFeed, csvp.get());
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFareRules(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/fare_rules.txt";
  try {
    auto csvp = getCsvParser("fare_rules.txt");
    if (csvp->isGood()) {
      parseFareRules(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFareAttributes(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/fare_attributes.txt";
  try {
    auto csvp = getCsvParser("fare_attributes.txt");
    if (csvp->isGood()) {
      parseFareAttributes(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseTranslations(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/translations.txt";
  try {
    auto csvp = getCsvParser("translations.txt");
    if (csvp->isGood()) {
      parseTranslations(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseAttributions(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/attributions.txt";
  try {
    auto csvp = getCsvParser("attributions.txt");
    if (csvp->isGood()) {
      parseAttributions(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseTransfers(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/transfers.txt";
  try {
    auto csvp = getCsvParser("transfers.txt");
    if (csvp->isGood()) {
      parseTransfers(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseFrequencies(gtfs::FEEDB* targetFeed) const {
  std::string curFile = _path + "/frequencies.txt";
  try {
    auto csvp = getCsvParser("frequencies.txt");
    if (csvp->isGood()) {
      parseFrequencies(targetFeed, csvp.get());
    }
  } catch (const CsvParserException& e) {
    throw ParserException(e.getMsg(), e.getFieldName(), e.getLine(), curFile);
  }
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseShapes(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::ShapePoint fp;
  auto flds = getShapeFlds(csvp);

  while (nextShapePoint(csvp, &fp, flds)) {
    if (!targetFeed->getShapes().has(fp.id)) {
      targetFeed->getShapes().add(ShapeT(fp.id));
    }

    auto s = targetFeed->getShapes().get(fp.id);
    targetFeed->updateBox(fp.lat, fp.lng);

    if (s) {
      if (!s->addPoint(ShapePoint(fp.lat, fp.lng, fp.travelDist, fp.seq))) {
        throw ParserException(
            "shape_pt_sequence collision,"
            "shape_pt_sequence has "
            "to be increasing for a single shape.",
            "shape_pt_sequence", csvp->getCurLine(), csvp->getReadablePath());
      }
    }
  }

  targetFeed->getShapes().finalize();
}

// ____________________________________________________________________________
inline gtfs::flat::StopTimeFlds Parser::getStopTimeFlds(CsvParser* csvp) {
  gtfs::flat::StopTimeFlds s;
  s.stopIdFld = csvp->getFieldIndex("stop_id");
  s.tripIdFld = csvp->getFieldIndex("trip_id");
  s.arrivalTimeFld = csvp->getFieldIndex("arrival_time");
  s.departureTimeFld = csvp->getFieldIndex("departure_time");
  s.stopSequenceFld = csvp->getFieldIndex("stop_sequence");
  s.stopHeadsignFld = csvp->getOptFieldIndex("stop_headsign");
  s.shapeDistTraveledFld = csvp->getOptFieldIndex("shape_dist_traveled");
  s.timepointFld = csvp->getOptFieldIndex("timepoint");
  s.pickUpTypeFld = csvp->getOptFieldIndex("pickup_type");
  s.dropOffTypeFld = csvp->getOptFieldIndex("drop_off_type");
  s.continuousDropOffFld = csvp->getOptFieldIndex("continuous_drop_off");
  s.continuousPickupFld = csvp->getOptFieldIndex("continuous_pickup");
  return s;
}

// ____________________________________________________________________________
inline bool Parser::nextStopTime(CsvParser* csvp, gtfs::flat::StopTime* s,
                                 const gtfs::flat::StopTimeFlds& flds) const {
  if (csvp->readNextLine()) {
    s->at = getTime(*csvp, flds.arrivalTimeFld);
    s->dt = getTime(*csvp, flds.departureTimeFld);

    if (s->at.empty() && !s->dt.empty()) s->at = s->dt;
    if (s->dt.empty() && !s->at.empty()) s->dt = s->at;

    s->trip = getString(*csvp, flds.tripIdFld);
    s->s = getString(*csvp, flds.stopIdFld);
    s->sequence = getRangeInteger(*csvp, flds.stopSequenceFld, 0, UINT32_MAX);
    s->headsign = getString(*csvp, flds.stopHeadsignFld, "");
    s->pickupType = static_cast<gtfs::flat::StopTime::PU_DO_TYPE>(
        getRangeInteger(*csvp, flds.pickUpTypeFld, 0, 3, 0));
    s->dropOffType = static_cast<gtfs::flat::StopTime::PU_DO_TYPE>(
        getRangeInteger(*csvp, flds.dropOffTypeFld, 0, 3, 0));

    s->continuousDropOff =
        getRangeInteger(*csvp, flds.continuousDropOffFld, 0, 3, 1);
    s->continuousPickup =
        getRangeInteger(*csvp, flds.continuousPickupFld, 0, 3, 1);

    // if at and dt are empty, default to 0 here
    s->isTimepoint = getRangeInteger(*csvp, flds.timepointFld, 0, 1,
                                     !(s->at.empty() && s->dt.empty()));

    if (s->isTimepoint && s->at.empty() && s->dt.empty()) {
      throw ParserException(
          "if arrival_time and departure_time are empty, timepoint cannot be 1",
          "timepoint", csvp->getCurLine(), csvp->getReadablePath());
    }

    s->shapeDistTravelled = -1;  // using -1 as a null value here
    if (flds.shapeDistTraveledFld < csvp->getNumColumns()) {
      if (!getString(*csvp, flds.shapeDistTraveledFld, "").empty()) {
        s->shapeDistTravelled = getDouble(*csvp, flds.shapeDistTraveledFld);
        if (s->shapeDistTravelled <
            -0.01) {  // TODO(patrick): better double comp
          throw ParserException(
              "negative values not supported for distances"
              " (value was: " +
                  std::to_string(s->shapeDistTravelled),
              "shape_dist_traveled", csvp->getCurLine(),
              csvp->getReadablePath());
        }
      }
    }
    return true;
  }

  return false;
}

// ____________________________________________________________________________
FEEDTPL
void Parser::parseStopTimes(gtfs::FEEDB* targetFeed, CsvParser* csvp) const {
  gtfs::flat::StopTime fst;
  auto flds = getStopTimeFlds(csvp);

  while (nextStopTime(csvp, &fst, flds)) {
    StopT* stop = 0;
    TripB<StopTimeT<StopT>, ServiceT, RouteT, ShapeT>* trip = 0;

    stop = targetFeed->getStops().get(fst.s);
    trip = targetFeed->getTrips().get(fst.trip);

    if (!stop) {
      std::stringstream msg;
      msg << "no stop with id '" << fst.s << "' defined in stops.txt, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "stop_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (!trip) {
      std::stringstream msg;
      msg << "no trip with id '" << fst.trip
          << "' defined in trips.txt, cannot "
          << "reference here.";
      throw ParserException(msg.str(), "trip_id", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    StopTimeT<StopT> st(fst.at, fst.dt, stop, fst.sequence, fst.headsign,
                        fst.pickupType, fst.dropOffType, fst.shapeDistTravelled,
                        fst.isTimepoint, fst.continuousDropOff,
                        fst.continuousPickup);

    if (st.getArrivalTime() > st.getDepartureTime()) {
      throw ParserException("arrival time '" + st.getArrivalTime().toString() +
                                "' is later than departure time '" +
                                st.getDepartureTime().toString() +
                                "'. You cannot depart earlier than you arrive.",
                            "departure_time", csvp->getCurLine(),
                            csvp->getReadablePath());
    }

    if (!trip->addStopTime(st)) {
      throw ParserException(
          "stop_sequence collision, stop_sequence has "
          "to be increasing for a single trip.",
          "stop_sequence", csvp->getCurLine(), csvp->getReadablePath());
    }
  }
}

// ___________________________________________________________________________
void Parser::fileNotFound(const std::string& file) const {
  throw ParserException("File not found", "", -1, std::string(file.c_str()));
}

// ___________________________________________________________________________
std::string Parser::getString(const CsvParser& csv, size_t field) const {
  const char* r = csv.getTString(field);
  if (r[0] == 0) {
    throw ParserException("expected non-empty string", csv.getFieldName(field),
                          csv.getCurLine(), csv.getReadablePath());
  }
  return r;
}

// ___________________________________________________________________________
std::string Parser::getString(const CsvParser& csv, size_t field,
                              const std::string& def) const {
  if (field < csv.getNumColumns() && !csv.fieldIsEmpty(field)) {
    return csv.getTString(field);
  }

  return def;
}

// ___________________________________________________________________________
double Parser::getDouble(const CsvParser& csv, size_t field) const {
  return csv.getDouble(field);
}

// ___________________________________________________________________________
double Parser::getDouble(const CsvParser& csv, size_t field, double ret) const {
  if (field < csv.getNumColumns() && !csv.fieldIsEmpty(field)) {
    return csv.getDouble(field);
  }

  return ret;
}

// ___________________________________________________________________________
int64_t Parser::getRangeInteger(const CsvParser& csv, size_t field,
                                int64_t minv, int64_t maxv) const {
  int64_t ret = csv.getLong(field);

  if (ret < minv || ret > maxv) {
    std::stringstream msg;
    msg << "expected integer in range [" << minv << "," << maxv << "]";
    throw ParserException(msg.str(), csv.getFieldName(field), csv.getCurLine(),
                          csv.getReadablePath());
  }

  return ret;
}

// ___________________________________________________________________________
int64_t Parser::getRangeInteger(const CsvParser& csv, size_t field,
                                int64_t minv, int64_t maxv, int64_t def) const {
  int64_t ret;

  if (field < csv.getNumColumns() && !csv.fieldIsEmpty(field)) {
    ret = csv.getLong(field);

    if (ret < minv || ret > maxv) {
      std::stringstream msg;
      msg << "expected integer in range [" << minv << "," << maxv << "]";
      if (_strict) {
        throw ParserException(msg.str(), csv.getFieldName(field),
                              csv.getCurLine(), csv.getReadablePath());
      } else {
        if (_warnCb)
          _warnCb(ParserException(msg.str(), csv.getFieldName(field),
                                  csv.getCurLine(), csv.getReadablePath())
                      .what());
        return def;
      }
    }

    return ret;
  }

  return def;
}

// ___________________________________________________________________________
uint32_t Parser::getColorFromHexString(const CsvParser& csv, size_t field,
                                       const std::string& def) const {
  std::string color_string;

  if (field < csv.getNumColumns()) {
    color_string = csv.getTString(field);
  }

  if (color_string.empty()) color_string = def;

  if (color_string.empty()) return std::numeric_limits<uint32_t>::max();

  size_t chars_processed = 0;
  uint32_t ret = 0;

  try {
    ret = std::stoul("0x" + color_string, &chars_processed, 16);
  } catch (const std::exception& e) {
    std::stringstream msg;
    msg << "expected a 6-character hexadecimal color string, found '"
        << color_string << "' instead. (Error while parsing was: " << e.what()
        << ")";
    if (_strict) {
      throw ParserException(msg.str(), csv.getFieldName(field),
                            csv.getCurLine(), csv.getReadablePath());
    } else {
      if (_warnCb)
        _warnCb(ParserException(msg.str(), csv.getFieldName(field),
                                csv.getCurLine(), csv.getReadablePath())
                    .what());
      return std::stoul("0x" + def, &chars_processed, 16);
    }
  }

  if (color_string.size() != 6 || chars_processed != 8) {
    std::stringstream msg;
    msg << "expected a 6-character hexadecimal color string, found '"
        << color_string << "' instead.";
    if (_strict) {
      throw ParserException(msg.str(), csv.getFieldName(field),
                            csv.getCurLine(), csv.getReadablePath());
    } else {
      if (_warnCb)
        _warnCb(ParserException(msg.str(), csv.getFieldName(field),
                                csv.getCurLine(), csv.getReadablePath())
                    .what());
      return std::stoul("0x" + def, &chars_processed, 16);
    }
  }

  return ret;
}

// ____________________________________________________________________________
ServiceDate Parser::getServiceDate(const CsvParser& csv, size_t field) const {
  return getServiceDate(csv, field, false);
}

// ____________________________________________________________________________
ServiceDate Parser::getServiceDate(const CsvParser& csv, size_t field,
                                   bool req) const {
  if (field >= csv.getNumColumns()) {
    if (!req) {
      return ServiceDate();
    } else {
      std::stringstream msg;
      msg << "expected a date in the YYYYMMDD format";
      throw ParserException(msg.str(), csv.getFieldName(field),
                            csv.getCurLine(), csv.getReadablePath());
    }
  }
  const char* val = csv.getTString(field);
  if (strlen(val) == 0 && !req) return ServiceDate();

  try {
    uint32_t yyyymmdd = atoi(&val);
    if (*val != 0 || yyyymmdd > 99999999) {
      std::stringstream msg;
      msg << "expected a date in the YYYYMMDD format, found '"
          << csv.getTString(field) << "' instead.";
      throw ParserException(msg.str(), csv.getFieldName(field),
                            csv.getCurLine(), csv.getReadablePath());
    }
    return ServiceDate(yyyymmdd);
  } catch (const std::out_of_range& e) {
    std::stringstream msg;
    msg << "expected a date in the YYYYMMDD format, found '" << val
        << "' instead. (Integer out of range).";
    throw ParserException(msg.str(), csv.getFieldName(field), csv.getCurLine(),
                          csv.getReadablePath());
  } catch (const std::invalid_argument& e) {
    std::stringstream msg;
    msg << "expected a date in the YYYYMMDD format, found '" << val
        << "' instead.";
    throw ParserException(msg.str(), csv.getFieldName(field), csv.getCurLine(),
                          csv.getReadablePath());
  }
}

// ____________________________________________________________________________
Time Parser::getTime(const CsvParser& csv, size_t field) const {
  const char* val = csv.getTString(field);

  // TODO(patrick): null value
  if (val[0] == 0) return Time();

  try {
    uint32_t h = atoi(&val);
    if (h > 255)
      throw std::out_of_range(
          "only non-negative hour-values up to 255 are "
          "supported.");
    if (*val != ':') {
      throw std::invalid_argument("invalid separator");
    }

    val++;

    uint32_t m = atoi(&val);
    // allow values of 60, although standard forbids it
    if (m > 60)
      throw std::out_of_range(
          "only non-negative minute-values up to 60 are "
          "allowed.");

    // allow missing second values, although standard forbids it
    uint32_t s = 0;

    if (*val == ':') {
      val++;
      s = atoi(&val);
    }

    // allow values of 60, although standard forbids it
    if (s > 60)
      throw std::out_of_range(
          "only non-negative second-values up to 60 are "
          "allowed.");

    return Time(h, m % 60, s % 60);
  } catch (const std::exception& e) {
    std::stringstream msg;
    msg << "expected a time in HH:MM:SS (or H:MM:SS) format, found '"
        << csv.getTString(field) << "' instead. (" << e.what() << ")";
    throw ParserException(msg.str(), csv.getFieldName(field), csv.getCurLine(),
                          csv.getReadablePath());
  }
}

// ____________________________________________________________________________
gtfs::flat::Route::TYPE Parser::getRouteType(const CsvParser& csv, size_t field,
                                             int64_t tn) const {
  auto t = gtfs::flat::Route::getRouteType(tn);
  if (t == gtfs::flat::Route::NONE) {
    std::stringstream msg;
    msg << "route type '" << tn << "' not supported.";
    throw ParserException(msg.str(), csv.getFieldName(field), csv.getCurLine(),
                          csv.getReadablePath());
  }
  return t;
}

// ___________________________________________________________________________
inline uint32_t Parser::atoi(const char** p) {
  uint32_t x = 0;
  if (**p < '0' || **p > '9') return -1;
  while (**p >= '0' && **p <= '9') {
    x = (x * 10) + (**p - '0');
    ++(*p);
  }
  return x;
}

// ___________________________________________________________________________
inline std::unique_ptr<CsvParser> Parser::getCsvParser(
    const std::string& file) const {
#ifdef LIBZIP_FOUND
  if (_za)
    return std::unique_ptr<CsvParser>(
        new ZipCsvParser(_za, file, _path + "/" + file));
#endif
  return std::unique_ptr<CsvParser>(new CsvParser(_path + "/" + file));
}

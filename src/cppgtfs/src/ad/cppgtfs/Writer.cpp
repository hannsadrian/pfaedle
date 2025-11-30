// Copyright 2018, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include <cmath>
#include <fstream>
#include <map>
#include <string>
#include <utility>

#include "Writer.h"
#include "ad/util/CsvWriter.h"
#include "gtfs/Shape.h"
#include "gtfs/Trip.h"
#include "gtfs/flat/Agency.h"

using ad::cppgtfs::Writer;
using ad::cppgtfs::gtfs::AddFlds;
using ad::cppgtfs::gtfs::Agency;
using ad::cppgtfs::gtfs::Route;
using ad::cppgtfs::gtfs::Service;
using ad::cppgtfs::gtfs::ServiceDate;
using ad::cppgtfs::gtfs::Shape;
using ad::cppgtfs::gtfs::ShapePoint;
using ad::cppgtfs::gtfs::Stop;
using ad::cppgtfs::gtfs::StopTime;
using ad::cppgtfs::gtfs::Time;
using ad::cppgtfs::gtfs::Trip;
using ad::util::CsvWriter;

// ____________________________________________________________________________
bool Writer::write(gtfs::Feed* sourceFeed, const std::string& path) const {
  std::ofstream fs;
  std::string gtfsPath(path);
  std::string curFile;

  curFile = gtfsPath + "/shapes.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  writeShapes(sourceFeed, &fs);
  fs.close();

  curFile = gtfsPath + "/trips.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  bool hasFreqs = writeTrips(sourceFeed, &fs);
  fs.close();

  curFile = gtfsPath + "/agency.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  writeAgencies(sourceFeed, &fs);
  fs.close();

  curFile = gtfsPath + "/stops.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  writeStops(sourceFeed, &fs);
  fs.close();

  curFile = gtfsPath + "/stop_times.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  writeStopTimes(sourceFeed, &fs);
  fs.close();

  curFile = gtfsPath + "/routes.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  writeRoutes(sourceFeed, &fs);
  fs.close();

  if (!sourceFeed->getPublisherUrl().empty() &&
      !sourceFeed->getPublisherName().empty()) {
    curFile = gtfsPath + "/feed_info.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeFeedInfo(sourceFeed, &fs);
    fs.close();
  }

  curFile = gtfsPath + "/calendar.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  writeCalendars(sourceFeed, &fs);
  fs.close();

  curFile = gtfsPath + "/calendar_dates.txt";
  fs.open(curFile.c_str());
  if (!fs.good()) cannotWrite(curFile);
  writeCalendarDates(sourceFeed, &fs);
  fs.close();

  if (hasFreqs) {
    curFile = gtfsPath + "/frequencies.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeFrequencies(sourceFeed, &fs);
    fs.close();
  }

  if (sourceFeed->getTransfers().size()) {
    curFile = gtfsPath + "/transfers.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeTransfers(sourceFeed, &fs);
    fs.close();
  }

  if (sourceFeed->getFares().size()) {
    curFile = gtfsPath + "/fare_attributes.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeFares(sourceFeed, &fs);
    fs.close();

    curFile = gtfsPath + "/fare_rules.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeFareRules(sourceFeed, &fs);
    fs.close();
  }

  if (sourceFeed->getLevels().size()) {
    curFile = gtfsPath + "/levels.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeLevels(sourceFeed, &fs);
    fs.close();
  }

  if (sourceFeed->getPathways().size()) {
    curFile = gtfsPath + "/pathways.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writePathways(sourceFeed, &fs);
    fs.close();
  }

  if (sourceFeed->getAttributions().size()) {
    curFile = gtfsPath + "/attributions.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeAttributions(sourceFeed, &fs);
    fs.close();
  }

  if (sourceFeed->getTranslations().size()) {
    curFile = gtfsPath + "/translations.txt";
    fs.open(curFile.c_str());
    if (!fs.good()) cannotWrite(curFile);
    writeTranslations(sourceFeed, &fs);
    fs.close();
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getAgencyCsvw(std::ostream* os,
                                                 const gtfs::AddFlds& addFlds) {
  ad::util::HeaderList headers{"agency_id",       "agency_name", "agency_url",
                               "agency_timezone", "agency_lang", "agency_phone",
                               "agency_fare_url", "agency_email"};

  for (auto i : addFlds) {
    headers.push_back(i.first);
  }

  return std::unique_ptr<CsvWriter>(new CsvWriter(os, headers));
}

// ____________________________________________________________________________
bool Writer::writeAgency(const gtfs::flat::Agency& ag, CsvWriter* csvw,
                         const gtfs::AddFlds& addFlds) const {
  csvw->writeString(ag.id);
  csvw->writeString(ag.name);
  csvw->writeString(ag.url);
  csvw->writeString(ag.timezone);
  csvw->writeString(ag.lang);
  csvw->writeString(ag.phone);
  csvw->writeString(ag.fare_url);
  csvw->writeString(ag.agency_email);

  for (const auto& addFld : addFlds) {
    auto v = addFld.second.find(ag.id);
    if (v == addFld.second.end()) {
      csvw->writeString("");
    } else {
      csvw->writeString(v->second);
    }
  }

  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
bool Writer::writeAgencies(gtfs::Feed* sourceFeed, std::ostream* s) const {
  auto csvw = getAgencyCsvw(s, sourceFeed->getAgencyAddFlds());
  for (const auto& a : sourceFeed->getAgencies()) {
    writeAgency(a.second->getFlat(), csvw.get(),
                sourceFeed->getAgencyAddFlds());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getStopsCsvw(std::ostream* os,
                                                const gtfs::AddFlds& addFlds) {
  ad::util::HeaderList headers{
      "stop_id",       "stop_code",      "stop_name",     "stop_desc",
      "stop_lat",      "stop_lon",       "zone_id",       "stop_url",
      "location_type", "parent_station", "stop_timezone", "wheelchair_boarding",
      "platform_code", "level_id"};

  for (auto i : addFlds) {
    headers.push_back(i.first);
  }

  return std::unique_ptr<CsvWriter>(new CsvWriter(os, headers));
}

// ____________________________________________________________________________
bool Writer::writeStop(const gtfs::flat::Stop& s, CsvWriter* csvw,
                       const gtfs::AddFlds& addFlds) const {
  csvw->writeString(s.id);
  csvw->writeString(s.code);
  csvw->writeString(s.name);
  csvw->writeString(s.desc);
  if (fabs(s.lat) > 90 || fabs(s.lng) > 180) {
    csvw->skip();
    csvw->skip();
  } else {
    csvw->writeDouble(s.lat, 6);
    csvw->writeDouble(s.lng, 6);
  }
  csvw->writeString(s.zone_id);
  csvw->writeString(s.stop_url);
  csvw->writeInt(s.location_type);
  if (!s.parent_station.empty())
    csvw->writeString(s.parent_station);
  else
    csvw->skip();
  csvw->writeString(s.stop_timezone);
  csvw->writeInt(s.wheelchair_boarding);
  csvw->writeString(s.platform_code);
  csvw->writeString(s.level_id);

  for (const auto& addFld : addFlds) {
    auto v = addFld.second.find(s.id);
    if (v == addFld.second.end()) {
      csvw->writeString("");
    } else {
      csvw->writeString(v->second);
    }
  }

  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
bool Writer::writeStops(gtfs::Feed* sourceFeed, std::ostream* s) const {
  auto csvw = getStopsCsvw(s, sourceFeed->getStopAddFlds());

  for (const auto& t : sourceFeed->getStops()) {
    writeStop(t.second->getFlat(), csvw.get(), sourceFeed->getStopAddFlds());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getTripsCsvw(std::ostream* os,
                                                const AddFlds& addFlds) {
  ad::util::HeaderList headers{
      "route_id",      "service_id",      "trip_id",
      "trip_headsign", "trip_short_name", "direction_id",
      "block_id",      "shape_id",        "wheelchair_accessible",
      "bikes_allowed"};

  for (auto i : addFlds) {
    headers.push_back(i.first);
  }

  return std::unique_ptr<CsvWriter>(new CsvWriter(os, headers));
}

// ____________________________________________________________________________
bool Writer::writeTrip(const gtfs::flat::Trip& t, CsvWriter* csvw,
                       const AddFlds& addFlds) const {
  csvw->writeString(t.route);
  csvw->writeString(t.service);
  csvw->writeString(t.id);
  csvw->writeString(t.headsign);
  csvw->writeString(t.short_name);
  if (t.dir < 2)
    csvw->writeInt(t.dir);
  else
    csvw->skip();
  csvw->writeString(t.block_id);
  if (!t.shape.empty())
    csvw->writeString(t.shape);
  else
    csvw->skip();
  csvw->writeInt(t.wc);
  csvw->writeInt(t.ba);

  for (const auto& addFld : addFlds) {
    auto v = addFld.second.find(t.id);
    if (v == addFld.second.end()) {
      csvw->writeString("");
    } else {
      csvw->writeString(v->second);
    }
  }

  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeTrips(gtfs::Feed* sourceFeed, std::ostream* s) const {
  bool hasFreqs = false;
  auto csvw = getTripsCsvw(s, sourceFeed->getTripAddFlds());
  for (const auto& t : sourceFeed->getTrips()) {
    if (t.second->getFrequencies().size()) hasFreqs = true;
    writeTrip(t.second->getFlat(), csvw.get(), sourceFeed->getTripAddFlds());
  }

  return hasFreqs;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getStopTimesCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os,
      {"trip_id", "arrival_time", "departure_time", "stop_id", "stop_sequence",
       "stop_headsign", "pickup_type", "drop_off_type", /* "shape_dist_traveled", */
       "timepoint", "continuous_pickup", "continuous_drop_off"}));
}

// ____________________________________________________________________________
bool Writer::writeStopTime(const gtfs::flat::StopTime& st,
                           CsvWriter* csvw) const {
  csvw->writeString(st.trip);
  csvw->writeString(st.at.toString());
  csvw->writeString(st.dt.toString());
  csvw->writeString(st.s);
  csvw->writeInt(st.sequence);
  csvw->writeString(st.headsign);
  csvw->writeInt(st.pickupType);
  csvw->writeInt(st.dropOffType);

  /*
  if (st.shapeDistTravelled > -.5)
    csvw->writeDouble(st.shapeDistTravelled, 3);
  else
    csvw->skip();
  */
  csvw->writeInt(st.isTimepoint);

  if (st.continuousPickup != 1)
    csvw->writeInt(st.continuousPickup);
  else
    csvw->skip();

  if (st.continuousDropOff != 1)
    csvw->writeInt(st.continuousDropOff);
  else
    csvw->skip();

  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeStopTimes(gtfs::Feed* sourceFeed, std::ostream* s) const {
  auto csvw = getStopTimesCsvw(s);

  for (const auto& t : sourceFeed->getTrips()) {
    for (const auto& p : t.second->getStopTimes()) {
      writeStopTime(
          gtfs::flat::StopTime{
              p.getArrivalTime(), p.getDepartureTime(), t.second->getId(),
              p.getStop()->getId(), p.getSeq(), p.getHeadsign(),
              p.getPickupType(), p.getDropOffType(), p.isTimepoint(),
              p.getShapeDistanceTravelled(), p.getContinuousDropOff(),
              p.getContinuousPickup()},
          csvw.get());
    }
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getShapesCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(
      new CsvWriter(os, {"shape_id", "shape_pt_lat", "shape_pt_lon",
                         "shape_pt_sequence", "shape_dist_traveled"}));
}

// ____________________________________________________________________________
bool Writer::writeShapePoint(const gtfs::flat::ShapePoint& s,
                             CsvWriter* csvw) const {
  csvw->writeString(s.id);
  csvw->writeDouble(s.lat, 6);
  csvw->writeDouble(s.lng, 6);
  csvw->writeInt(s.seq);
  if (s.travelDist > -0.5)
    csvw->writeDouble(s.travelDist, 3);
  else
    csvw->skip();
  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeShapes(gtfs::Feed* sourceFeed, std::ostream* s) const {
  auto csvw = getShapesCsvw(s);
  csvw->flushLine();
  for (const auto& t : sourceFeed->getShapes()) {
    for (const auto& p : t.second->getPoints()) {
      writeShapePoint(gtfs::flat::ShapePoint{t.second->getId(), p.lat, p.lng,
                                             p.travelDist, p.seq},
                      csvw.get());
    }
  }

  return true;
}

// ____________________________________________________________________________
bool Writer::writeRoute(const gtfs::flat::Route& s, CsvWriter* csvw,
                        const AddFlds& addFlds) const {
  csvw->writeString(s.id);
  if (!s.agency.empty())
    csvw->writeString(s.agency);
  else
    csvw->skip();
  csvw->writeString(s.short_name);
  csvw->writeString(s.long_name);
  csvw->writeString(s.desc);
  csvw->writeInt(s.type);
  csvw->writeString(s.url);

  auto colorStr = gtfs::flat::Route::getHexColorString(s.color);
  csvw->writeString(colorStr);
  auto textColorStr = gtfs::flat::Route::getHexColorString(s.text_color);
  csvw->writeString(textColorStr);

  if (s.sort_order != std::numeric_limits<int64_t>::max())
    csvw->writeInt(s.sort_order);
  else
    csvw->skip();

  if (s.continuous_pickup != 1)
    csvw->writeInt(s.continuous_pickup);
  else
    csvw->skip();

  if (s.continuous_drop_off != 1)
    csvw->writeInt(s.continuous_drop_off);
  else
    csvw->skip();

  for (const auto& addFld : addFlds) {
    auto v = addFld.second.find(s.id);
    if (v == addFld.second.end()) {
      csvw->writeString("");
    } else {
      csvw->writeString(v->second);
    }
  }

  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getRoutesCsvw(std::ostream* os,
                                                 const gtfs::AddFlds& addFlds) {
  ad::util::HeaderList headers{
      "route_id",         "agency_id",         "route_short_name",
      "route_long_name",  "route_desc",        "route_type",
      "route_url",        "route_color",       "route_text_color",
      "route_sort_order", "continuous_pickup", "continuous_drop_off"};

  for (auto i : addFlds) {
    headers.push_back(i.first);
  }

  return std::unique_ptr<CsvWriter>(new CsvWriter(os, headers));
}

// ____________________________________________________________________________
bool Writer::writeRoutes(gtfs::Feed* sourceFeed, std::ostream* s) const {
  auto csvw = getRoutesCsvw(s, sourceFeed->getRouteAddFlds());
  csvw->flushLine();
  for (const auto& a : sourceFeed->getRoutes()) {
    writeRoute(a.second->getFlat(), csvw.get(), sourceFeed->getRouteAddFlds());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getFeedInfoCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os, {"feed_publisher_name", "feed_publisher_url", "feed_lang",
           "feed_start_date", "feed_end_date", "feed_version",
           "feed_contact_email", "feed_contact_url", "default_lang"}));
}

// ____________________________________________________________________________
bool Writer::writeFeedInfo(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getFeedInfoCsvw(os);
  csvw->flushLine();
  csvw->writeString(f->getPublisherName());
  csvw->writeString(f->getPublisherUrl());
  csvw->writeString(f->getLang());
  if (!f->getStartDate().empty())
    csvw->writeInt(f->getStartDate().getYYYYMMDD());
  else
    csvw->skip();
  if (!f->getEndDate().empty())
    csvw->writeInt(f->getEndDate().getYYYYMMDD());
  else
    csvw->skip();
  csvw->writeString(f->getVersion());
  csvw->writeString(f->getContactEmail());
  csvw->writeString(f->getContactUrl());
  csvw->writeString(f->getDefaultLang());
  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getTransfersCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os,
      {"from_stop_id", "to_stop_id", "from_route_id", "to_route_id",
       "from_trip_id", "to_trip_id", "transfer_type", "min_transfer_time"}));
}

// ____________________________________________________________________________
bool Writer::writeTransfer(const gtfs::flat::Transfer& t,
                           CsvWriter* csvw) const {
  csvw->writeString(t.fromStop);
  csvw->writeString(t.toStop);
  csvw->writeString(t.fromRoute);
  csvw->writeString(t.toRoute);
  csvw->writeString(t.fromTrip);
  csvw->writeString(t.toTrip);
  csvw->writeInt(t.type);
  if (t.tTime > -1)
    csvw->writeInt(t.tTime);
  else
    csvw->skip();
  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeTransfers(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getTransfersCsvw(os);
  csvw->flushLine();

  for (const auto& t : f->getTransfers()) {
    writeTransfer(t.getFlat(), csvw.get());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getFaresCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(
      new CsvWriter(os, {"fare_id", "price", "currency_type", "payment_method",
                         "transfers", "agency_id", "transfer_duration"}));
}

// ____________________________________________________________________________
bool Writer::writeFare(const gtfs::flat::Fare& t, CsvWriter* csvw) const {
  csvw->writeString(t.id);
  csvw->writeDouble(t.price, 2);
  csvw->writeString(t.currencyType);
  csvw->writeInt(t.paymentMethod);
  csvw->writeInt(t.numTransfers);
  if (!t.agency.empty())
    csvw->writeString(t.agency);
  else
    csvw->skip();
  if (t.duration > -1)
    csvw->writeInt(t.duration);
  else
    csvw->skip();
  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeFares(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getFaresCsvw(os);
  csvw->flushLine();

  for (const auto& r : f->getFares()) {
    writeFare(r.second->getFlat(), csvw.get());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getFareRulesCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os,
      {"fare_id", "route_id", "origin_id", "destination_id", "contains_id"}));
}

// ____________________________________________________________________________
bool Writer::writeFareRule(const gtfs::flat::FareRule& t,
                           CsvWriter* csvw) const {
  csvw->writeString(t.fare);
  if (!t.route.empty())
    csvw->writeString(t.route);
  else
    csvw->skip();
  if (!t.originZone.empty())
    csvw->writeString(t.originZone);
  else
    csvw->skip();
  if (!t.destZone.empty())
    csvw->writeString(t.destZone);
  else
    csvw->skip();
  if (!t.containsZone.empty())
    csvw->writeString(t.containsZone);
  else
    csvw->skip();
  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeFareRules(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getFareRulesCsvw(os);
  csvw->flushLine();

  for (const auto& fare : f->getFares()) {
    for (const auto& r : fare.second->getFareRules()) {
      writeFareRule(
          gtfs::flat::FareRule{
              fare.second->getId(), r.getRoute() ? r.getRoute()->getId() : "",
              r.getOriginId(), r.getDestId(), r.getContainsId()},
          csvw.get());
    }
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getCalendarCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os, {"service_id", "monday", "tuesday", "wednesday", "thursday", "friday",
           "saturday", "sunday", "start_date", "end_date"}));
}

// ____________________________________________________________________________
bool Writer::writeCalendar(const gtfs::flat::Calendar& s,
                           CsvWriter* csvw) const {
  csvw->writeString(s.id);
  csvw->writeInt((bool)(s.serviceDays & gtfs::Service::SERVICE_DAY::MONDAYS));
  csvw->writeInt((bool)(s.serviceDays & gtfs::Service::SERVICE_DAY::TUESDAYS));
  csvw->writeInt(
      (bool)(s.serviceDays & gtfs::Service::SERVICE_DAY::WEDNESDAYS));
  csvw->writeInt((bool)(s.serviceDays & gtfs::Service::SERVICE_DAY::THURSDAYS));
  csvw->writeInt((bool)(s.serviceDays & gtfs::Service::SERVICE_DAY::FRIDAYS));
  csvw->writeInt((bool)(s.serviceDays & gtfs::Service::SERVICE_DAY::SATURDAYS));
  csvw->writeInt((bool)(s.serviceDays & gtfs::Service::SERVICE_DAY::SUNDAYS));
  csvw->writeInt(s.begin.getYYYYMMDD());
  csvw->writeInt(s.end.getYYYYMMDD());
  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
bool Writer::writeCalendars(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getCalendarCsvw(os);
  csvw->flushLine();
  for (const auto& r : f->getServices()) {
    if (!r.second->hasServiceDays()) continue;
    writeCalendar(r.second->getFlat(), csvw.get());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getCalendarDatesCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(
      new CsvWriter(os, {"service_id", "date", "exception_type"}));
}

// ____________________________________________________________________________
bool Writer::writeCalendarDate(const gtfs::flat::CalendarDate& s,
                               CsvWriter* csvw) const {
  csvw->writeString(s.id);
  csvw->writeInt(s.date.getYYYYMMDD());
  csvw->writeInt(s.type);
  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
bool Writer::writeCalendarDates(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getCalendarDatesCsvw(os);
  csvw->flushLine();

  for (const auto& r : f->getServices()) {
    for (const auto& e : r.second->getExceptions()) {
      gtfs::flat::CalendarDate cd;
      cd.date = e.first;
      cd.id = r.second->getId();
      cd.type = e.second;
      writeCalendarDate(cd, csvw.get());
    }
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getFrequencyCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os,
      {"trip_id", "start_time", "end_time", "headway_secs", "exact_times"}));
}

// ____________________________________________________________________________
bool Writer::writeFrequency(const gtfs::flat::Frequency& f,
                            CsvWriter* csvw) const {
  csvw->writeString(f.tripId);
  csvw->writeString(f.startTime.toString());
  csvw->writeString(f.endTime.toString());
  csvw->writeInt(f.headwaySecs);
  csvw->writeInt(f.exactTimes);
  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeFrequencies(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getFrequencyCsvw(os);
  csvw->flushLine();

  for (const auto& t : f->getTrips()) {
    for (const auto& f : t.second->getFrequencies()) {
      writeFrequency(gtfs::flat::Frequency{t.second->getId(), f.getStartTime(),
                                           f.getEndTime(), f.getHeadwaySecs(),
                                           f.hasExactTimes()},
                     csvw.get());
    }
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getAttributionCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(
      new CsvWriter(os, {"organization_name", "attribution_url", "is_producer",
                         "is_operator", "is_authority"}));
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getPathwayCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os,
      {"pathway_id", "from_stop_id", "to_stop_id", "pathway_mode",
       "is_bidirectional", "length", "traversal_time", "stair_count",
       "max_slope", "min_width", "signposted_as", "reversed_signposted_as"}));
}

// ____________________________________________________________________________
bool Writer::writePathway(const gtfs::flat::Pathway& l, CsvWriter* csvw) const {
  csvw->writeString(l.id);
  csvw->writeString(l.from_stop_id);
  csvw->writeString(l.to_stop_id);
  csvw->writeInt(l.pathway_mode);
  csvw->writeInt(l.is_bidirectional);
  if (l.length < 0)
    csvw->skip();
  else
    csvw->writeDouble(l.length, 4);

  if (l.traversal_time < 0)
    csvw->skip();
  else
    csvw->writeInt(l.traversal_time);

  if (l.stair_count == 0)
    csvw->skip();
  else
    csvw->writeInt(l.stair_count);

  if (l.max_slope == 0)
    csvw->skip();
  else
    csvw->writeDouble(l.max_slope, 4);

  if (l.min_width < 0)
    csvw->skip();
  else
    csvw->writeDouble(l.min_width, 4);

  csvw->writeString(l.signposted_as);
  csvw->writeString(l.reversed_signposted_as);
  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getAttributionsCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os, {"attribution_id", "agency_id", "route_id", "trip_id",
           "organization_name", "is_producer", "is_operator", "is_authority",
           "attribution_url", "attribution_email", "attribution_phone"}));
}

// ____________________________________________________________________________
bool Writer::writeAttribution(const gtfs::flat::Attribution& t,
                              CsvWriter* csvw) const {
  csvw->writeString(t.attributionId);
  csvw->writeString(t.agencyId);
  csvw->writeString(t.routeId);
  csvw->writeString(t.tripId);
  csvw->writeString(t.organizationName);
  if (t.isProducer == gtfs::flat::Attribution::TYPE::ROLE) {
    csvw->writeInt(t.isProducer);
  } else {
    csvw->skip();
  }
  if (t.isOperator == gtfs::flat::Attribution::TYPE::ROLE) {
    csvw->writeInt(t.isOperator);
  } else {
    csvw->skip();
  }
  if (t.isAuthority == gtfs::flat::Attribution::TYPE::ROLE) {
    csvw->writeInt(t.isAuthority);
  } else {
    csvw->skip();
  }
  csvw->writeString(t.attributionUrl);
  csvw->writeString(t.attributionEmail);
  csvw->writeString(t.attributionPhone);

  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeAttributions(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getAttributionsCsvw(os);
  csvw->flushLine();

  for (const auto& t : f->getAttributions()) {
    writeAttribution(t.getFlat(), csvw.get());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getTranslationsCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(new CsvWriter(
      os, {"table_name", "field_name", "language", "translation", "record_id", "record_sub_id", "field_value"}));
}

// ____________________________________________________________________________
bool Writer::writeTranslation(const gtfs::flat::Translation& t,
                              CsvWriter* csvw) const {
  if (t.table == gtfs::flat::Translation::TABLE::AGENCY) {
    csvw->writeString("agency");
  }
  if (t.table == gtfs::flat::Translation::TABLE::STOPS) {
    csvw->writeString("stops");
  }
  if (t.table == gtfs::flat::Translation::TABLE::ROUTES) {
    csvw->writeString("routes");
  }
  if (t.table == gtfs::flat::Translation::TABLE::TRIPS) {
    csvw->writeString("trips");
  }
  if (t.table == gtfs::flat::Translation::TABLE::STOP_TIMES) {
    csvw->writeString("stop_times");
  }
  if (t.table == gtfs::flat::Translation::TABLE::PATHWAYS) {
    csvw->writeString("pathways");
  }
  if (t.table == gtfs::flat::Translation::TABLE::LEVELS) {
    csvw->writeString("levels");
  }
  if (t.table == gtfs::flat::Translation::TABLE::FEED_INFO) {
    csvw->writeString("feed_info");
  }
  if (t.table == gtfs::flat::Translation::TABLE::ATTRIBUTIONS) {
    csvw->writeString("attributions");
  }

  csvw->writeString(t.fieldName);
  csvw->writeString(t.language);
  csvw->writeString(t.translation);
  csvw->writeString(t.recordId);
  csvw->writeString(t.recordSubId);
  csvw->writeString(t.fieldValue);

  csvw->flushLine();
  return true;
}

// ____________________________________________________________________________
bool Writer::writeTranslations(gtfs::Feed* f, std::ostream* os) const {
  auto csvw = getTranslationsCsvw(os);
  csvw->flushLine();

  for (const auto& t : f->getTranslations()) {
    writeTranslation(t.getFlat(), csvw.get());
  }

  return true;
}

// ____________________________________________________________________________
bool Writer::writePathways(gtfs::Feed* sourceFeed, std::ostream* s) const {
  auto csvw = getPathwayCsvw(s);
  for (const auto& a : sourceFeed->getPathways()) {
    writePathway(a.second->getFlat(), csvw.get());
  }

  return true;
}

// ____________________________________________________________________________
std::unique_ptr<CsvWriter> Writer::getLevelCsvw(std::ostream* os) {
  return std::unique_ptr<CsvWriter>(
      new CsvWriter(os, {"level_id", "level_index", "level_name"}));
}

// ____________________________________________________________________________
bool Writer::writeLevel(const gtfs::flat::Level& l, CsvWriter* csvw) const {
  csvw->writeString(l.id);
  csvw->writeDouble(l.index);
  if (l.name.size())
    csvw->writeString(l.name);
  else
    csvw->skip();
  csvw->flushLine();

  return true;
}

// ____________________________________________________________________________
bool Writer::writeLevels(gtfs::Feed* sourceFeed, std::ostream* s) const {
  auto csvw = getLevelCsvw(s);
  for (const auto& a : sourceFeed->getLevels()) {
    writeLevel(a.second->getFlat(), csvw.get());
  }

  return true;
}

// ___________________________________________________________________________
void Writer::cannotWrite(const std::string& file) {
  throw WriterException("Could not write to file.", file);
}

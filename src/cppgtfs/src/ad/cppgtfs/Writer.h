// Copyright 2018, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_WRITER_H_
#define AD_CPPGTFS_WRITER_H_

#include <stdint.h>

#include <exception>
#include <iostream>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ad/util/CsvWriter.h"
#include "gtfs/Feed.h"
#include "gtfs/flat/Agency.h"
#include "gtfs/flat/Fare.h"
#include "gtfs/flat/Frequency.h"
#include "gtfs/flat/Level.h"
#include "gtfs/flat/Pathway.h"
#include "gtfs/flat/Route.h"
#include "gtfs/flat/Shape.h"
#include "gtfs/flat/StopTime.h"
#include "gtfs/flat/Transfer.h"
#include "gtfs/flat/Attribution.h"
#include "gtfs/flat/Translation.h"
#include "gtfs/flat/Trip.h"

using ad::util::CsvWriter;
using std::string;

// A GTFS writer

namespace ad {
namespace cppgtfs {

class WriterException : public std::exception {
 public:
  WriterException(std::string msg, std::string file_name)
      : _msg(msg), _file_name(file_name) {}
  WriterException(std::string msg) : _msg(msg), _file_name("?") {}
  ~WriterException() throw() {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << _file_name << ":";
    ss << " " << _msg;
    _what_msg = ss.str();
    return _what_msg.c_str();
  }

 private:
  mutable std::string _what_msg;
  std::string _msg;
  std::string _file_name;
};

class Writer {
 public:
  // Default initialization.
  Writer() {}

  // write a GtfsFeed to a zip/folder
  bool write(gtfs::Feed* sourceFeed, const std::string& path) const;

  bool writeAgency(const gtfs::flat::Agency& ag, CsvWriter* csvw,
                   const gtfs::AddFlds& addFlds) const;
  bool writeAgencies(gtfs::Feed* f, std::ostream* os) const;

  bool writeStop(const gtfs::flat::Stop& ag, CsvWriter* csvw,
                 const gtfs::AddFlds& addFlds) const;
  bool writeStops(gtfs::Feed* f, std::ostream* os) const;

  bool writeShapePoint(const gtfs::flat::ShapePoint& ag, CsvWriter* csvw) const;
  bool writeShapes(gtfs::Feed* f, std::ostream* os) const;

  bool writeTrip(const gtfs::flat::Trip& ag, CsvWriter* csvw,
                 const gtfs::AddFlds& addFlds) const;
  bool writeTrips(gtfs::Feed* f, std::ostream* os) const;

  bool writeStopTime(const gtfs::flat::StopTime& ag, CsvWriter* csvw) const;
  bool writeStopTimes(gtfs::Feed* f, std::ostream* os) const;

  bool writeRoute(const gtfs::flat::Route& ag, CsvWriter* csvw,
                  const gtfs::AddFlds& addFlds) const;
  bool writeRoutes(gtfs::Feed* f, std::ostream* os) const;

  bool writeFeedInfo(gtfs::Feed* f, std::ostream* os) const;

  bool writeTransfer(const gtfs::flat::Transfer& ag, CsvWriter* csvw) const;
  bool writeTransfers(gtfs::Feed* f, std::ostream* os) const;

  bool writeCalendar(const gtfs::flat::Calendar& ag, CsvWriter* csvw) const;
  bool writeCalendars(gtfs::Feed* f, std::ostream* os) const;

  bool writeCalendarDate(const gtfs::flat::CalendarDate& ag,
                         CsvWriter* csvw) const;
  bool writeCalendarDates(gtfs::Feed* f, std::ostream* os) const;

  bool writeFrequency(const gtfs::flat::Frequency& ag, CsvWriter* csvw) const;
  bool writeFrequencies(gtfs::Feed* f, std::ostream* os) const;

  bool writeFare(const gtfs::flat::Fare& ag, CsvWriter* csvw) const;
  bool writeFares(gtfs::Feed* f, std::ostream* os) const;

  bool writeFareRule(const gtfs::flat::FareRule& ag, CsvWriter* csvw) const;
  bool writeFareRules(gtfs::Feed* f, std::ostream* os) const;

  bool writeLevel(const gtfs::flat::Level& ag, CsvWriter* csvw) const;
  bool writeLevels(gtfs::Feed* f, std::ostream* os) const;

  bool writePathway(const gtfs::flat::Pathway& ag, CsvWriter* csvw) const;
  bool writePathways(gtfs::Feed* f, std::ostream* os) const;

  bool writeAttribution(const gtfs::flat::Attribution& a,
                        CsvWriter* csvw) const;
  bool writeAttributions(gtfs::Feed*, std::ostream* os) const;

  bool writeTranslation(const gtfs::flat::Translation& a,
                        CsvWriter* csvw) const;
  bool writeTranslations(gtfs::Feed*, std::ostream* os) const;

  static void cannotWrite(const std::string& file);

  static std::unique_ptr<CsvWriter> getAgencyCsvw(std::ostream* os,
                                                  const gtfs::AddFlds& addFlds);
  static std::unique_ptr<CsvWriter> getStopsCsvw(std::ostream* os,
                                                 const gtfs::AddFlds& addFlds);
  static std::unique_ptr<CsvWriter> getRoutesCsvw(std::ostream* os,
                                                  const gtfs::AddFlds& addFlds);
  static std::unique_ptr<CsvWriter> getCalendarCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getCalendarDatesCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getFrequencyCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getTransfersCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getAttributionsCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getTranslationsCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getFaresCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getFareRulesCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getShapesCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getTripsCsvw(std::ostream* os,
                                                 const gtfs::AddFlds& addFlds);
  static std::unique_ptr<CsvWriter> getStopTimesCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getFeedInfoCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getLevelCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getPathwayCsvw(std::ostream* os);
  static std::unique_ptr<CsvWriter> getAttributionCsvw(std::ostream* os);
};
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_WRITER_H_

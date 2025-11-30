// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FLAT_TRANSLATION_H_
#define AD_CPPGTFS_GTFS_FLAT_TRANSLATION_H_

#include <stdint.h>

#include <string>

namespace ad {
namespace cppgtfs {
namespace gtfs {
namespace flat {

struct TranslationFlds {
  size_t tableFld;
  size_t fieldNameFld;
  size_t translationFld;
  size_t recordIdFld;
  size_t recordSubIdFld;
  size_t fieldValueFld;
  size_t languageFld;
};

struct Translation {
  enum TABLE : uint8_t {
    AGENCY = 0,
    STOPS = 1,
    ROUTES = 2,
    TRIPS = 3,
    STOP_TIMES = 4,
    PATHWAYS = 5,
    LEVELS = 6,
    FEED_INFO = 7,
    ATTRIBUTIONS = 8
  };

  TABLE table;
  std::string fieldName;
  std::string language;
  std::string translation;
  std::string recordId;
  std::string recordSubId;
  std::string fieldValue;
};

}  // namespace flat
}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_FLAT_TRANSLATION_H_

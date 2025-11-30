// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_TRANSLATION_H_
#define AD_CPPGTFS_GTFS_TRANSLATION_H_

#include <stdint.h>

#include <string>

#include "flat/Translation.h"

using std::exception;
using std::string;

namespace ad {
namespace cppgtfs {
namespace gtfs {
class Translation {
 public:
  typedef flat::Translation::TABLE TABLE;
  Translation() {}

  Translation(TABLE table, const std::string& fieldName,
              const std::string& language, const std::string& translation,
              const std::string& recordId, const std::string& recordSubId,
              const std::string& fieldValue)
      : _table(table),
        _fieldName(fieldName),
        _language(language),
        _translation(translation),
        _recordId(recordId),
        _recordSubId(recordSubId),
        _fieldValue(fieldValue) {}

  TABLE getTable() const { return _table; }
  const std::string& getFieldName() const { return _fieldName; }
  const std::string& getLanguage() const { return _language; }
  const std::string& getTranslation() const { return _translation; }
  const std::string& getRecordId() const { return _recordId; }
  const std::string& getRecordSubId() const { return _recordSubId; }
  const std::string& getFieldValue() const { return _fieldValue; }

  flat::Translation getFlat() const {
    return flat::Translation{_table,       _fieldName, _language,
                             _translation, _recordId, _recordSubId,
                             _fieldValue};
  }

 private:
  TABLE _table;
  std::string _fieldName;
  std::string _language;
  std::string _translation;
  std::string _recordId;
  std::string _recordSubId;
  std::string _fieldValue;
};

}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_TRANSLATION_H_

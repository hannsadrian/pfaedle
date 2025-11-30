// Copyright 2014, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Hannah Bast <bast@informatik.uni-freiburg.de>,
//          Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_UTIL_ZIPCSVPARSER_H_
#define AD_UTIL_ZIPCSVPARSER_H_

#ifdef LIBZIP_FOUND
#include <stdint.h>
#include <zip.h>

#include <exception>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "CsvParser.h"

using std::exception;
using std::string;

/**
 * A parser for CSV files in ZIPs, as defined at
 * http://tools.ietf.org/html/rfc4180#page-2
 */
namespace ad {
namespace util {

class ZipCsvParser : public CsvParser {
 public:
  // Initializes the parser by opening the file and reading the table header.
  explicit ZipCsvParser(zip_file_t* zf);

  // Initializes the parser by opening a file in a ZIP
  ZipCsvParser(zip* za, const std::string& filename);

  // Initializes the parser by opening a file in a ZIP, with a readable name
  ZipCsvParser(zip* za, const std::string& filename,
               const std::string& readableName);

  virtual ~ZipCsvParser();

  virtual std::pair<size_t, size_t> fetchLine();
  virtual bool isGood() const;
  virtual const string getReadablePath() const { return _readablePath; }

 private:
  // The handle to the file.
  zip_file_t* _zf;
  std::string _readablePath;

  int64_t _lastLine = -1;
  int64_t _bufSize = 0;

  char _tmpBuff[BUFFER_S] = {0};
};
}  // namespace util
}  // namespace ad
#endif

#endif  // AD_UTIL_ZIPCSVPARSER_H_

// Copyright 2018, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_UTIL_CSVWRITER_H_
#define AD_UTIL_CSVWRITER_H_

#include <stdint.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * A writer for CSV files, as defined at
 * http://tools.ietf.org/html/rfc4180#page-2
 *
 * This parser is an extension of the original AD CSV parser. It can handle
 * quoted strings and quote escapes as well as BOMs.
 */
namespace ad {
namespace util {

typedef std::vector<std::string> HeaderList;

class CsvWriter {
 public:
  // Initializes the parser by opening the file
  CsvWriter(std::ostream* str, const HeaderList& headers);

  // Initializes the parser by opening the file and reading the table header.
  CsvWriter(const std::string& path, const HeaderList& headers);

  virtual ~CsvWriter() = default;

  void writeDouble(double d);
  void writeDouble(double d, size_t digits);
  void writeString(const std::string& str);
  void writeInt(int i);
  void skip();

  void flushLine();

 protected:
  virtual void write(const char* s, size_t n);
  virtual void put(char c);

  HeaderList _headers;
  bool _hWritten;
  bool _first;
  char _delim;

  char _dblBuf[25];

  void writeRawString(const std::string& str);
  void writeStrArr(const std::vector<std::string>& arr);
  void writeHeader();

  int pow10(int i) const;

  std::string escStr(const std::string& str) const;

  std::ostream* _stream;
  std::ofstream _ofstream;
};
}  // namespace util
}  // namespace ad

#endif  // AD_UTIL_CSVWRITER_H_

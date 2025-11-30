// Copyright 2014, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Hannah Bast <bast@informatik.uni-freiburg.de>,
//          Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_UTIL_CSVPARSER_H_
#define AD_UTIL_CSVPARSER_H_

#include <stdint.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::exception;
using std::string;

/**
 * A parser for CSV files, as defined at
 * http://tools.ietf.org/html/rfc4180#page-2
 *
 * This parser is an extension of the original AD CSV parser. It can handle
 * quoted strings and quote escapes as well as BOMs.
 */
namespace ad {
namespace util {

static const size_t BUFFER_S = 50000;

class CsvParserException : public exception {
 public:
  CsvParserException(std::string msg, int index, std::string fieldName,
                     uint64_t line, std::string fileName)
      : _msg(msg),
        _index(index),
        _fieldName(fieldName),
        _line(line),
        _fileName(fileName) {}
  ~CsvParserException() throw() {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << _msg;
    if (_index > -1)
      ss << " for field #" << (_index + 1) << " (" << _fieldName << ")   ";
    _what_msg = ss.str();
    return _what_msg.c_str();
  }

  virtual uint64_t getLine() const { return _line; }

  const std::string& getMsg() const { return _msg; }
  const std::string& getFieldName() const { return _fieldName; }
  const std::string& getFileName() const { return _fileName; }

 private:
  mutable std::string _what_msg;
  std::string _msg;
  int _index;
  std::string _fieldName;
  uint64_t _line;
  std::string _fileName;
};

class CsvParser {
 public:
  // Default initialization.
  CsvParser();

  virtual ~CsvParser();

  // Initializes the parser by opening the file and reading the table header.
  explicit CsvParser(std::istream* stream);

  // Initializes the parser by opening the file and reading the table header,
  // with a readable file path displayed on errors
  CsvParser(std::istream* stream, const std::string& path);

  // Initializes the parser by opening the file and reading the table header.
  explicit CsvParser(const std::string& path);

  // Read next line.
  // Returns true iff the line was read successfully.
  bool readNextLine();

  virtual std::pair<size_t, size_t> fetchLine();

  // Getters for i-th column from current line. Prerequisite: i < _numColumns.
  // Second arguments are default values.

  // returns the i-th column as a trimmed string
  const char* getTString(const size_t i) const;

  // returns the i-th column as a double
  double getDouble(const size_t i) const;

  // returns the i-th columns as a 32bit integer
  int32_t getLong(const size_t i) const;

  // returns the column with given field name.
  // these methods behave exactly the same as the ones above, except that
  // columns are accessed by their identifier.
  const char* getTString(const std::string& fieldName) const;

  double getDouble(const std::string& fieldName) const;

  int32_t getLong(const std::string& fieldName) const;

  // returns the line number the parser is currently at
  int32_t getCurLine() const;

  // checks whether a column with a specific name exists in this file
  bool hasItem(const std::string& fieldName) const;

  // checks whether the field is empty in the current line
  bool fieldIsEmpty(const std::string& fieldName) const;
  bool fieldIsEmpty(size_t field) const;

  // Get the number of columns. Will be zero before openFile has been called.
  size_t getNumColumns() const;

  // Get the number of headers
  size_t getNumHeaders() const { return _headerVec.size(); }

  // returns the index number of a field name
  size_t getFieldIndex(const string& fieldName) const;

  size_t getOptFieldIndex(const string& fieldName) const;

  const string getFieldName(size_t i) const;

  virtual const string getReadablePath() const { return _readablePath; }

  virtual bool isGood() const;

 protected:
  int32_t _curLine = 0;

  // Parses the header row and fills the header map.
  void parseHeader();

  // returns a trimmed version of a const char*
  //
  // careful: this function is not idempotent. it will leave t
  // right-trimmed.
  const char* inlineRightTrim(const char* t) const;

  // Map of field names to column indices. Parsed from the
  // table header (first row in a CSV file).
  std::unordered_map<std::string, size_t> _headerMap;
  std::vector<std::string> _headerVec;

  // Pointers to the items in the current line.
  std::vector<const char*> _currentItems;

  // modified, quote-escaped strings
  std::vector<std::string> _currentModItems;

  char _buff[BUFFER_S] = {0};

  static double atof(const char* p, uint8_t mn, bool* fail);
  static uint32_t atoi(const char* p, bool* fail);

 private:
  // The handle to the file.
  std::istream* _stream;

  // Used internally if no external stream is provided
  std::ifstream _ifstream;

  std::string _readablePath;
};
}  // namespace util
}  // namespace ad

#endif  // AD_UTIL_CSVPARSER_H_

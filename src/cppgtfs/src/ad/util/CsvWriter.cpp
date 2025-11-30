// Copyright 2018, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include <algorithm>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

#include "CsvWriter.h"
#include "dtoa_milo.h"

using ad::util::CsvWriter;

// cached first 10 powers of 10
static int pow10cache[10] = {1,      10,      100,      1000,      10000,
                             100000, 1000000, 10000000, 100000000, 1000000000};

// _____________________________________________________________________________
CsvWriter::CsvWriter(std::ostream* str, const HeaderList& headers)
    : _headers(headers),
      _hWritten(false),
      _first(true),
      _delim(','),
      _stream(str) {}

// _____________________________________________________________________________
CsvWriter::CsvWriter(const std::string& fileName, const HeaderList& headers)
    : _headers(headers), _hWritten(false), _first(true), _delim(',') {
  _ofstream.open(fileName);
  _stream = &_ofstream;
}

// _____________________________________________________________________________
int CsvWriter::pow10(int i) const {
  if (i < 10) return pow10cache[i];
  return pow(10, i);
}

// _____________________________________________________________________________
void CsvWriter::writeDouble(double d, size_t digits) {
  double p = pow10(digits);
  d = round(d * p) / p;
  writeDouble(d);
}

// _____________________________________________________________________________
void CsvWriter::writeDouble(double d) {
  ad::util::dtoa_milo(d, _dblBuf);
  if (!_first) put(_delim);
  _first = false;
  size_t i = 0;
  while (_dblBuf[i] != 0) {
    put(_dblBuf[i]);
    i++;
  }
}

// _____________________________________________________________________________
void CsvWriter::skip() {
  if (!_first) write(&_delim, 1);
  _first = false;
}

// _____________________________________________________________________________
void CsvWriter::writeString(const std::string& str) {
  if (!_first) write(&_delim, 1);
  _first = false;

  if (str.find(_delim) != std::string::npos) {
    if (str.find('"') != std::string::npos) {
      const auto& estr = escStr(str);
      put('\"');
      write(estr.c_str(), estr.size());
      put('\"');
    } else {
      put('\"');
      write(str.c_str(), str.size());
      put('\"');
    }
  } else {
    write(str.c_str(), str.size());
  }
}

// _____________________________________________________________________________
std::string CsvWriter::escStr(const std::string& str) const {
  std::stringstream ret;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == '\"')
      ret << "\"\"";
    else
      ret << str[i];
  }

  return ret.str();
}

// _____________________________________________________________________________
void CsvWriter::writeRawString(const std::string& str) {
  if (!_first) write(&_delim, 1);
  _first = false;
  write(str.c_str(), str.size());
}

// _____________________________________________________________________________
void CsvWriter::writeInt(int i) {
  if (!_first) write(&_delim, 1);
  _first = false;
  if (i >= 0 && i <= 9) {
    put(48 + i);
  } else {
    const auto& str = std::to_string(i);
    write(str.c_str(), str.size());
  }
}

// _____________________________________________________________________________
void CsvWriter::flushLine() {
  if (!_hWritten) writeHeader();
  put('\n');
  _first = true;
}

// _____________________________________________________________________________
void CsvWriter::writeHeader() {
  _hWritten = true;
  writeStrArr(_headers);
}

// _____________________________________________________________________________
void CsvWriter::writeStrArr(const std::vector<std::string>& arr) {
  if (!arr.size()) return;
  bool first = false;
  for (const auto& str : arr) {
    if (first) put(_delim);
    first = true;
    write(str.c_str(), str.size());
  }
}

// _____________________________________________________________________________
void CsvWriter::write(const char* s, size_t n) { _stream->write(s, n); }

// _____________________________________________________________________________
void CsvWriter::put(char c) { _stream->put(c); }

// Copyright 2013, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Hannah Bast <bast@informatik.uni-freiburg.de>,
//          Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifdef LIBZIP_FOUND
#include <zip.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <string>

#include "ZipCsvParser.h"

using ad::util::ZipCsvParser;

// _____________________________________________________________________________
ZipCsvParser::ZipCsvParser(zip_file_t* zf) : _zf(zf), _readablePath("?") {
  readNextLine();
  parseHeader();
}

// _____________________________________________________________________________
ZipCsvParser::~ZipCsvParser() {
  if (isGood()) zip_fclose(_zf);
}

// _____________________________________________________________________________
ZipCsvParser::ZipCsvParser(zip* za, const std::string& filename)
    : ZipCsvParser(za, filename, filename) {}

// _____________________________________________________________________________
ZipCsvParser::ZipCsvParser(zip* za, const std::string& filename,
                           const std::string& readablePath)
    : _zf(0), _readablePath(readablePath) {
  // locate file in zip
  auto fi = zip_name_locate(za, filename.c_str(), ZIP_FL_NOCASE | ZIP_FL_NODIR);

  if (fi < 0) {
    // file not found
    return;
  }

  auto zf = zip_fopen_index(za, fi, 0);

  if (zf == 0) {
    return;
  }

  // everything was successful
  _zf = zf;

  readNextLine();
  parseHeader();
}

// _____________________________________________________________________________
bool ZipCsvParser::isGood() const { return _zf != 0; }

// _____________________________________________________________________________
std::pair<size_t, size_t> ZipCsvParser::fetchLine() {
  if (_zf == 0) return {0, 0};

  auto lineEnd =
      memchr(_buff + (_lastLine + 1), '\n', _bufSize - (_lastLine + 1));

  if (!lineEnd) {
    // copy end of buffer to tmp buffer
    memcpy(_tmpBuff, _buff + (_lastLine + 1), _bufSize - (_lastLine + 1));

    // copy tmp buffer to original buffer
    memcpy(_buff, _tmpBuff, _bufSize - (_lastLine + 1));

    // only read BUFFER_S - 1 bytes to always have space for the 0 byte
    auto bytesRead = zip_fread(_zf, _buff + (_bufSize - (_lastLine + 1)),
                               BUFFER_S - 1 - (_bufSize - (_lastLine + 1)));

    if (bytesRead < 0) {
      return {0, 0};
    }

    _bufSize = (_bufSize - (_lastLine + 1)) + bytesRead;
    _lastLine = -1;

    if (bytesRead == 0) {
      // arrived at end of file
      _buff[_bufSize] = 0;
      _lastLine = _bufSize - 1;
      return {0, _bufSize};
    }

    return fetchLine();
  }

  auto ret = std::pair<size_t, size_t>(_lastLine + 1, ((char*)lineEnd - _buff));
  _buff[ret.second] = 0;

  _lastLine = ret.second;

  return ret;
}
#endif

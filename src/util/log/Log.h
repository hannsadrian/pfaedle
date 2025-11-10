// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_LOG_LOG_H_
#define UTIL_LOG_LOG_H_

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifndef UTIL_LOGLVL
#define UTIL_LOGLVL 2
#endif

// compiler will optimize statement away if x > UTIL_LOGLVL
#ifndef LOG
#define LOG(x) if (x <= UTIL_LOGLVL) util::Log<x>().log()
#define LOGTO(x, os) if (x <= UTIL_LOGLVL) util::Log<x>(&os).log()
#else
#define PB_LOG(x) if (x <= UTIL_LOGLVL) util::Log<x>().log()
#define PB_LOGTO(x, os) if (x <= UTIL_LOGLVL) util::Log<x>(&os).log()
#endif

using std::setfill;
using std::setw;
using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::time_point_cast;

namespace util {

enum LogLevel {
  VDEBUG = 4,
  DEBUG = 3,
  INFO = 2,
  WARN = 1,
  ERROR = 0
};

const static char* LOGS[] = {"ERROR", "WARN ", "INFO ", "DEBUG", "DEBUG"};

template <char LVL>
class Log {
 public:
  Log() { if (LVL < LogLevel::INFO) os = &std::cerr; else os = &std::cout; }
  explicit Log(std::ostream* s) { os = s; }
  ~Log() { buf << std::endl; (*os) << buf.str(); }
  std::ostream& log() { return ts() << LOGS[(size_t)LVL] << ": "; }

 private:
  std::ostream* os;
  std::ostringstream buf;
  std::ostream& ts() {
    char tl[20];
    auto n = system_clock::now();
    time_t tt = system_clock::to_time_t(n);
    int m = duration_cast<milliseconds>(n-time_point_cast<seconds>(n)).count();
    struct tm t = *localtime(&tt);
    strftime(tl, 20, "%Y-%m-%d %H:%M:%S", &t);
    return buf << "[" << tl << "." << setfill('0') << setw(3) << m << "] ";
  }
};
}

#endif  // UTIL_LOG_LOG_H_

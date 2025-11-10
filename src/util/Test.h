// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_TEST_H_
#define UTIL_TEST_H_

#define _TEST3(s, o, e) {auto __ss = s; if (!(__ss o e)) {  std::cerr << "\n" << __FILE__ << ":" << __LINE__ << ": Test failed!\n  Expected " << #s << " " << #o " " << (e) << ", got " << (__ss) << std::endl;  exit(1);}}
#define _TEST2(s, e) _TEST3(s, ==, o)
#define _TEST1(s) _TEST3(static_cast<bool>(s), ==, true)

#define _GET_TEST_MACRO(_1,_2,_3,NAME,...) NAME
#define TEST(...) _GET_TEST_MACRO(__VA_ARGS__, _TEST3, _TEST2, _TEST1, UNUSED)(__VA_ARGS__)

#endif  // UTIL_TEST_H_

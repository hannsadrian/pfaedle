// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_GEO_LINE_H_
#define UTIL_GEO_LINE_H_

#include <vector>
#include <algorithm>
#include <cstdint>
#include <math.h>

#include "./Point.h"

namespace util {
namespace geo {

template <typename T>
class Line : public std::vector<Point<T>> {
  using std::vector<Point<T>>::vector;
};

template <typename T>
using LineSegment = std::pair<Point<T>, Point<T>>;

template <typename T>
struct AngledLineSegment {
  LineSegment<T> seg;
  int16_t prevAng;
  bool firstIsBoundary;
  int16_t nextAng;
  bool secondIsBoundary;
};

// _____________________________________________________________________________
template <typename T>
inline bool sameOrig(const LineSegment<T>& a, const LineSegment<T>& b) {
  if (a.first.getX() <= a.second.getX() && b.first.getX() <= b.second.getX())
    return a.first == b.first;
  if (a.first.getX() <= a.second.getX() && b.first.getX() >= b.second.getX())
    return a.first == b.second;
  if (a.first.getX() >= a.second.getX() && b.first.getX() <= b.second.getX())
    return a.second == b.first;
  if (a.first.getX() >= a.second.getX() && b.first.getX() >= b.second.getX())
    return a.second == b.second;

  return false;
}

// _____________________________________________________________________________
inline double angBetween(double p1x, double p1y, double q1x, double q1y) {
  double dY = q1y - p1y;
  double dX = q1x - p1x;
  return atan2(dY, dX);
}

// _____________________________________________________________________________
template <typename T>
inline double angBetween(const Point<T>& p1, const Point<T>& q1) {
  return angBetween(p1.getX(), p1.getY(), q1.getX(), q1.getY());
}

// _____________________________________________________________________________
template <typename T>
inline double angBetween(const Point<T>& o, const Point<T>& p1,
                         const Point<T>& p2) {
  double dy1 = p1.getY() - o.getY();
  double dy2 = p2.getY() - o.getY();
  double dx1 = p1.getX() - o.getX();
  double dx2 = p2.getX() - o.getX();

  return atan2(dy1 * dx2 - dy2 * dx1, dx1 * dx2 + dy1 * dy2);
}

template <typename T>
struct XSortedTuple {
  // a tuple used in X-sorted linestrings and polygons.
  // we do not store the corresponding line segment explicitly, but only the
  // other point, with "vals" holding which of the two points comes first on
  // bit 1, and whether this element is an out event on bit 0. Bit 2 is 1 if
  // the line is completely empty.
  // bit 3 holds whether the line segment was reversed due to the x order
  // bit 4 holds whether the first segment point was part of the boundary
  // bit 5 holds whether the second segment point was part of the boundary
  // for 64bit points, we thus safe 16 bytes (~29%), which let's more
  // XSortedTuples fit into the CPU caches during tests for intersect / contains
  // (in particular the binary search during startup), and also makes for faster
  // reading from disk
  Point<T> p;
  Point<T> other;
  int16_t outAngle;
  int16_t inAngle;
  uint8_t vals;

  XSortedTuple() {}
  XSortedTuple(const Point<T>& p, bool out) : p(p), vals(0) {
    if (out) vals = 1;
    vals += 4;
  }
  XSortedTuple(const Point<T>& p, const LineSegment<T>& seg, bool rev,
               double prevAngle, double nextAngle, bool out)
      : XSortedTuple(p, seg, rev, prevAngle, nextAngle, false, false, out) {}
  XSortedTuple(const Point<T>& p, const LineSegment<T>& seg, bool rev,
               double prevAngle, double nextAngle, bool firstIsBoundary,
               bool secondIsBoundary, bool out)
      : p(p), vals(0) {
    if (out) vals = 1;
    if (rev) vals += 8;
    if (seg.first == p) {
      vals += 2;
      other = seg.second;
    } else if (seg.second == p) {
      other = seg.first;
    }
    outAngle = (nextAngle / M_PI) * 32766;
    inAngle = (prevAngle / M_PI) * 32766;

    if (nextAngle > M_PI) outAngle = 32767;
    if (prevAngle > M_PI) inAngle = 32767;

    if (firstIsBoundary) vals += 16;
    if (secondIsBoundary) vals += 32;
  }

  double nextAng() const { return ((1.0 * outAngle) / 32766.0) * M_PI; }
  double prevAng() const { return ((1.0 * inAngle) / 32766.0) * M_PI; }

  int16_t rawNextAng() const { return outAngle; }
  int16_t rawPrevAng() const { return inAngle; }

  bool firstIsBoundary() const { return vals & 16; }
  bool secondIsBoundary() const { return vals & 32; }

  LineSegment<T> seg() const {
    if (vals & 4) return {{0, 0}, {0, 0}};
    if (vals & 2) return {p, other};
    return {other, p};
  }

  LineSegment<T> origSeg() const {
    if (vals & 4) {
      return {{0, 0}, {0, 0}};
    }
    if (vals & 2) {
      if (vals & 8) return {other, p};
      return {p, other};
    }

    if (vals & 8) return {p, other};
    return {other, p};
  }

  AngledLineSegment<T> origSegAng() const {
    if (vals & 4) {
      return {{{0, 0}, {0, 0}}, rawPrevAng(), firstIsBoundary(), rawNextAng(), secondIsBoundary()};
    }
    if (vals & 2) {
      if (vals & 8) return {{other, p}, rawPrevAng(), firstIsBoundary(), rawNextAng(), secondIsBoundary()};
      return {{p, other}, rawPrevAng(), firstIsBoundary(), rawNextAng(), secondIsBoundary()};
    }

    if (vals & 8) return {{p, other}, rawPrevAng(), firstIsBoundary(), rawNextAng(), secondIsBoundary()};
    return {{other, p}, rawPrevAng(), firstIsBoundary(), rawNextAng(), secondIsBoundary()};
  }

  bool out() const { return vals & 1; }
};

template <typename T>
bool operator==(const XSortedTuple<T>& a, const XSortedTuple<T>& b) {
  return a.p == b.p && a.other == b.other && a.outAngle == b.outAngle &&
         a.inAngle == b.inAngle && a.vals == b.vals;
}

template <typename T>
bool operator!=(const XSortedTuple<T>& a, const XSortedTuple<T>& b) {
  return !(a == b);
}

template <typename T>
bool operator<(const XSortedTuple<T>& a, const XSortedTuple<T>& b) {
  if (a.p.getX() < b.p.getX()) return true;
  if (a.p.getX() == b.p.getX()) return {!a.out() && b.out()};

  return false;
}

template <typename T>
bool operator<(const LineSegment<T>& a, const LineSegment<T>& b) {
  auto af = a.first;
  auto as = a.second;

  auto bf = b.first;
  auto bs = b.second;

  if (as.getX() < af.getX()) std::swap(af, as);
  if (bs.getX() < bf.getX()) std::swap(bf, bs);

  // this should be implicitely catched by the comparisons below,
  // however, because of floating point imprecisions, we might decide
  // < for equivalent segments. This is a problem, as segments are only
  // identified via they coordinates, not by some ID, in the active sets
  if (af.getX() == bf.getX() && af.getY() == bf.getY() &&
      as.getX() == bs.getX() && as.getY() == bs.getY()) {
    return false;
  }

  // special case: we touch a the end / beginning, always classify the same to
  // avoid transitivity problems for this operator
  if (as == bf && as.getX() != af.getX() && bs.getX() != bf.getX())
    return false;
  if (bs == af && as.getX() != af.getX() && bs.getX() != bf.getX()) return true;

  if (af.getX() < bf.getX() || bf.getX() == bs.getX()) {
    // a was first in active set
    if (af.getX() != as.getX()) {
      // check whether first point of b is right of or left of a
      double d =
          -((1.0 * bf.getX() - af.getX()) * (1.0 * as.getY() - af.getY()) -
            (1.0 * bf.getY() - af.getY()) * (1.0 * as.getX() - af.getX()));
      if (d < 0) return false;
      if (d > 0) return true;

      // if we arrive here, first point of b was EXACTLY on a, we have to decide
      // based on the second point of b
      d = -((1.0 * bs.getX() - af.getX()) * (1.0 * as.getY() - af.getY()) -
            (1.0 * bs.getY() - af.getY()) * (1.0 * as.getX() - af.getX()));
      if (d < 0) return false;
      if (d > 0) return true;
    }
  } else {
    // b was first in active set
    if (bf.getX() != bs.getX()) {
      // check whether first point of a is right of or left of b
      double d =
          ((1.0 * af.getX() - bf.getX()) * (1.0 * bs.getY() - bf.getY()) -
           (1.0 * af.getY() - bf.getY()) * (1.0 * bs.getX() - bf.getX()));
      if (d < 0) return false;
      if (d > 0) return true;

      // if we arrive here, first point of a was EXACTLY on b, we have to decide
      // based on the second point of a
      d = ((1.0 * as.getX() - bf.getX()) * (1.0 * bs.getY() - bf.getY()) -
           (1.0 * as.getY() - bf.getY()) * (1.0 * bs.getX() - bf.getX()));
      if (d < 0) return false;
      if (d > 0) return true;
    }
  }

  // a and b are colinear
  return af.getY() < bf.getY() ||
         (af.getY() == bf.getY() && af.getX() < bf.getX()) ||
         (af.getY() == bf.getY() && af.getX() == bf.getX() &&
          as.getY() < bs.getY()) ||
         (af.getY() == bf.getY() && af.getX() == bf.getX() &&
          as.getY() == bs.getY() && as.getX() < bs.getX());
}

template <typename T>
bool operator>(const LineSegment<T>& a, const LineSegment<T>& b) {
  return b < a;
}

template <typename T>
bool operator<(const AngledLineSegment<T>& a, const AngledLineSegment<T>& b) {
  return a.seg < b.seg;
}

template <typename T>
bool operator>(const AngledLineSegment<T>& a, const AngledLineSegment<T>& b) {
  return b.seg < a.seg;
}

template <typename T>
bool sameLeftX(const LineSegment<T>& a, const LineSegment<T>& b) {
  if (a.first.getX() < a.second.getX()) {
    if (b.first.getX() < b.second.getX()) {
      return a.first.getX() == b.first.getX();
    } else {
      return a.first.getX() == b.second.getX();
    }
  } else {
    if (b.first.getX() < b.second.getX()) {
      return a.second.getX() == b.first.getX();
    } else {
      return a.second.getX() == b.second.getX();
    }
  }
}

template <typename T>
class XSortedLine {
 public:
  XSortedLine() {}

  XSortedLine(const Line<T>& line) {
    if (line.size() < 2) return;
    _first = line.front();
    _last = line.back();

    _line.reserve(2 * line.size());
    for (size_t i = 1; i < line.size(); i++) {
      if (line[i - 1].getX() == line[i].getX() &&
          line[i - 1].getY() == line[i].getY())
        continue;
      T len = fabs(line[i - 1].getX() - line[i].getX());
      if (len > _maxSegLen) _maxSegLen = len;

      double prevAng = 0;
      double nextAng = 0;

      size_t prev;

      if (i > 1)
        prev = i - 2;
      else
        prev = line.size() - 1;

      while (line[prev].getX() == line[i - 1].getX() &&
             line[prev].getY() == line[i - 1].getY() && prev != i - 1) {
        if (prev > 0)
          prev = prev - 1;
        else
          prev = line.size() - 1;
      }

      prevAng = util::geo::angBetween(line[i - 1], line[prev]);

      size_t next = (i + 1) % line.size();

      while (line[next].getX() == line[i].getX() &&
             line[next].getY() == line[i].getY() && next != i) {
        next = (next + 1) % line.size();
      }

      nextAng = util::geo::angBetween(line[i], line[next]);

      if (line.front() != line.back()) {
        if (i == 1) prevAng = 2 * M_PI;
        if (i == line.size() - 1) nextAng = 2 * M_PI;
      }

      if (line[i - 1].getX() < line[i].getX()) {
        _line.push_back({line[i - 1],
                         {line[i - 1], line[i]},
                         false,
                         prevAng,
                         nextAng,
                         i == 1,
                         i == line.size() - 1,
                         false});
        _line.push_back({line[i],
                         {line[i - 1], line[i]},
                         false,
                         prevAng,
                         nextAng,
                         i == 1,
                         i == line.size() - 1,
                         true});
      } else {
        _line.push_back({line[i],
                         {line[i], line[i - 1]},
                         true,
                         prevAng,
                         nextAng,
                         i == 1,
                         i == line.size() - 1,
                         false});
        _line.push_back({line[i - 1],
                         {line[i], line[i - 1]},
                         true,
                         prevAng,
                         nextAng,
                         i == 1,
                         i == line.size() - 1,
                         true});
      }
    }

    std::sort(_line.begin(), _line.end());
  }

  XSortedLine(const LineSegment<T>& line) {
    _line.resize(2);
    if (line.first.getX() < line.second.getX()) {
      _line[0] = {line.first, {line.first, line.second},
                  false,      2 * M_PI,
                  2 * M_PI,   true,
                  true,       false};
      _line[1] = {line.second, {line.first, line.second},
                  false,       2 * M_PI,
                  2 * M_PI,    true,
                  true,        true};
    } else {
      _line[0] = {line.second, {line.second, line.first},
                  true,        2 * M_PI,
                  2 * M_PI,    true,
                  true,        false};
      _line[1] = {line.first, {line.second, line.first},
                  true,       2 * M_PI,
                  2 * M_PI,   true,
                  true,       true};
    }
    _maxSegLen = fabs(line.first.getX() - line.second.getX());
    _first = line.first;
    _last = line.second;
  }

  bool operator==(const XSortedLine<T>& other) const {
    return _maxSegLen == other._maxSegLen && _line == other._line &&
           _first == other._first && _last == other._last;
  }

  size_t size() const { return _line.size(); }

  bool empty() const { return _line.size() == 0; }

  T getMaxSegLen() const { return _maxSegLen; }
  void setMaxSegLen(T l) { _maxSegLen = l; }

  Point<T> firstPoint() const { return _first; }
  Point<T> lastPoint() const { return _last; }

  void setFirstPoint(const Point<T>& p) { _first = p; }
  void setLastPoint(const Point<T>& p) { _last = p; }

  const std::vector<XSortedTuple<T>>& rawLine() const { return _line; }
  std::vector<XSortedTuple<T>>& rawLine() { return _line; }

 private:
  std::vector<XSortedTuple<T>> _line;
  Point<T> _first;
  Point<T> _last;
  T _maxSegLen = -1;
};

template <typename T>
using MultiLine = std::vector<Line<T>>;

}  // namespace geo
}  // namespace util

#endif  // UTIL_GEO_LINE_H_

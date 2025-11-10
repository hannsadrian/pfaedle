// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_GEO_COLLECTION_H_
#define UTIL_GEO_COLLECTION_H_

#include <vector>

#include "./Box.h"
#include "./Line.h"
#include "./Point.h"
#include "./Polygon.h"

namespace util {
namespace geo {

template <typename T>
class AnyGeometry;

template <typename T>
using Collection = std::vector<AnyGeometry<T>>;

template <typename T>
class AnyGeometry {
 public:
  AnyGeometry(const Point<T>& p) : _point(p), _type(0){};
  AnyGeometry(const Line<T>& l) : _line(l), _type(1){};
  AnyGeometry(const Polygon<T>& p) : _polygon(p), _type(2){};
  AnyGeometry(const MultiLine<T>& ml) : _multiline(ml), _type(3){};
  AnyGeometry(const MultiPolygon<T>& mp) : _multipolygon(mp), _type(4){};
  AnyGeometry(const Collection<T>& c) : _collection(c), _type(5){};
  AnyGeometry(const MultiPoint<T>& p) : _multipoint(p), _type(6){};

  const Point<T>& getPoint() const { return _point; }
  const Line<T>& getLine() const { return _line; }
  const Polygon<T>& getPolygon() const { return _polygon; }
  const MultiLine<T>& getMultiLine() const { return _multiline; }
  const MultiPolygon<T>& getMultiPolygon() const { return _multipolygon; }
  const MultiPoint<T>& getMultiPoint() const { return _multipoint; }
  const Collection<T>& getCollection() const { return _collection; }

  uint8_t getType() const { return _type; }

 private:
  Point<T> _point;
  Line<T> _line;
  Polygon<T> _polygon;
  MultiLine<T> _multiline;
  MultiPolygon<T> _multipolygon;
  Collection<T> _collection;
  MultiPoint<T> _multipoint;
  uint8_t _type;
};

template <typename T>
inline bool operator==(const AnyGeometry<T>& a, const AnyGeometry<T>& b) {
  if (a.getType() != b.getType()) return false;

  if (a.getType() == 0) return a.getPoint() == b.getPoint();
  if (a.getType() == 1) return a.getLine() == b.getLine();
  if (a.getType() == 2) return a.getPolygon() == b.getPolygon();
  if (a.getType() == 3) return a.getMultiLine() == b.getMultiLine();
  if (a.getType() == 4) return a.getMultiPolygon() == b.getMultiPolygon();
  if (a.getType() == 5) return a.getCollection() == b.getCollection();
  if (a.getType() == 6) return a.getMultiPoint() == b.getMultiPoint();

  return false;
}

}  // namespace geo
}  // namespace util

#endif  // UTIL_GEO_LINE_H_

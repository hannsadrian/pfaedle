// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_GEO_OUTPUT_GEOJSONOUTPUT_H_
#define UTIL_GEO_OUTPUT_GEOJSONOUTPUT_H_

#include <map>
#include <ostream>
#include <string>
#include "util/String.h"
#include "util/geo/Geo.h"
#include "util/json/Writer.h"

namespace util {
namespace geo {
namespace output {

class GeoJsonOutput {
 public:
  GeoJsonOutput(std::ostream& str);
  GeoJsonOutput(std::ostream& str, bool raw);
  GeoJsonOutput(std::ostream& str, json::Val attrs);
  ~GeoJsonOutput();

  template <template <typename> class Geometry, typename T>
  void print(const Geometry<T>& g, json::Val attrs);

  template <template <typename> class Geometry, typename T>
  void print(const std::vector<Geometry<T>>& g, json::Val attrs);

  template <typename T>
  void printGeom(const Point<T>& p);

  template <typename T>
  void printGeom(const MultiPoint<T>& ps);

  template <typename T>
  void printGeom(const Line<T>& l);

  template <typename T>
  void printGeom(const MultiLine<T>& l);

  template <typename T>
  void printGeom(const Polygon<T>& l);

  template <typename T>
  void printGeom(const MultiPolygon<T>& l);

  template <typename T>
  void printGeom(const Collection<T>& l);

  template <template <typename> class Geometry, typename T>
  void printLatLng(const Geometry<T>& g, json::Val attrs);

  template <template <typename> class Geometry, typename T>
  void printLatLng(const std::vector<Geometry<T>>& g, json::Val attrs);

  template <typename T>
  void printGeomLatLng(const Point<T>& p);

  template <typename T>
  void printGeomLatLng(const MultiPoint<T>& ps);

  template <typename T>
  void printGeomLatLng(const Line<T>& l);

  template <typename T>
  void printGeomLatLng(const MultiLine<T>& l);

  template <typename T>
  void printGeomLatLng(const Polygon<T>& l);

  template <typename T>
  void printGeomLatLng(const MultiPolygon<T>& l);

  template <typename T>
  void printGeomLatLng(const Collection<T>& l);

  void flush();

 private:
  json::Writer _wr;
};

#include "util/geo/output/GeoJsonOutput.tpp"
}  // namespace output
}  // namespace geo
}  // namespace util

#endif  // UTIL_GEO_OUTPUT_GEOJSONOUTPUT_H_

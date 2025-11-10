// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_GEO_GEO_H_
#define UTIL_GEO_GEO_H_

#define _USE_MATH_DEFINES

#include <array>
#include <functional>
#include <sstream>

#include "util/geo/Box.h"
#include "util/geo/Collection.h"
#include "util/geo/DE9IMatrix.h"
#include "util/geo/IntervalIdx.h"
#include "util/geo/Line.h"
#include "util/geo/Point.h"
#include "util/geo/Polygon.h"

// -------------------
// Geometry stuff
// ------------------

namespace util {
namespace geo {

// convenience aliases

typedef Point<double> DPoint;
typedef Point<float> FPoint;
typedef Point<int> IPoint;
typedef Point<int32_t> I32Point;
typedef Point<int64_t> I64Point;

typedef MultiPoint<double> DMultiPoint;
typedef MultiPoint<float> FMultiPoint;
typedef MultiPoint<int> IMultiPoint;
typedef MultiPoint<int32_t> I32MultiPoint;
typedef MultiPoint<int64_t> I64MultiPoint;

typedef LineSegment<double> DLineSegment;
typedef LineSegment<float> FLineSegment;
typedef LineSegment<int> ILineSegment;
typedef LineSegment<int32_t> I32LineSegment;
typedef LineSegment<int64_t> I64LineSegment;

typedef Line<double> DLine;
typedef Line<float> FLine;
typedef Line<int> ILine;
typedef Line<int32_t> I32Line;
typedef Line<int64_t> I64Line;

typedef MultiLine<double> DMultiLine;
typedef MultiLine<float> FMultiLine;
typedef MultiLine<int> IMultiLine;
typedef MultiLine<int32_t> I32MultiLine;
typedef MultiLine<int64_t> I64MultiLine;

typedef XSortedLine<double> DXSortedLine;
typedef XSortedLine<float> FXSortedLine;
typedef XSortedLine<int> IXSortedLine;
typedef XSortedLine<int32_t> I32XSortedLine;
typedef XSortedLine<int64_t> I64XSortedLine;

typedef Box<double> DBox;
typedef Box<float> FBox;
typedef Box<int> IBox;
typedef Box<int32_t> I32Box;
typedef Box<int64_t> I64Box;

typedef Polygon<double> DPolygon;
typedef Polygon<float> FPolygon;
typedef Polygon<int> IPolygon;
typedef Polygon<int32_t> I32Polygon;
typedef Polygon<int64_t> I64Polygon;

typedef XSortedPolygon<double> DXSortedPolygon;
typedef XSortedPolygon<float> FXSortedPolygon;
typedef XSortedPolygon<int> IXSortedPolygon;
typedef XSortedPolygon<int32_t> I32XSortedPolygon;
typedef XSortedPolygon<int64_t> I64XSortedPolygon;

typedef MultiPolygon<double> DMultiPolygon;
typedef MultiPolygon<float> FMultiPolygon;
typedef MultiPolygon<int> IMultiPolygon;
typedef MultiPolygon<int32_t> I32MultiPolygon;
typedef MultiPolygon<int64_t> I64MultiPolygon;

typedef Collection<double> DCollection;
typedef Collection<float> FCollection;
typedef Collection<int> ICollection;
typedef Collection<int32_t> I32Collection;
typedef Collection<int64_t> I64Collection;

typedef XSortedMultiPolygon<double> DXSortedMultiPolygon;
typedef XSortedMultiPolygon<float> FXSortedMultiPolygon;
typedef XSortedMultiPolygon<int> IXSortedMultiPolygon;
typedef XSortedMultiPolygon<int32_t> I32XSortedMultiPolygon;
typedef XSortedMultiPolygon<int64_t> I64XSortedMultiPolygon;

template <typename T>
struct IntersectorLinePoly {
  static uint8_t check(const LineSegment<T>& ls1, int16_t prevLs1Ang, bool,
                       int16_t nextLs1Ang, bool, const LineSegment<T>& ls2,
                       int16_t prevLs2Ang, bool, int16_t nextLs2Ang, bool);
};

template <typename T>
struct IntersectorPoly {
  static uint8_t check(const LineSegment<T>& ls1, int16_t prevLs1Ang, bool,
                       int16_t nextLs1Ang, bool, const LineSegment<T>& ls2,
                       int16_t, bool, int16_t, bool);
  static uint8_t checkOneDir(const LineSegment<T>& ls1, int16_t prevLs1Ang,
                             int16_t nextLs1Ang, const LineSegment<T>& ls2);
};

template <typename T>
struct IntersectorLine {
  static uint8_t check(const LineSegment<T>& ls1, int16_t prevLs1Ang, bool,
                       int16_t nextLs1Ang, bool, const LineSegment<T>& ls2,
                       int16_t prevLs2Ang, bool, int16_t nextLs2Ang, bool);
};

const static double EPSILON = 0.0000001;
const static double RAD = 0.017453292519943295;  // PI/180
const static double IRAD = 180.0 / M_PI;         // 180 / PI
const static double AVERAGING_STEP = 20;

const static double M_PER_DEG = 111319.4;

enum WKTType : uint8_t {
  NONE = 0,
  POINT = 1,
  LINESTRING = 2,
  POLYGON = 3,
  MULTIPOINT = 4,
  MULTILINESTRING = 5,
  MULTIPOLYGON = 6,
  COLLECTION = 7
};

uint8_t boolArrToInt8(const std::array<bool, 8> arr);

template <typename T>
Box<T> pad(const Box<T>& box, double xPadding, double yPadding);

template <typename T>
Box<T> pad(const Box<T>& box, double padding);

template <typename T>
RotatedBox<T> pad(const RotatedBox<T>& box, double padding);

template <typename T>
Point<T> centroid(const Point<T> p);

template <typename T>
Point<T> centroid(const LineSegment<T>& ls);

template <typename T>
Point<T> centroid(const Line<T>& ls);

template <typename T>
Point<T> ringCentroid(const Line<T>& ls);

template <typename T>
Point<T> centroid(const Polygon<T>& o);

template <typename T>
Point<T> centroid(const Box<T>& box);

template <typename T>
Point<T> centroid(const std::vector<Point<T>>& multigeo);

template <typename T>
Point<T> centroid(const std::vector<Line<T>>& multigeo);

template <typename T>
Point<T> centroid(const std::vector<LineSegment<T>>& multigeo);

template <typename T>
Point<T> centroid(const std::vector<Polygon<T>>& multigeo);

template <typename T>
int8_t dimension(const Collection<T>& col);

template <typename T>
Point<T> centroid(const Collection<T>& col);

template <typename T>
Point<T> rotate(const Point<T>& p, double);

template <typename T>
Point<T> rotateRAD(const Point<T>& p, double);

template <typename T>
Point<T> rotateSinCos(Point<T> p, double si, double co, const Point<T>& c);

template <typename T>
Point<T> rotateRAD(Point<T> p, double rad, const Point<T>& c);

template <typename T>
Point<T> rotate(Point<T> p, double deg, const Point<T>& c);

template <typename T>
LineSegment<T> rotate(LineSegment<T> geo, double deg, const Point<T>& c);

template <typename T>
LineSegment<T> rotateRAD(LineSegment<T> geo, double deg, const Point<T>& c);

template <typename T>
LineSegment<T> rotate(LineSegment<T> geo, double deg);

template <typename T>
LineSegment<T> rotateRAD(LineSegment<T> geo, double deg);

template <typename T>
Line<T> rotateSinCos(Line<T> geo, double si, double co, const Point<T>& c);

template <typename T>
Line<T> rotateRAD(Line<T> geo, double deg, const Point<T>& c);

template <typename T>
Line<T> rotate(Line<T> geo, double deg, const Point<T>& c);

template <typename T>
Polygon<T> rotateSinCos(Polygon<T> geo, double si, double co,
                        const Point<T>& c);

template <typename T>
Polygon<T> rotateRAD(Polygon<T> geo, double deg, const Point<T>& c);

template <typename T>
Polygon<T> rotate(Polygon<T> geo, double deg, const Point<T>& c);

template <template <typename> class Geometry, typename T>
std::vector<Geometry<T>> rotate(std::vector<Geometry<T>> multigeo, double deg,
                                const Point<T>& c);

template <template <typename> class Geometry, typename T>
std::vector<Geometry<T>> rotateRAD(std::vector<Geometry<T>> multigeo,
                                   double deg, const Point<T>& c);

template <template <typename> class Geometry, typename T>
std::vector<Geometry<T>> rotate(std::vector<Geometry<T>> multigeo, double deg);

template <template <typename> class Geometry, typename T>
std::vector<Geometry<T>> rotateRAD(std::vector<Geometry<T>> multigeo,
                                   double deg);

template <typename T>
Point<T> move(const Point<T>& geo, double x, double y);

template <typename T>
Line<T> move(Line<T> geo, double x, double y);

template <typename T>
LineSegment<T> move(LineSegment<T> geo, double x, double y);

template <typename T>
Polygon<T> move(Polygon<T> geo, double x, double y);

template <typename T>
Box<T> move(Box<T> geo, double x, double y);

template <template <typename> class Geometry, typename T>
std::vector<Geometry<T>> move(std::vector<Geometry<T>> multigeo, double x,
                              double y);

template <typename T>
Box<T> minbox();

template <typename T>
RotatedBox<T> shrink(const RotatedBox<T>& b, double d);

bool doubleEq(double a, double b);

template <typename T>
std::string getWKT(const Point<T>& p, uint16_t prec);

template <typename T>
std::string getWKT(const Point<T>& p);

template <typename T>
std::string getWKT(const std::vector<Point<T>>& p, uint16_t prec);

template <typename T>
std::string getWKT(const std::vector<Point<T>>& p);

template <typename T>
std::string getWKT(const Line<T>& l, uint16_t prec);

template <typename T>
std::string getWKT(const Line<T>& l);

template <typename T>
std::string getWKT(const std::vector<Line<T>>& ls, uint16_t prec);

template <typename T>
std::string getWKT(const std::vector<Line<T>>& ls);

template <typename T>
std::string getWKT(const XSortedPolygon<T>& ls, uint16_t prec);

template <typename T>
std::string getWKT(const XSortedPolygon<T>& ls);

template <typename T>
std::string getWKT(const LineSegment<T>& l, uint16_t prec);

template <typename T>
std::string getWKT(const LineSegment<T>& l);

template <typename T>
std::string getWKT(const Box<T>& l, uint16_t prec);

template <typename T>
std::string getWKT(const Box<T>& l);

template <typename T>
std::string getWKT(const Polygon<T>& p, uint16_t prec);

template <typename T>
std::string getWKT(const Polygon<T>& p);

template <typename T>
std::string getWKT(const std::vector<Polygon<T>>& ls, uint16_t prec);

template <typename T>
std::string getWKT(const std::vector<Polygon<T>>& ls);

template <typename T>
std::string getWKT(const Collection<T>& coll, uint16_t prec);

template <typename T>
std::string getWKT(const Collection<T>& coll);

template <typename T>
bool contains(const Point<T>& p, const Box<T>& box);

template <typename T>
bool contains(const Line<T>& l, const Box<T>& box);

template <typename T>
bool contains(const LineSegment<T>& l, const Box<T>& box);

template <typename T>
bool contains(const Box<T>& b, const Box<T>& box);

template <typename T>
bool contains(const Point<T>& p, const LineSegment<T>& ls);

template <typename T>
bool contains(const LineSegment<T>& a, const LineSegment<T>& b);

template <typename T>
bool contains(const Point<T>& p, const Line<T>& l);

template <typename T>
bool ringContains(const Point<T>& p, const Ring<T>& ph);

template <typename T>
int8_t polyContCheck(const Point<T>& a, Point<T> b, Point<T> c);

template <typename T>
bool contains(const Point<T>& p, const Polygon<T>& poly);

template <typename T>
std::pair<double, bool> withinDist(
    const Point<T>& p, const XSortedRing<T>& ph, double maxEuclideanDist,
    double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
double withinDist(
    const Point<T>& p, const XSortedPolygon<T>& poly, double maxEuclideanDist,
    double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
std::pair<bool, bool> ringContains(const Point<T>& p, const XSortedRing<T>& ph,
                                   size_t i);

template <typename T>
double withinDist(
    const Point<T>& p, const XSortedLine<T>& line, double maxEuclideanDist,
    double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
std::tuple<bool, bool> intersectsContains(const Point<T>& p,
                                          const XSortedLine<T>& line, size_t i);

template <typename T>
std::tuple<bool, bool> intersectsContains(const Point<T>& p,
                                          const XSortedLine<T>& line);

template <typename T>
DE9IMatrix DE9IM(const Point<T>& p, const XSortedLine<T>& line, size_t i);

template <typename T>
DE9IMatrix DE9IM(const Point<T>& p, const XSortedLine<T>& line);

template <typename T>
DE9IMatrix DE9IM(const XSortedLine<T>& line, size_t i, const Point<T>& p);

template <typename T>
DE9IMatrix DE9IM(const XSortedLine<T>& line, const Point<T>& p);

template <typename T>
std::pair<bool, bool> containsCovers(const Point<T>& p,
                                     const XSortedPolygon<T>& poly, size_t i);

template <typename T>
std::pair<bool, bool> containsCovers(const Point<T>& p,
                                     const XSortedPolygon<T>& poly);

template <typename T>
DE9IMatrix DE9IM(const Point<T>& p, const XSortedPolygon<T>& poly, size_t i);

template <typename T>
DE9IMatrix DE9IM(const Point<T>& p, const XSortedPolygon<T>& poly);

template <typename T>
DE9IMatrix DE9IM(const XSortedPolygon<T>& poly, size_t i, const Point<T>& p);

template <typename T>
DE9IMatrix DE9IM(const XSortedPolygon<T>& poly, const Point<T>& p);

template <typename T>
bool ringContains(const Ring<T>& a, const Ring<T>& b);

template <typename T>
bool ringIntersects(const Polygon<T>& polyC, const Ring<T>& h);

template <typename T>
bool ringContains(const Polygon<T>& polyC, const Ring<T>& poly);

template <typename T>
bool contains(const Polygon<T>& polyC, const Polygon<T>& poly);

template <typename T>
bool intersects(const Polygon<T>& polyA, const Polygon<T>& polyB);

template <typename T>
bool ringContains(const LineSegment<T>& ls, const Ring<T>& ph);

template <typename T>
bool contains(const LineSegment<T>& ls, const Polygon<T>& p);

template <typename T>
bool contains(const Line<T>& l, const Polygon<T>& poly);

template <typename T>
bool contains(const Line<T>& l, const Line<T>& other);

template <typename T>
bool contains(const Box<T>& b, const Polygon<T>& poly);

template <typename T>
bool contains(const Polygon<T>& poly, const Box<T>& b);

template <typename T>
bool contains(const Polygon<T>& poly, const Line<T>& l);

template <template <typename> class GeometryA,
          template <typename> class GeometryB, typename T>
bool contains(const std::vector<GeometryA<T>>& multigeo,
              const GeometryB<T>& geo);

template <typename T>
bool intersectsNaive(const std::vector<XSortedTuple<T>>& ls1,
                     const std::vector<XSortedTuple<T>>& ls2);

template <typename T, template <typename> class C>
uint8_t intersectsHelper(const std::vector<XSortedTuple<T>>& ls1,
                         const std::vector<XSortedTuple<T>>& ls2, T maxSegLenA,
                         T maxSegLenB, const Box<T>& boxA, const Box<T>& boxB,
                         size_t* firstRelIn1, size_t* firstRelIn2);

template <typename T>
std::tuple<bool, bool, bool, bool, bool, bool, bool, bool> intersectsPoly(
    const std::vector<XSortedTuple<T>>& ls1,
    const std::vector<XSortedTuple<T>>& ls2, T maxSegLenA, T maxSegLenB,
    const Box<T>& boxA, const Box<T>& boxB, size_t* firstRelIn1,
    size_t* firstRelIn2);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsLinePoly(
    const std::vector<XSortedTuple<T>>& ls1,
    const std::vector<XSortedTuple<T>>& ls2, T maxSegLenA, T maxSegLenB,
    const Box<T>& boxA, const Box<T>& boxB, size_t* firstRelIn1,
    size_t* firstRelIn2);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedLine<T>& a,
                 const util::geo::XSortedLine<T>& b, const Box<T>& boxA,
                 const Box<T>& boxB, size_t* firstRelIn1, size_t* firstRelIn2);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedLine<T>& a,
                 const util::geo::XSortedLine<T>& b, const Box<T>& boxA,
                 const Box<T>& boxB);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsLinePoly(
    const XSortedRing<T>& ls1, const XSortedRing<T>& ls2, T maxSegLenA,
    T maxSegLenB, const Box<T>& boxA, const Box<T>& boxB, size_t* firstRelIn1,
    size_t* firstRelIn2);

template <typename T>
std::tuple<bool, bool, bool, bool, bool, bool, bool, bool> intersectsPoly(
    const XSortedRing<T>& ls1, const XSortedRing<T>& ls2, T maxSegLenA,
    T maxSegLenB, const Box<T>& boxA, const Box<T>& boxB, size_t* firstRelIn1,
    size_t* firstRelIn2);

template <typename T>
bool intersects(const LineSegment<T>& ls1, const LineSegment<T>& ls2);

template <typename T>
bool intersects(const Point<T>& a, const Point<T>& b, const Point<T>& c,
                const Point<T>& d);

template <typename T>
bool intersects(const Line<T>& ls1, const Line<T>& ls2);

template <typename T>
bool intersects(const Line<T>& l, const Point<T>& p);

template <typename T>
bool intersects(const Point<T>& p, const Line<T>& l);

template <typename T>
bool intersects(const Polygon<T>& l, const Point<T>& p);

template <typename T>
bool intersects(const Point<T>& p, const Polygon<T>& l);

template <typename T>
bool intersects(const Box<T>& b1, const Box<T>& b2);

template <typename T>
bool intersects(const Box<T>& b, const Polygon<T>& poly);

template <typename T>
bool intersects(const Polygon<T>& poly, const Box<T>& b);

template <typename T>
bool intersects(const LineSegment<T>& ls, const Box<T>& b);

template <typename T>
bool ringIntersects(const LineSegment<T>& ls, const Ring<T>& ph);

template <typename T>
bool intersects(const LineSegment<T>& ls, const Polygon<T>& p);

template <typename T>
bool intersects(const Polygon<T>& p, const LineSegment<T>& ls);

template <typename T>
bool intersects(const Box<T>& b, const LineSegment<T>& ls);

template <typename T>
bool intersects(const Line<T>& l, const Box<T>& b);

template <typename T>
bool intersects(const Box<T>& b, const Line<T>& l);

template <typename T>
bool intersects(const Point<T>& p, const Box<T>& b);

template <typename T>
bool intersects(const Box<T>& b, const Point<T>& p);

template <template <typename> class GeometryA,
          template <typename> class GeometryB, typename T>
bool intersects(const std::vector<GeometryA<T>>& multigeom,
                const GeometryB<T>& b);

template <template <typename> class GeometryA,
          template <typename> class GeometryB, typename T>
bool intersects(const GeometryB<T>& b,
                const std::vector<GeometryA<T>>& multigeom);

template <template <typename> class GeometryA,
          template <typename> class GeometryB, typename T>
bool intersects(const std::vector<GeometryA<T>>& multigeomA,
                const std::vector<GeometryA<T>>& multigeomB);

template <typename T>
std::tuple<bool, bool, bool, bool, bool, bool> intersectsContainsInner(
    const util::geo::XSortedLine<T>& a, const util::geo::XSortedRing<T>& b);

template <typename T>
std::tuple<bool, bool, bool, bool, bool, bool, bool> intersectsContainsInner(
    const util::geo::XSortedRing<T>& a, const util::geo::XSortedRing<T>& b);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedLine<T>& a,
                 const util::geo::Box<T>& boxA,
                 const util::geo::XSortedPolygon<T>& b,
                 const util::geo::Box<T>& boxB, size_t* firstRel1,
                 size_t* firstRel2);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedPolygon<T>& a,
                 const util::geo::Box<T>& boxA,
                 const util::geo::XSortedLine<T>& b,
                 const util::geo::Box<T>& boxB, size_t* firstRel1,
                 size_t* firstRel2);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedLine<T>& a,
                 const util::geo::Box<T>& boxA,
                 const util::geo::XSortedPolygon<T>& b,
                 const util::geo::Box<T>& boxB);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedLine<T>& a,
                 const util::geo::XSortedPolygon<T>& b);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedPolygon<T>& a,
                 const util::geo::Box<T>& boxA,
                 const util::geo::XSortedLine<T>& b,
                 const util::geo::Box<T>& boxB);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedPolygon<T>& a,
                 const util::geo::XSortedLine<T>& b);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsContainsCovers(
    const util::geo::XSortedPolygon<T>& a, const util::geo::Box<T>& boxA,
    double outerAreaA, const util::geo::XSortedPolygon<T>& b,
    const util::geo::Box<T>& boxB, double outerAreaB, size_t* firstRel1,
    size_t* firstRel2);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsContainsCovers(
    const util::geo::XSortedPolygon<T>& a, const util::geo::Box<T>& boxA,
    double outerAreaA, const util::geo::XSortedPolygon<T>& b,
    const util::geo::Box<T>& boxB, double outerAreaB);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsContainsCovers(
    const util::geo::XSortedPolygon<T>& a,
    const util::geo::XSortedPolygon<T>& b);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedPolygon<T>& a,
                 const util::geo::Box<T>& boxA, double outerAreaA,
                 const util::geo::XSortedPolygon<T>& b,
                 const util::geo::Box<T>& boxB, double outerAreaB,
                 size_t* firstRel1, size_t* firstRel2);

template <typename T>
DE9IMatrix DE9IM(const util::geo::Point<T>& a, const util::geo::Point<T>& b);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedPolygon<T>& a,
                 const util::geo::Box<T>& boxA, double outerAreaA,
                 const util::geo::XSortedPolygon<T>& b,
                 const util::geo::Box<T>& boxB, double outerAreaB);

template <typename T>
DE9IMatrix DE9IM(const util::geo::XSortedPolygon<T>& a,
                 const util::geo::XSortedPolygon<T>& b);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsCovers(
    const util::geo::XSortedLine<T>& a, const util::geo::XSortedLine<T>& b,
    const Box<T>& boxA, const Box<T>& boxB, size_t* firstRelIn1,
    size_t* firstRelIn2);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsCovers(
    const util::geo::XSortedLine<T>& a, const util::geo::XSortedLine<T>& b,
    const Box<T>& boxA, const Box<T>& boxB);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsContainsCovers(
    const util::geo::XSortedLine<T>& a, const util::geo::Box<T>& boxA,
    const util::geo::XSortedPolygon<T>& b, const util::geo::Box<T>& boxB,
    size_t* firstRel1, size_t* firstRel2);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsContainsCovers(
    const util::geo::XSortedLine<T>& a, const util::geo::Box<T>& boxA,
    const util::geo::XSortedPolygon<T>& b, const util::geo::Box<T>& boxB);

template <typename T>
std::tuple<bool, bool, bool, bool, bool> intersectsContainsCovers(
    const util::geo::XSortedLine<T>& a, const util::geo::XSortedPolygon<T>& b);

template <typename T>
Point<T> intersection(T p1x, T p1y, T q1x, T q1y, T p2x, T p2y, T q2x, T q2y);

template <typename T>
Point<T> intersection(const Point<T>& p1, const Point<T>& q1,
                      const Point<T>& p2, const Point<T>& q2);

template <typename T>
Point<T> intersection(const LineSegment<T>& s1, const LineSegment<T>& s2);

template <typename T>
std::vector<Point<T>> intersection(const Line<T>& l1, const Line<T>& l2);

template <typename T>
Box<T> intersection(const Box<T>& b1, const Box<T>& b2);

template <typename T>
bool lineIntersects(T p1x, T p1y, T q1x, T q1y, T p2x, T p2y, T q2x, T q2y);

template <typename T>
bool lineIntersects(const Point<T>& p1, const Point<T>& q1, const Point<T>& p2,
                    const Point<T>& q2);

double angBetween(double p1x, double p1y);

template <typename T>
double angBetween(const Point<T>& p1);

template <typename T>
double angBetween(const Point<T>& p1, const MultiPoint<T>& points);

double dist(double x1, double y1, double x2, double y2);

template <typename T>
double dist(const LineSegment<T>& ls, const Point<T>& p);

template <typename T>
double dist(const Point<T>& p, const LineSegment<T>& ls);

template <typename T>
double dist(const LineSegment<T>& ls1, const LineSegment<T>& ls2);

template <typename T>
double dist(
    const LineSegment<T>& ls1, const LineSegment<T>& ls2,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
double dist(const Point<T>& p, const Line<T>& l);

template <typename T>
double dist(const Line<T>& l, const Point<T>& p);

template <typename T>
double dist(const LineSegment<T>& ls, const Line<T>& l);

template <typename T>
double dist(const Line<T>& l, const LineSegment<T>& ls);

template <typename T>
double dist(const Line<T>& la, const Line<T>& lb);

template <template <typename> class GeometryA,
          template <typename> class GeometryB, typename T>
double dist(const std::vector<GeometryA<T>>& multigeom, const GeometryB<T>& b);

template <template <typename> class GeometryA,
          template <typename> class GeometryB, typename T>
double dist(const GeometryB<T>& b, const std::vector<GeometryA<T>>& multigeom);

template <template <typename> class GeometryA,
          template <typename> class GeometryB, typename T>
double dist(const std::vector<GeometryA<T>>& multigeomA,
            const std::vector<GeometryB<T>>& multigeomB);

double innerProd(double x1, double y1, double x2, double y2, double x3,
                 double y3);

template <typename T>
double innerProd(const Point<T>& a, const Point<T>& b, const Point<T>& c);

double crossProd(double x1, double y1, double x2, double y2);

template <typename T>
double crossProd(const Point<T>& a, const Point<T>& b);

template <typename T>
double crossProd(const Point<T>& p, const LineSegment<T>& ls);

template <typename T>
double dist(const Polygon<T>& poly1, const Polygon<T>& poly2);

template <typename T>
double dist(const Line<T>& l, const Polygon<T>& poly);

template <typename T>
double dist(const Point<T>& p, const Polygon<T>& poly);

template <typename T>
double dist(const Point<T>& p1, const Point<T>& p2);

template <typename T>
size_t numPoints(const Point<T>&);

template <typename T>
size_t numPoints(const Line<T>& l);

template <typename T>
size_t numPoints(const Polygon<T>& p);

template <template <typename> class Geometry, typename T>
size_t numPoints(const std::vector<Geometry<T>>& pol);

template <typename T>
size_t numPoints(const Collection<T>& collection);

template <typename T>
bool empty(const Point<T>&);

template <typename T>
bool empty(const Polygon<T>& g);

template <typename T>
bool empty(const Line<T>& g);

template <template <typename> class Geometry, typename T>
size_t empty(const std::vector<Geometry<T>>& pol);

template <typename T>
bool empty(const Collection<T>& g);

template <typename T, typename F>
Line<T> lineFromWKTProj(const char* c, const char** endr, F projFunc);

template <typename T>
Line<T> lineFromWKT(const char* c, const char** endr);

template <typename T>
MultiLine<T> multiLineFromWKT(const char* c, const char** endr);

template <typename T, typename F>
MultiPoint<T> multiPointFromWKTProj(const char* c, const char** endr,
                                    F projFunc);

template <typename T>
MultiPoint<T> multiPointFromWKT(const char* c, const char** endr);

template <typename T>
MultiPoint<T> multiPointFromWKT(const std::string& wkt);

template <typename T, typename F>
MultiPoint<T> multiPointFromWKTProj(
    const std::string& wkt,
    std::function<Point<T>(const Point<double>& p1)> projFunc);

template <typename T, typename F>
Point<T> pointFromWKTProj(const char* c, const char** endr, F projFunc);

template <typename T>
Point<T> pointFromWKT(const char* c, const char** endr);

template <typename T>
Point<T> pointFromWKT(std::string wkt);

template <typename T, typename F>
Point<T> pointFromWKTProj(std::string wkt, std::function<F> projFunc);

template <typename T, typename F>
Polygon<T> polygonFromWKTProj(const char* c, const char** endr, F projFunc);

template <typename T>
Polygon<T> polygonFromWKT(const char* c, const char** endr);

template <typename T>
Polygon<T> polygonFromWKT(std::string wkt);

template <typename T, typename F>
Polygon<T> polygonFromWKTProj(std::string wkt, F projFunc);

template <typename T, typename F>
MultiLine<T> multiLineFromWKTProj(const char* c, const char** endr, F projFunc);

template <typename T, typename F>
MultiPolygon<T> multiPolygonFromWKTProj(const char* c, const char** endr,
                                        F projFunc);

template <typename T>
MultiPolygon<T> multiPolygonFromWKT(const char* c, const char** endr);

WKTType getWKTType(const char* c, const char** endr);

WKTType getWKTType(const char* c);

WKTType getWKTType(const std::string& str);

template <typename T, typename F>
Collection<T> collectionFromWKTProj(const char* c, const char** endr,
                                    F projFunc);

template <typename T>
Collection<T> collectionFromWKT(const char* c, const char** endr);

template <typename T>
Line<T> lineFromWKT(const std::string& wkt);

template <typename T>
Line<T> lineFromWKTProj(
    const std::string& wkt,
    std::function<Point<T>(const Point<double>& p1)> projFunc);

template <typename T>
MultiLine<T> multiLineFromWKT(const std::string& wkt);

template <typename T>
MultiLine<T> multiLineFromWKTProj(
    const std::string& wkt,
    std::function<Point<T>(const Point<double>& p1)> projFunc);

template <typename T>
MultiPolygon<T> multiPolygonFromWKT(const std::string& wkt);

template <typename T>
MultiPolygon<T> multiPolygonFromWKTProj(
    const std::string& wkt,
    std::function<Point<T>(const Point<double>& p1)> projFunc);

template <typename T>
Collection<T> collectionFromWKT(const std::string& wkt);

template <typename T>
Collection<T> collectionFromWKTProj(
    const std::string& wkt,
    std::function<Point<T>(const Point<double>& p1)> projFunc);

template <typename T>
double len(const Point<T>&);

template <typename T>
double len(const Line<T>& g);

template <typename T>
double len(const Polygon<T>& g);

template <typename T>
double len(const std::vector<XSortedTuple<T>>& g);

template <typename T>
double len(const XSortedLine<T>& g);

template <typename T>
double len(const LineSegment<T>& g);

template <template <typename> class Geometry, typename T>
size_t len(const std::vector<Geometry<T>>& pol);

template <typename T>
double len(const Collection<T>& collection);

template <typename T>
bool shorterThan(const Line<T>& g, double d);

template <typename T>
bool longerThan(const Line<T>& g, double d);

template <typename T>
bool longerThan(const Line<T>& a, const Line<T>& b, double d);

template <typename T>
Point<T> simplify(const Point<T>& g, double);

template <typename T>
LineSegment<T> simplify(const LineSegment<T>& g, double);

template <typename T>
Box<T> simplify(const Box<T>& g, double);

template <typename T>
RotatedBox<T> simplify(const RotatedBox<T>& g, double);

template <typename T>
Line<T> simplify(const Line<T>& g, double d);

template <typename T>
Polygon<T> simplify(const Polygon<T>& g, double d);

template <template <typename> class Geometry, typename T>
std::vector<Geometry<T>> simplify(const std::vector<Geometry<T>>& pol,
                                  double d);

template <typename T>
Collection<T> simplify(const Collection<T>& collection, double d);

double distToSegment(double lax, double lay, double lbx, double lby, double px,
                     double py);

template <typename T>
double distToSegment(const Point<T>& la, const Point<T>& lb, const Point<T>& p);

template <typename T>
double distToSegment(const LineSegment<T>& ls, const Point<T>& p);

template <typename T>
Point<T> projectOn(const Point<T>& a, const Point<T>& b, const Point<T>& c);

template <typename T>
double parallelity(const Box<T>& box, const Line<T>& line);

template <typename T>
double parallelity(const Box<T>& box, const MultiLine<T>& multiline);

template <template <typename> class Geometry, typename T>
RotatedBox<T> getOrientedEnvelope(const std::vector<Geometry<T>>& pol);

template <template <typename> class Geometry, typename T>
RotatedBox<T> getOrientedEnvelope(const Geometry<T>& pol);

template <typename T>
RotatedBox<T> getOrientedEnvelope(const Collection<T>& collection);

template <typename T>
Polygon<T> buffer(const Line<T>& line, double d, size_t points);

template <typename T>
Polygon<T> buffer(const Polygon<T>& pol, double d, size_t points);

template <typename T>
Box<T> getBoundingBox(const Point<T>& p);

template <typename T>
Box<T> getBoundingBox(const Line<T>& l);

template <typename T>
Box<T> getBoundingBox(const Polygon<T>& pol);

template <typename T>
Box<T> getBoundingBox(const LineSegment<T>& ls);

template <typename T>
Box<T> getBoundingBox(const Box<T>& b);

template <template <typename> class Geometry, typename T>
Box<T> getBoundingBox(const std::vector<Geometry<T>>& multigeo);

template <typename T>
Box<T> getBoundingBox(const Collection<T>& coll);

template <typename T>
Box<T> getBoundingRect(const Box<T>& b);

template <template <typename> class Geometry, typename T>
Box<T> getBoundingRect(const Geometry<T> geom);

template <typename T>
double getEnclosingRadius(const Point<T>& p, const Point<T>& pp);

template <typename T>
double getEnclosingRadius(const Point<T>& p, const Line<T>& l);

template <typename T>
double getEnclosingRadius(const Point<T>& p, const Polygon<T>& pg);

template <template <typename> class Geometry, typename T>
double getEnclosingRadius(const Point<T>& p,
                          const std::vector<Geometry<T>>& multigeom);

template <typename T>
Polygon<T> convexHull(const Point<T>& p);

template <typename T>
Polygon<T> convexHull(const Box<T>& b);

template <typename T>
Polygon<T> convexHull(const LineSegment<T>& b);

template <typename T>
Polygon<T> convexHull(const RotatedBox<T>& b);

template <typename T>
size_t convexHullImpl(const MultiPoint<T>& a, size_t p1, size_t p2, Line<T>* h);

template <typename T>
Polygon<T> convexHull(const MultiPoint<T>& l);

template <typename T>
Polygon<T> convexHull(const Polygon<T>& p);

template <typename T>
Polygon<T> convexHull(const MultiPolygon<T>& ps);

template <typename T>
Polygon<T> convexHull(const MultiLine<T>& ls);

template <typename T>
Polygon<T> convexHull(const Collection<T>& collection);

template <typename T>
Box<T> extendBox(const Line<T>& l, Box<T> b);

template <typename T>
Box<T> extendBox(const LineSegment<T>& ls, Box<T> b);

template <typename T>
Box<T> extendBox(const Polygon<T>& ls, Box<T> b);

template <template <typename> class Geometry, typename T>
Box<T> extendBox(const std::vector<Geometry<T>>& multigeom, Box<T> b);

template <typename T>
Box<T> extendBox(const Collection<T>& collection, Box<T> b);

template <typename T>
Point<T> pointAt(const Line<T> l, double at);

template <typename T>
Point<T> pointAt(const Line<T> l, double at, size_t* lastI, double* totPos);

template <typename T>
Point<T> pointAtDist(const Line<T> l, double atDist);

template <typename T>
Point<T> pointAtDist(const Line<T> l, double atDist, size_t* lastI,
                     double* totPos);

template <typename T>
Point<T> interpolate(const Point<T>& a, const Point<T>& b, double d);

template <typename T>
Line<T> orthoLineAtDist(const Line<T>& l, double d, double length);

template <typename T>
Line<T> segment(const Line<T>& line, double a, double b);

template <typename T>
Line<T> segment(const Line<T>& line, const Point<T>& start, size_t startI,
                const Point<T>& end, size_t endI);

template <typename T>
Line<T> average(const std::vector<const Line<T>*>& lines);

template <typename T>
Line<T> average(const std::vector<const Line<T>*>& lines,
                const std::vector<double>& weights);

template <typename T>
double area(const Point<T>&);

template <typename T>
double area(const LineSegment<T>&);

template <typename T>
double area(const Line<T>&);

template <typename T>
double area(const Box<T>& b);

template <typename T>
double area(const Collection<T>& col);

template <template <typename> class Geometry, typename T>
double area(const std::vector<Geometry<T>>& gs);

template <typename T>
double commonArea(const Box<T>& ba, const Box<T>& bb);

template <template <typename> class Geometry, typename T>
RotatedBox<T> getFullEnvelope(std::vector<Geometry<T>> pol);

template <template <typename> class Geometry, typename T>
RotatedBox<T> getFullEnvelope(const Geometry<T> pol);

template <typename T>
RotatedBox<T> getOrientedEnvelopeAvg(MultiLine<T> ml);

template <typename T>
double haversine(T lat1, T lon1, T lat2, T lon2);

template <typename T>
double haversine(const Point<T>& a, const Point<T>& b);

template <typename T>
Line<T> densify(const Line<T>& l, double d);

template <typename T>
double frechetDistC(size_t i, size_t j, const Line<T>& p, const Line<T>& q,
                    std::vector<float>& ca);

template <typename T>
double frechetDist(const Line<T>& a, const Line<T>& b, double d);

template <typename T>
double accFrechetDistC(const Line<T>& a, const Line<T>& b, double d);

template <typename T>
double frechetDistCHav(size_t i, size_t j, const Line<T>& p, const Line<T>& q,
                       std::vector<float>& ca);

template <typename T>
double frechetDistHav(const Line<T>& a, const Line<T>& b, double d);

template <typename T>
double accFrechetDistCHav(const Line<T>& a, const Line<T>& b, double d);

template <typename T>
Point<T> latLngToWebMerc(double lat, double lng);

template <typename T>
Point<T> latLngToWebMerc(Point<T> lngLat);

template <typename T>
Point<T> webMercToLatLng(double x, double y);

template <typename T>
double webMercMeterDist(const Point<T>& a, const Point<T>& b);

template <typename G1, typename G2>
double webMercMeterDist(const G1& a, const G2& b);

template <typename T>
double webMercLen(const Line<T>& g);

template <typename T>
double latLngLen(const Line<T>& g);

template <typename G>
double webMercDistFactor(const G& a);

template <typename G>
double latLngDistFactor(const G& a);

template <typename T>
double withinDist(
    const std::vector<XSortedTuple<T>>& ls1,
    const std::vector<XSortedTuple<T>>& ls2, T maxSegLenA, T maxSegLenB,
    const Box<T>& boxA, const Box<T>& boxB, double maxEuclideanDistX,
    double maxEuclideanDistY, double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
std::pair<double, bool> withinDist(
    const XSortedRing<T>& p1, const XSortedRing<T>& p2, const Box<T>& boxA,
    const Box<T>& boxB, double maxEuclideanDistX, double maxEuclideanDistY,
    double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
std::pair<double, bool> withinDist(
    const XSortedLine<T>& ls1, const XSortedRing<T>& p2, const Box<T>& boxA,
    const Box<T>& boxB, double maxEuclideanDistX, double maxEuclideanDistY,
    double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
double withinDist(
    const XSortedLine<T>& a, const XSortedPolygon<T>& b, const Box<T>& boxA,
    const Box<T>& boxB, double maxEuclideanDistX, double maxEuclideanDistY,
    double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
double withinDist(
    const XSortedPolygon<T>& p1, const XSortedPolygon<T>& p2,
    const Box<T>& boxA, const Box<T>& boxB, double maxEuclideanDistX,
    double maxEuclideanDistY, double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

template <typename T>
double withinDist(
    const XSortedLine<T>& ls1, const XSortedLine<T>& ls2, const Box<T>& padboxA,
    const Box<T>& padboxB, double maxEuclideanDistX, double maxEuclideanDistY,
    double maxDist,
    std::function<double(const Point<T>& p1, const Point<T>& p2)> distFunc);

}  // namespace geo
}  // namespace util

#include "Geo.tpp"

#endif  // UTIL_GEO_GEO_H_

// Copyright 2016
// Author: Patrick Brosi

#include "util/Test.h"
#include "util/geo/Geo.h"
#include "util/geo/Grid.h"
#include "util/geo/RTree.h"
#include "util/geo/output/GeoJsonOutput.h"
#include "util/tests/GeoTest.h"

using namespace util;
using namespace util::geo;

// _____________________________________________________________________________
void GeoTest::run() {
  // WKT PARSING
  {
    TEST(util::geo::getWKTType("LINESTRING(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);

    TEST(util::geo::getWKTType("MLINESTRING(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  MLINESTRING (0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  MLINESTRING Z(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  LINESTRING Z(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  LINESTRING M(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  LINESTRING ZM(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  LINESTRING (0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  LINESTRIN (0 0, 1 1)", 0), ==, WKTType::NONE);
    TEST(util::geo::getWKTType("  INESTRING (0 0, 1 1)", 0), ==, WKTType::NONE);

    TEST(util::geo::getWKTType("linestring(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("mlinestring(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  mlinestring (0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  mlinestring z(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  linestring z(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  linestring (0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);

    TEST(util::geo::getWKTType("liNestRing(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("mlinestrIng(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  mliNestring (0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  MlInestring z(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  Linestring z(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  Linestring ZM(0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);
    TEST(util::geo::getWKTType("  linestrinG (0 0, 1 1)", 0), ==,
         WKTType::LINESTRING);

    TEST(util::geo::getWKT(lineFromWKT<int>("LINESTRING(0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("LINESTRING (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("LINESTRING   (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(
             lineFromWKT<int>("   LINESTRING   (  0    0  , 1  1   )", 0)),
         ==, "LINESTRING(0 0,1 1)");

    TEST(util::geo::getWKT(lineFromWKT<int>("(0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>(" (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("   (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("      (  0    0  , 1  1   )", 0)),
         ==, "LINESTRING(0 0,1 1)");

    TEST(util::geo::getWKT(lineFromWKT<int>("LINESTRING Z(0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("LINESTRING Z (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("LINESTRING Z  (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(
             lineFromWKT<int>("   LINESTRING Z   (  0    0  , 1  1   )", 0)),
         ==, "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("MLINESTRING (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("MLINESTRING  (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>("MLINESTRING   (0 0, 1 1)", 0)), ==,
         "LINESTRING(0 0,1 1)");
    TEST(util::geo::getWKT(lineFromWKT<int>(
             "   MLINESTRING    (  0    0  0.5 , 1  1 100  )", 0)),
         ==, "LINESTRING(0 0,1 1)");
  }

  {
    TEST(util::geo::getWKTType("MULTIPOINT(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("MMULTIPOINT(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MMULTIPOINT (0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MMULTIPOINT Z(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MULTIPOINT Z(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MULTIPOINT M(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MULTIPOINT ZM(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MULTIPOINT (0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MULTIPOIN (0 0, 1 1)", 0), ==, WKTType::NONE);
    TEST(util::geo::getWKTType("  ULTIPOINT (0 0, 1 1)", 0), ==, WKTType::NONE);

    TEST(util::geo::getWKTType("multipoint(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("mmultipoint(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  mmultipoint (0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  mmultipoint z(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  mmultipoint z(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  mmultipoint (0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);

    TEST(util::geo::getWKTType("mUltIpOint(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("mMultiPoint(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  Multipoint z(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  Multipoint ZM(0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);
    TEST(util::geo::getWKTType("  MultipoinT (0 0, 1 1)", 0), ==,
         WKTType::MULTIPOINT);

    TEST(util::geo::getWKT(multiPointFromWKT<int>("MULTIPOINT(0 0, 1 1)", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(
             multiPointFromWKT<int>("MULTIPOINT((0 0), (1 1))", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(multiPointFromWKT<int>("MULTIPOINT (0 0, 1 1)", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(
        util::geo::getWKT(multiPointFromWKT<int>("MULTIPOINT   (0 0, 1 1)", 0)),
        ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(multiPointFromWKT<int>(
             "   MULTIPOINT   (  0    0  , 1  1   )", 0)),
         ==, "MULTIPOINT(0 0,1 1)");

    TEST(util::geo::getWKT(multiPointFromWKT<int>("(0 0, 1 1)", 0)), ==,
         "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(multiPointFromWKT<int>(" (0 0, 1 1)", 0)), ==,
         "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(multiPointFromWKT<int>("   (0 0, 1 1)", 0)), ==,
         "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(
             multiPointFromWKT<int>("      (  0    0  , 1  1   )", 0)),
         ==, "MULTIPOINT(0 0,1 1)");

    TEST(util::geo::getWKT(multiPointFromWKT<int>("MULTIPOINT Z(0 0, 1 1)", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(
        util::geo::getWKT(multiPointFromWKT<int>("MULTIPOINT Z (0 0, 1 1)", 0)),
        ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(
             multiPointFromWKT<int>("MULTIPOINT Z  (0 0, 1 1)", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(multiPointFromWKT<int>(
             "   MULTIPOINT Z   (  0    0  , 1  1   )", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(multiPointFromWKT<int>("MMULTIPOINT (0 0, 1 1)", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(
             multiPointFromWKT<int>("MMULTIPOINT  Z  ( (0 0 ),  ( 1 1 ) )", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(
        util::geo::getWKT(multiPointFromWKT<int>("MMULTIPOINT  (0 0, 1 1)", 0)),
        ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(
             multiPointFromWKT<int>("MMULTIPOINT   (0 0, 1 1)", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
    TEST(util::geo::getWKT(multiPointFromWKT<int>(
             "   MMULTIPOINT    (  0    0  0.5 , 1  1 100  )", 0)),
         ==, "MULTIPOINT(0 0,1 1)");
  }

  {
    TEST(util::geo::getWKTType("POINT(1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType("POIN (1 1)", 0), ==, WKTType::NONE);
    TEST(util::geo::getWKTType(" POINT (1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType(" Point (1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType("POINT Z(1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType("   POINT Z (1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType(" POINT ZM(1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType(" Point MZ (1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType("mPoint MZ (1 1)", 0), ==, WKTType::POINT);
    TEST(util::geo::getWKTType(" oint MZ (1 1)", 0), ==, WKTType::NONE);

    TEST(util::geo::getWKT(pointFromWKT<int>("POINT(1 1)")), ==, "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("POINT (1 1)")), ==, "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("POINT   ( 1 1)")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("   POINT   (   1  1   )")), ==,
         "POINT(1 1)");

    TEST(util::geo::getWKT(pointFromWKT<int>("( 1 1)")), ==, "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>(" ( 1 1)")), ==, "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("   ( 1 1)")), ==, "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("      (   1  1   )")), ==,
         "POINT(1 1)");

    TEST(util::geo::getWKT(pointFromWKT<int>("POINT Z( 1 1)")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("POINT Z ( 1 1)")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("POINT Z  ( 1 1)")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("   POINT Z   (   1  1   )")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("MPOINT ( 1 1 50)")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("MPOINT  ( 1 1)")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("MPOINT   ( 1 1)")), ==,
         "POINT(1 1)");
    TEST(util::geo::getWKT(pointFromWKT<int>("   MPOINT    (   1  1 0.5  )")),
         ==, "POINT(1 1)");
  }

  {
    TEST(util::geo::getWKTType("POLYGON(0 0, 1 1)"), ==, WKTType::POLYGON);
    TEST(util::geo::getWKTType("POLYGO(0 0, 1 1)"), ==, WKTType::NONE);
    TEST(util::geo::getWKTType("  Polygon (0 0, 1 1)"), ==, WKTType::POLYGON);
    TEST(util::geo::getWKTType("  PolygonZ (0 0, 1 1)"), ==, WKTType::POLYGON);
    TEST(util::geo::getWKTType("  PolygonM (0 0, 1 1)"), ==, WKTType::POLYGON);
    TEST(util::geo::getWKTType("  Polygon ZM (0 0, 1 1)"), ==,
         WKTType::POLYGON);
    TEST(util::geo::getWKTType("  polygon ZM (0 0, 1 1)"), ==,
         WKTType::POLYGON);
    TEST(util::geo::getWKTType("  mpolygon ZM (0 0, 1 1)"), ==,
         WKTType::POLYGON);

    TEST(util::geo::getWKT(polygonFromWKT<int>("POLYGON((0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("POLYGON ((0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("POLYGON (  (0 0, 1 1) )")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(
             polygonFromWKT<int>("   POLYGON   (  (0    0  , 1  1   )  ) ")),
         ==, "POLYGON((0 0,1 1,0 0))");

    TEST(util::geo::getWKT(polygonFromWKT<int>("((0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("( (0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>(" (  (0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(
        util::geo::getWKT(polygonFromWKT<int>(" (     (  0    0  , 1  1   ))")),
        ==, "POLYGON((0 0,1 1,0 0))");

    TEST(util::geo::getWKT(polygonFromWKT<int>("POLYGON Z((0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("POLYGON Z( (0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("POLYGON Z(  (0 0, 1 1) )")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(
             polygonFromWKT<int>("   POLYGON Z   (  (0    0  , 1  1 (  )")),
         ==, "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("MPOLYGON ((0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("MPOLYGON ( (0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>("MPOLYGON (  (0 0, 1 1))")), ==,
         "POLYGON((0 0,1 1,0 0))");
    TEST(util::geo::getWKT(polygonFromWKT<int>(
             "   MPOLYGON  (  (  0    0  0.5 , 1  1 100  ))")),
         ==, "POLYGON((0 0,1 1,0 0))");
  }

  {
    TEST(util::geo::getWKTType("MULTILINESTRIN((1 1,3 3),(1 3,3 1))"), ==,
         WKTType::NONE);
    TEST(util::geo::getWKTType("MULTILINESTRING((1 1,3 3),(1 3,3 1))"), ==,
         WKTType::MULTILINESTRING);
    TEST(util::geo::getWKTType(" MULTILINESTRING  ((1 1,3 3),(1 3,3 1))"), ==,
         WKTType::MULTILINESTRING);
    TEST(util::geo::getWKTType("  MMULTILINESTRING  ((1 1,3 3),(1 3,3 1))"), ==,
         WKTType::MULTILINESTRING);
    TEST(util::geo::getWKTType(" MultiLineString  ((1 1,3 3),(1 3,3 1))"), ==,
         WKTType::MULTILINESTRING);
    TEST(util::geo::getWKTType("  MultiLineString\tM((1 1,3 3),(1 3,3 1))"), ==,
         WKTType::MULTILINESTRING);
    TEST(util::geo::getWKTType("  MMultiLineString Z((1 1,3 3),(1 3,3 1))"), ==,
         WKTType::MULTILINESTRING);
    TEST(util::geo::getWKTType("\t\tMMultiLineString Z((1 1,3 3),(1 3,3 1))"),
         ==, WKTType::MULTILINESTRING);

    TEST(util::geo::getWKT(
             multiLineFromWKT<int>("MULTILINESTRING((1 1,3 3),(1 3,3 1))")),
         ==, "MULTILINESTRING((1 1,3 3),(1 3,3 1))");
    TEST(util::geo::getWKT(multiLineFromWKT<int>(
             " MULTILINESTRING  ( (1 1,3 3) ,(1 3,3 1))")),
         ==, "MULTILINESTRING((1 1,3 3),(1 3,3 1))");
    TEST(util::geo::getWKT(multiLineFromWKT<int>(
             " MULTILINESTRING Z  ( (1 1,3 3) ,(1 3,3 1 ) )")),
         ==, "MULTILINESTRING((1 1,3 3),(1 3,3 1))");
    TEST(util::geo::getWKT(multiLineFromWKT<int>(
             " MULTILINESTRING Z  ( (1 1  ,3 3) ,(1   3 ,  3 1 ) )")),
         ==, "MULTILINESTRING((1 1,3 3),(1 3,3 1))");
  }

  {
    TEST(util::geo::getWKTType(
             "MULTIPOLYGON(((1 1,3 3,1 1), (0 0,1 1,0 0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::MULTIPOLYGON);
    TEST(util::geo::getWKTType(
             "MULTIPOLYGO (((1 1,3 3,1 1), (0 0,1 1,0 0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::NONE);
    TEST(util::geo::getWKTType(
             " MULTIPOLYGON (((1 1,3 3,1 1), (0 0,1 1,0 0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::MULTIPOLYGON);
    TEST(
        util::geo::getWKTType(
            " MMULTIPOLYGON (((1 1,3 3,1 1), (0 0,1 1,0 0)),((1 3,3 1, 1 3)))"),
        ==, WKTType::MULTIPOLYGON);
    TEST(util::geo::getWKTType(" MULTIPOLYGON Z (((1 1,3 3,1 1), (0 0,1 1,0 "
                               "0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::MULTIPOLYGON);
    TEST(util::geo::getWKTType(" MULTIPOLYGON ZM (((1 1,3 3,1 1), (0 0,1 1,0 "
                               "0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::MULTIPOLYGON);
    TEST(util::geo::getWKTType("  MultiPolygon ZM (((1 1,3 3,1 1), (0 0,1 1,0 "
                               "0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::MULTIPOLYGON);
    TEST(util::geo::getWKTType("M MultiPolygon ZM (((1 1,3 3,1 1), (0 0,1 1,0 "
                               "0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::MULTIPOLYGON);
    TEST(util::geo::getWKTType("\tM\tMultiPolygon ZM\t(((1 1,3 3,1 1), (0 0,1 "
                               "1,0 0)),((1 3,3 1, 1 3)))"),
         ==, WKTType::MULTIPOLYGON);

    TEST(util::geo::getWKT(multiPolygonFromWKT<int>(
             "MULTIPOLYGON(((1 1,3 3,1 1), (0 0,1 1,0 0)),((1 3,3 1, 1 3)))")),
         ==, "MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3)))");
    TEST(util::geo::getWKT(
             multiPolygonFromWKT<int>("MULTIPOLYGON(((1 1 ,3  3, 1 1 ), (0 0,1 "
                                      "1,0 0)),((1 3,3 1, 1 3)))")),
         ==, "MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3)))");
    TEST(util::geo::getWKT(
             multiPolygonFromWKT<int>("MULTIPOLYGON Z (  (   ( 1    1 ,3  3, 1 "
                                      "1 ), (0 0,1 1,0 0)),((1 3,3 1, 1 3)))")),
         ==, "MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3)))");
    TEST(
        util::geo::getWKT(multiPolygonFromWKT<int>(
            " (  (   ( 1    1 ,3  3, 1 1 ), (0 0,1 1,0 0)),((1 3,3 1, 1 3)))")),
        ==, "MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3)))");
  }

  {
    TEST(util::geo::getWKTType("GEOMETRYCOLLECTION(MULTIPOLYGON(((1 1,3 3,1 "
                               "1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::COLLECTION);
    TEST(util::geo::getWKTType("GEOMETRYCOLLECTIO (MULTIPOLYGON(((1 1,3 3,1 "
                               "1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::NONE);
    TEST(util::geo::getWKTType(" GEOMETRYCOLLECTION (MULTIPOLYGON(((1 1,3 3,1 "
                               "1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::COLLECTION);
    TEST(util::geo::getWKTType(" GeometryCollection (MULTIPOLYGON(((1 1,3 3,1 "
                               "1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::COLLECTION);
    TEST(util::geo::getWKTType("\t\tGeometryCollection (MULTIPOLYGON(((1 1,3 "
                               "3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::COLLECTION);
    TEST(util::geo::getWKTType("\t\tmGeometryCollection (MULTIPOLYGON(((1 1,3 "
                               "3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::COLLECTION);
    TEST(util::geo::getWKTType("\t\tmGeometryCollection M(MULTIPOLYGON(((1 1,3 "
                               "3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::COLLECTION);
    TEST(util::geo::getWKTType("\t\tmGeometryCollection ZM(MULTIPOLYGON(((1 "
                               "1,3 3,1 1),(0 0,1 1,0 0)),((1 3,3 1,1 3))))"),
         ==, WKTType::COLLECTION);

    TEST(util::geo::getWKT(collectionFromWKT<int>(
             "GEOMETRYCOLLECTION(MULTIPOLYGON(((1 1,3 3,1 1), (0 0,1 1,0 "
             "0)),((1 3,3 1, 1 3))))")),
         ==,
         "GEOMETRYCOLLECTION(MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 "
         "3,3 1,1 3))))");

    TEST(util::geo::getWKT(collectionFromWKT<int>(
             "GEOMETRYCOLLECTION (MULTIPOLYGON (((1 1,3 3,1 1), (0 0,1 1,0 "
             "0)),((1 3,3 1, 1 3))))")),
         ==,
         "GEOMETRYCOLLECTION(MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 "
         "3,3 1,1 3))))");

    TEST(util::geo::getWKT(collectionFromWKT<int>(
             "GEOMETRYCOLLECTION (  MULTIPOLYGON Z (((1 1,3 3,1 1), (0 0,1 1,0 "
             "0)),((1 3,3 1, 1 3))))")),
         ==,
         "GEOMETRYCOLLECTION(MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 "
         "3,3 1,1 3))))");

    TEST(util::geo::getWKT(collectionFromWKT<int>(
             "GEOMETRYCOLLECTION (  MMULTIPOLYGON Z (((1 1,3 3,1 1), (0 0,1 "
             "1,0 0)),((1 3,3 1, 1 3))))")),
         ==,
         "GEOMETRYCOLLECTION(MULTIPOLYGON(((1 1,3 3,1 1),(0 0,1 1,0 0)),((1 "
         "3,3 1,1 3))))");
  }
  {
    TEST(angBetween(Point<int>{0, 0}, Point<int>{0, 1}, Point<int>{0, 1}) == 0);
    TEST(angBetween(Point<int>{0, 0}, Point<int>{0, 1}, Point<int>{1, 1}) > 0);
    TEST(angBetween(Point<int>{0, 0}, Point<int>{0, 1}, Point<int>{-1, 1}) < 0);
    TEST(angBetween(Point<int>{0, 0}, Point<int>{0, 1}, Point<int>{1, 0}) ==
         approx(M_PI / 2));
    TEST(angBetween(Point<int>{0, 0}, Point<int>{0, 1}, Point<int>{-1, 0}) ==
         approx(-M_PI / 2));
    TEST(angBetween(Point<int>{0, 0}, Point<int>{1, 0}, Point<int>{0, 1}) ==
         approx(-M_PI / 2));
  }

  {
    auto c = LineSegment<int>{{8830335, 62149579}, {8829909, 62149287}};
    auto d = LineSegment<int>{{8829947, 62149129}, {8829997, 62148988}};
    auto a = LineSegment<int>{{8829974, 62148972}, {8829997, 62148988}};
    auto b = LineSegment<int>{{8829997, 62148988}, {8830058, 62148937}};

    TEST(a > b);
    TEST(!(a < b));

    TEST(b < c);
    TEST(!(b > c));

    TEST(a < c);
    TEST(!(a > c));

    TEST(a < d);
    TEST(!(a > d));

    TEST(d < c);
    TEST(!(d > c));
  }

  {
    util::geo::DE9IMatrix de9im1 = "FFFFFFFF2";
    util::geo::DE9IMatrix de9im2 = "FFFFFFFF2";
    util::geo::DE9IMatrix de9im3 = "F1FFFFFF2";

    TEST(de9im1, ==, de9im2);
    TEST(de9im1, !=, de9im3);

    TEST(de9im1.toString(), ==, "FFFFFFFF2");
    TEST(de9im2.toString(), ==, "FFFFFFFF2");
    TEST(de9im3.toString(), ==, "F1FFFFFF2");

    de9im3.setTo(2, D2);
    TEST(de9im3.toString(), ==, "F12FFFFF2");

    de9im3.setTo(7, D1);
    TEST(de9im3.toString(), ==, "F12FFFF12");

    de9im3.setTo(4, D0);
    TEST(de9im3.toString(), ==, "F12F0FF12");

    de9im3.II(D0);
    TEST(de9im3.II(), ==, D0);
    TEST(de9im3.toString(), ==, "012F0FF12");

    de9im3.BB(D2);
    TEST(de9im3.BB(), ==, D2);
    TEST(de9im3.toString(), ==, "012F2FF12");

    util::geo::DE9IMatrix de9im4 = "FFFFFFFF2";
    util::geo::DE9IMatrix de9im5 = "F0FFFFFF2";
    auto a = de9im4 + de9im5;
    TEST(a.toString(), ==, "F0FFFFFF2");

    util::geo::DE9IMatrix de9im6 = "000200002";

    auto c = de9im3 + de9im5;
    TEST(c.toString(), ==, "012F2FF12");

    auto d = de9im3 + de9im5 + de9im6;
    TEST(d.toString(), ==, "012220012");
  }

  {
    util::geo::DE9IMFilter f1 = "F12******";
    util::geo::DE9IMFilter f2 = "012******";
    util::geo::DE9IMFilter f3 = "012*****1";
    util::geo::DE9IMFilter f4 = "012**T**T";
    util::geo::DE9IMFilter f5 = "012TTTTTT";
    util::geo::DE9IMFilter f6 = "TTTTTTTTT";
    util::geo::DE9IMatrix de9im1 = "F12222222";
    util::geo::DE9IMatrix de9im2 = "012222222";

    TEST(de9im1 & f1);
    TEST(!(de9im2 & f1));
    TEST(!(de9im1 & f2));
    TEST(!(de9im1 & f3));
    TEST(!(de9im2 & f3));
    TEST(!(de9im1 & f4));
    TEST(!(de9im1 & f5));
    TEST(!(de9im1 & f6));
    TEST(de9im2 & f6);
    TEST(de9im2 & f2);
    TEST(de9im2 & f4);
    TEST(de9im2 & f5);

    TEST(util::geo::DE9IMatrix("FFFF1FFF2") &
         util::geo::DE9IMFilter("****T****"));
    TEST(!(util::geo::DE9IMatrix("FFFF1FFF2") &
           util::geo::DE9IMFilter("***T*****")));
    TEST(!(util::geo::DE9IMatrix("FFFF1FFF2") &
           util::geo::DE9IMFilter("***TT*****")));
    TEST((util::geo::DE9IMatrix("FFFF1FFF2") &
          util::geo::DE9IMFilter("****T****T")));
    TEST((util::geo::DE9IMatrix("FFFF1FFF2") &
          util::geo::DE9IMFilter("****T***2")));

    TEST((util::geo::DE9IMatrix("2FFF1FFF2") &
          util::geo::DE9IMFilter("T***T***2")));
    TEST(!(util::geo::DE9IMatrix("2FFF1FFF2") &
           util::geo::DE9IMFilter("TT**T***2")));
    TEST((util::geo::DE9IMatrix("20FF1FFF2") &
          util::geo::DE9IMFilter("TT**T***2")));
    TEST(!(util::geo::DE9IMatrix("20FF1F1F2") &
           util::geo::DE9IMFilter("TT**T*2*2")));
    TEST(!(util::geo::DE9IMatrix("20FF1F1F2") &
           util::geo::DE9IMFilter("TT**T*0*2")));
    TEST((util::geo::DE9IMatrix("20FF1F1F2") &
          util::geo::DE9IMFilter("TT**T*T*2")));
    TEST((util::geo::DE9IMatrix("20FF1F1F2") &
          util::geo::DE9IMFilter("TT**T*1*2")));

    TEST((util::geo::DE9IMatrix("20FF1F1F2") &
          util::geo::DE9IMFilter("20FF1F1F2")));
    TEST((util::geo::DE9IMatrix("20FF1F1F2") &
          util::geo::DE9IMFilter("2*FF1F1F2")));
    TEST((util::geo::DE9IMatrix("20FF1F1F2") &
          util::geo::DE9IMFilter("2*FF1F1*2")));

    TEST(!(util::geo::DE9IMatrix("20FF1F1F2") &
           util::geo::DE9IMFilter("20FF0F1F2")));
    TEST((util::geo::DE9IMatrix("222222222") &
          util::geo::DE9IMFilter("222222222")));
    TEST((util::geo::DE9IMatrix("222222222") &
          util::geo::DE9IMFilter("*********")));
    TEST((util::geo::DE9IMatrix("222222222") &
          util::geo::DE9IMFilter("TTTTTTTTT")));
    TEST((util::geo::DE9IMatrix("111111111") &
          util::geo::DE9IMFilter("TTTTTTTTT")));
    TEST((util::geo::DE9IMatrix("000000000") &
          util::geo::DE9IMFilter("TTTTTTTTT")));
    TEST(!(util::geo::DE9IMatrix("0000F0000") &
           util::geo::DE9IMFilter("TTTTTTTTT")));
    TEST(!(util::geo::DE9IMatrix("0F0F0F0F0") &
           util::geo::DE9IMFilter("TTTTTTTTT")));
    TEST((util::geo::DE9IMatrix("0F0F0F0F0") &
          util::geo::DE9IMFilter("TFTFTFTFT")));
    TEST(!(util::geo::DE9IMatrix("0F0F0F0F0") &
           util::geo::DE9IMFilter("FTFTFTFTF")));
    TEST(!(util::geo::DE9IMatrix("1F1F1F1F1") &
           util::geo::DE9IMFilter("TTTTTTTTT")));
    TEST((util::geo::DE9IMatrix("1F1F1F1F1") &
          util::geo::DE9IMFilter("TFTFTFTFT")));
    TEST(!(util::geo::DE9IMatrix("1F1F1F1F1") &
           util::geo::DE9IMFilter("FTFTFTFTF")));
    TEST(!(util::geo::DE9IMatrix("2F2F2F2F2") &
           util::geo::DE9IMFilter("TTTTTTTTT")));
    TEST((util::geo::DE9IMatrix("2F2F2F2F2") &
          util::geo::DE9IMFilter("TFTFTFTFT")));
    TEST(!(util::geo::DE9IMatrix("2F2F2F2F2") &
           util::geo::DE9IMFilter("FTFTFTFTF")));

    TEST(util::geo::DE9IMFilter("FTFTFTFT*").toString(), ==, "FTFTFTFT*");
    TEST(util::geo::DE9IMFilter("FTFTFTFT2").toString(), ==, "FTFTFTFT2");
    TEST(util::geo::DE9IMFilter("FTFTFTFT1").toString(), ==, "FTFTFTFT1");
    TEST(util::geo::DE9IMFilter("FTFTFTFT0").toString(), ==, "FTFTFTFT0");
    TEST(util::geo::DE9IMFilter("TTTTTTTTT").toString(), ==, "TTTTTTTTT");
    TEST(util::geo::DE9IMFilter("*********").toString(), ==, "*********");
    TEST(util::geo::DE9IMFilter("****1****").toString(), ==, "****1****");
    TEST(util::geo::DE9IMFilter("T*T*T*T*T").toString(), ==, "T*T*T*T*T");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 1, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(0 0, 4 4)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<1>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FF0FF102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "101FF0FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 1, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 1 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<1>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FFF0FFF2");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1FFF0FFF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 1 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F01FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 3 3)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F00102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF1F00102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 3 3, 1 3, 1 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("F01F001F2");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 3 3, 1 3, 1 1, 2 0)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<4>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<4>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("0F1F00102");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(1 2, 1 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("F01FF0102");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(0 0, 1 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("FF1F00102");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 1 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("FF1F00102");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(0 0, 1 1, 1 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("1F1F00102");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(0 0, 1 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));

    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));
    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("101F00FF2");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));
    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("101FF0FF2");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3, 0 3)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<3>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<3>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("101FF0102");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<3>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("1010F0102");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 3 3)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("101F00FF2");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3, 5 5)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("101FF0FF2");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3, 5 5, 5 6)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 5 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("101F00FF2");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3, 5 5, 5 7)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 5 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    DE9IMatrix m("101FF0FF2");
    TEST(de9im, ==, m);

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, m.transpose());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3, 5 5, 5 7)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<3>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "101FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1F10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 3 3, 5 5, 6 6)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 5 7)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<3>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "101FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1F10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<1>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FFF0FFF2");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1FFF0FFF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 4 4, 5 5, 6 6)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "101F00FF2");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1FF00F102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(2 0, 1 1)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));
    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F00102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF1F00102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(7 300, 6 6)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F00102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF1F00102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(10 0, 5 5)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF10F0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "F01FF0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(5 5, 10 0)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 6 6)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF10F0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "F01FF0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(6 6, 10 0)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 7 7)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF10F0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "F01FF0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(10 0, 6 6)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 4 4, 5 5, 7 7)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF10F0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "F01FF0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 2 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F01FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 0 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F01FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 1 1, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 0 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F01FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 1 1, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 2 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(!std::get<1>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F01FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 4 4)");
    auto b = lineFromWKT<int>("LINESTRING(1 1, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<0>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));
    TEST(std::get<1>(geo::intersectsCovers(bx, ax, util::geo::getBoundingBox(b),
                                           util::geo::getBoundingBox(a))));

    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    TEST(!std::get<4>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsCovers(
        bx, ax, util::geo::getBoundingBox(b), util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "101FF0FF2");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1FF0FF102");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 2 0, 2 2, 0 2, 0 0))");
    auto aa =
        polygonFromWKT<int>("POLYGON((0 0, 2 0, 2 1, 1 1, 1 3, 0 3, 0 0))");
    auto b = pointFromWKT<int>("POINT(0 1)");
    auto c = pointFromWKT<int>("POINT(1 1)");
    auto d = pointFromWKT<int>("POINT(0 0)");
    auto e = pointFromWKT<int>("POINT(2 2)");
    auto f = pointFromWKT<int>("POINT(2 0)");
    auto g = pointFromWKT<int>("POINT(2 3)");
    auto h = pointFromWKT<int>("POINT(2 -1)");
    auto i = pointFromWKT<int>("POINT(0 3)");
    auto j = pointFromWKT<int>("POINT(2 3)");

    auto bb = pointFromWKT<int>("POINT(1 4)");
    auto cc = pointFromWKT<int>("POINT(1 3)");

    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> aax(aa);

    TEST(geo::contains(c, a));
    TEST(geo::contains(b, a));

    TEST(!geo::containsCovers(b, ax).first);
    TEST(geo::containsCovers(b, ax).second);

    TEST(!geo::containsCovers(d, ax).first);
    TEST(geo::containsCovers(d, ax).second);

    TEST(!geo::containsCovers(e, ax).first);
    TEST(geo::containsCovers(e, ax).second);

    TEST(!geo::containsCovers(f, ax).first);
    TEST(geo::containsCovers(f, ax).second);

    TEST(geo::containsCovers(c, ax).first);
    TEST(geo::containsCovers(c, ax).second);

    TEST(!geo::containsCovers(g, ax).first);
    TEST(!geo::containsCovers(g, ax).second);

    TEST(!geo::containsCovers(h, ax).first);
    TEST(!geo::containsCovers(h, ax).second);

    TEST(!geo::containsCovers(i, ax).first);
    TEST(!geo::containsCovers(i, ax).second);

    TEST(!geo::containsCovers(j, ax).first);
    TEST(!geo::containsCovers(j, ax).second);

    TEST(!geo::containsCovers(bb, aax).first);
    TEST(!geo::containsCovers(bb, aax).second);

    TEST(!geo::containsCovers(cc, aax).first);
    TEST(geo::containsCovers(cc, aax).second);
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))");
    XSortedPolygon<int> ax(a);

    for (size_t i = 0; i < ax.getOuter().rawRing().size(); i++) {
      auto c = ax.getOuter().rawRing()[i];
      if (util::geo::dist(c.seg().first, c.seg().second) > 0) {
        TEST(fabs(c.nextAng() - (-M_PI / 2)) < 0.001);
      }
    }
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 0, 1 1, 0 1, 0 0))");
    XSortedPolygon<int> ax(a);

    for (size_t i = 0; i < ax.getOuter().rawRing().size(); i++) {
      auto c = ax.getOuter().rawRing()[i];
      TEST(fabs(c.nextAng() - (-M_PI / 2)) < 0.001);
    }
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 0 0, 1 0, 1 1, 0 1, 0 0))");
    XSortedPolygon<int> ax(a);

    for (size_t i = 0; i < ax.getOuter().rawRing().size(); i++) {
      auto c = ax.getOuter().rawRing()[i];
      TEST(fabs(c.nextAng() - (-M_PI / 2)) < 0.001);
    }
  }

  {
    auto a = polygonFromWKT<int>(
        "POLYGON((0 0, 0 0, 1 0, 1 0, 1 0, 1 1, 1 1, 1 1, 1 1, 0 1, 0 0))");
    XSortedPolygon<int> ax(a);

    for (size_t i = 0; i < ax.getOuter().rawRing().size(); i++) {
      auto c = ax.getOuter().rawRing()[i];
      TEST(fabs(c.nextAng() - (-M_PI / 2)) < 0.001);
    }
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))");
    XSortedPolygon<int> ax(util::geo::getBoundingBox(a));

    for (size_t i = 0; i < ax.getOuter().rawRing().size(); i++) {
      auto c = ax.getOuter().rawRing()[i];
      TEST(fabs(c.nextAng() - (-M_PI / 2)) < 0.001);
    }
  }

  {
    // wrong polygon direction, should be corrected automatically
    auto a = polygonFromWKT<int>(
        "POLYGON((0 0, 1 0, 1 1, 0 1, 0 0), (0 0, 0 1, 1 1, 1 0, 0 0))");
    XSortedPolygon<int> ax(a);

    for (size_t i = 0; i < ax.getOuter().rawRing().size(); i++) {
      auto c = ax.getOuter().rawRing()[i];
      TEST(fabs(c.nextAng() - (-M_PI / 2)) < 0.001);
    }
    for (size_t i = 0; i < ax.getInners()[0].rawRing().size(); i++) {
      auto c = ax.getInners()[0].rawRing()[i];
      TEST(fabs(c.nextAng() - (M_PI / 2)) < 0.001);
    }
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b = lineFromWKT<int>("LINESTRING(1 3, 2 3, 3 4, 1 4, 1 3)");
    XSortedPolygon<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(signedRingArea(a.getOuter()) < 0);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));

    TEST(std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));

    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1F2101FF2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.transpose().within());
    TEST(de9im.covers());
    TEST(de9im.transpose().covered());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b = lineFromWKT<int>("LINESTRING(0 0, -1 0)");
    XSortedPolygon<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));

    TEST(!std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));

    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), ax, util::geo::getBoundingBox(a))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF2F01102");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.transpose().intersects());
    TEST(!de9im.contains());
    TEST(!de9im.transpose().within());
    TEST(!de9im.covers());
    TEST(!de9im.transpose().covered());
    TEST(de9im.touches());
    TEST(de9im.transpose().touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.transpose().disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((1 3, 2 3, 3 4, 1 4, 1 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<2>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212F11FF2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2FF11F212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((1 2, 2 2, 2 3, 1 3, 1 2))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<2>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F11212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F11212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((1 2, 2 1, 2 2, 1 2))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F01212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F01212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((1 3, 2 1, 2 2, 1 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F01212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F01212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((1 3, 2 4, 1 4, 1 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212F01FF2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2FF10F212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((1 3, 3 3, 3 5, 0 5, 0 0, 1 0, 1 3))");
    auto b = polygonFromWKT<int>("POLYGON((1 3, 2 1, 2 2, 1 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F01212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F01212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    auto b =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 0))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FFF1FFF2");

    TEST(de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2FFF1FFF2");

    TEST(de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());
  }

  {
    auto a =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 4, 0 0))");
    auto b =
        polygonFromWKT<int>("POLYGON((0 0, 1 0, 1 3, 2 3, 3 5, 0 4, 0 0))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212F11FF2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2FF11F212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());
  }

  {
    auto a = polygonFromWKT<int>(
        "POLYGON((0 0, 1 0, 1 3, 2 3, 3 3, 3 4, 3 5, 1 5, 0 5, 0 0))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 1 0, 1 3, 3 3, 3 5, 0 5, 0 3, 0 0))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    TEST(std::get<0>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(!std::get<1>(util::geo::intersectsPoly(
        bx.getOuter(), ax.getOuter(), bx.getOuter().getMaxSegLen(),
        ax.getOuter().getMaxSegLen(), util::geo::getBoundingBox(b),
        util::geo::getBoundingBox(a), 0, 0)));

    TEST(std::get<1>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FFF1FFF2");

    TEST(de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2FFF1FFF2");

    TEST(de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((7 5, 8 5, 8 6, 7 6, 7 5))");
    auto b = polygonFromWKT<int>("POLYGON((0 0, 7 0, 7 7, 0 7, 0 0))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F11212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F11212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((1 1, 6 1, 6 6, 1 6, 1 1))");
    auto b = polygonFromWKT<int>("POLYGON((0 0, 7 0, 7 7, 0 7, 0 0))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FF1FF212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212FF1FF2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((1 1, 6 1, 6 6, 1 6, 1 1))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2121FF212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2121F12F2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((1 1, 6 1, 6 6, 1 6, 1 1))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (1 1, 1 4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "21211F212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2121112F2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((1 1, 6 1, 6 6, 1 6, 1 1))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (1 1, 6 1, 6 6, 1 6, 1 1))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F1F212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F112F2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (1 1, 6 1, 6 6, 1 6, 1 1))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);
    TEST(bx.getInnerAreas()[0], >, 0)

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F11212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F11212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 7 0, 7 7, 0 7, 0 0)");
    auto b = polygonFromWKT<int>("POLYGON((0 0, 7 0, 7 7, 0 7, 0 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F1FF0F2F2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(!de9im.within());

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF210FFF2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(de9im.covers());
    TEST(de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 7 0, 7 7, 0 7)");
    auto b = polygonFromWKT<int>("POLYGON((0 0, 7 0, 7 7, 0 7, 0 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    // auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
    // util::geo::getBoundingBox(b)); TEST(de9im, ==, "F1FF0F212");

    // TEST(!de9im.equals());
    // TEST(de9im.intersects());
    // TEST(!de9im.contains());
    // TEST(!de9im.covers());
    // TEST(de9im.touches());
    // TEST(!de9im.disjoint());
    // TEST(de9im.covered());
    // TEST(!de9im.within());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 1, 6 1, 6 6, 1 6, 1 1)");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 4, 4 4, 4 3, 3 3))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FF0FF212");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(!de9im.contains());
    TEST(!de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(de9im.covered());
    TEST(de9im.within());

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "102FF1FF2");

    TEST(!de9im.equals());
    TEST(de9im.intersects());
    TEST(de9im.contains());
    TEST(de9im.covers());
    TEST(!de9im.touches());
    TEST(!de9im.disjoint());
    TEST(!de9im.covered());
    TEST(!de9im.within());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 3, 3 4, 4 4, 4 3, 3 3)");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 4, 4 4, 4 3, 3 3))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F1FF0F212");

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF2101FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 4, 4 4)");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 5, 5 5, 5 3, 3 3))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F00212");

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF2F01102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 4, 3 5)");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 2, 3 6, 5 5, 5 3, 3 2))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F1FF0F212");

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF2101FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 4, 3 5, 4 5)");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 2, 3 6, 5 5, 5 3, 3 2))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F1FF00212");

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF2101F02");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 4, 3 5, 2 5)");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 2, 3 6, 5 5, 5 3, 3 2))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "11F00F212");

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "102101FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 1, 3 1)");
    auto b = polygonFromWKT<int>("POLYGON((1 0, 1 2, 2 2, 2 0, 1 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "101FF0212");

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1F20F1102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 1, 2 1)");
    auto b = polygonFromWKT<int>("POLYGON((1 0, 1 3, 3 3, 3 0, 1 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, util::geo::getBoundingBox(a), bx,
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1010F0212");

    de9im = geo::DE9IM(bx, util::geo::getBoundingBox(b), ax,
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1020F1102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 0, 3 3)");
    auto b = lineFromWKT<int>("LINESTRING(2 0, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(geo::intersects(util::geo::getBoundingBox(a),
                         util::geo::getBoundingBox(b)));
    TEST(geo::intersects(a, b));
    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<4>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "0F1FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "0F1FF0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 0, 2 1)");
    auto b = lineFromWKT<int>("LINESTRING(2 0, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF10F0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "F01FF0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 0, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F00102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF1F00102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(2 2, 1 0)");
    auto b = lineFromWKT<int>("LINESTRING(2 0, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F00102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF1F00102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(2 2, 1 0)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 2 0)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F00102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF1F00102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(2 1, 1 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 2 0)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF100F102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "F01F001F2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(2 0, 1 0, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(2 2, 2 0)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F0F1F2");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF1F0F1F2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 1 1)");
    auto b = lineFromWKT<int>("LINESTRING(0 0, 1 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FFF0FFF2");
    TEST(de9im.equals());

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1FFF0FFF2");
    TEST(de9im.equals());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 1 1, 2 2)");
    auto b = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FFF0FFF2");
    TEST(de9im.equals());

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1FFF0FFF2");
    TEST(de9im.equals());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(2 2, 1 1, 0 0)");
    auto b = lineFromWKT<int>("LINESTRING(0 0, 2 2)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FFF0FFF2");
    TEST(de9im.equals());

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "1FFF0FFF2");
    TEST(de9im.equals());
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 0 1, 1 1, 0 0)");
    auto b = lineFromWKT<int>("LINESTRING(0 1, 1 1, 0 0, 0 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "10F0FFFF2");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "10F0FFFF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 1 1, 2 1, 2 2, 1 1)");
    auto b = lineFromWKT<int>("LINESTRING(1 0, 1 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F01F00102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    // TEST(de9im, ==, "FF0000101");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 0, 1 1, 2 1, 2 2, 1 1, 1 2)");
    auto b = lineFromWKT<int>("LINESTRING(1 0, 1 1)");
    XSortedLine<int> ax(a);
    XSortedLine<int> bx(b);

    TEST(std::get<0>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsCovers(
        ax, bx, util::geo::getBoundingBox(a), util::geo::getBoundingBox(b))));
    TEST(std::get<2>(geo::intersectsCovers(ax, bx, util::geo::getBoundingBox(a),
                                           util::geo::getBoundingBox(b))));

    auto de9im = geo::DE9IM(ax, bx, util::geo::getBoundingBox(a),
                            util::geo::getBoundingBox(b));
    TEST(de9im, ==, "F01FF0102");

    de9im = geo::DE9IM(bx, ax, util::geo::getBoundingBox(b),
                       util::geo::getBoundingBox(a));
    TEST(de9im, ==, "FF10F0102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 2, 10 2)");
    auto b = polygonFromWKT<int>("POLYGON((1 0, 1 3, 3 3, 3 0, 1 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "101FF0212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1F20F1102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 0, 0 0, 0 2, 10 2)");
    auto b = polygonFromWKT<int>("POLYGON((1 0, 1 3, 3 3, 3 0, 1 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "101F00212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1F2001102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 1, 2 1)");
    auto b =
        polygonFromWKT<int>("POLYGON((-1 -1, -1 10, 10 10, 10 -1, -1 -1))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FF0FF212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "102FF1FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 1, 2 1)");
    auto b = polygonFromWKT<int>(
        "POLYGON((-1 -1, -1 10, 10 10, 10 -1, -1 -1), (1 0, 1 3, 3 3, 3 0, 1 "
        "0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), bx, util::geo::getBoundingBox(b))));

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1010F0212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1020F1102");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 1, 9 1)");
    auto b = polygonFromWKT<int>(
        "POLYGON((-1 -1, -1 10, 10 10, 10 -1, -1 -1), (1 0, 1 3, 3 3, 3 0, 1 "
        "0), (4 0, 4 3,7 3, 7 0, 4 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1010FF212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1020F11F2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 1, 4 1)");
    auto b = polygonFromWKT<int>(
        "POLYGON((-1 -1, -1 10, 10 10, 10 -1, -1 -1), (1 0, 1 3, 3 3, 3 0, 1 "
        "0), (4 0, 4 3,7 3, 7 0, 4 0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FFF0F212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1F2F01FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 1, 4 1)");
    auto b = polygonFromWKT<int>(
        "POLYGON((-1 -1, -1 10, 10 10, 10 -1, -1 -1), (1 0, 1 3, 3 3, 3 0, 1 "
        "0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FF00F212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "102F01FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(0 1, 1 1)");
    auto b = polygonFromWKT<int>(
        "POLYGON((-1 -1, -1 10, 10 10, 10 -1, -1 -1), (1 0, 1 3, 3 3, 3 0, 1 "
        "0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "1FF00F212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "102F01FF2");
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(3 1, 1 1)");
    auto b = polygonFromWKT<int>(
        "POLYGON((-1 -1, -1 10, 10 10, 10 -1, -1 -1), (1 0, 1 3, 3 3, 3 0, 1 "
        "0))");
    XSortedLine<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im = DE9IM(ax, util::geo::getBoundingBox(a), bx,
                       util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF1F0F212");

    de9im = DE9IM(bx, util::geo::getBoundingBox(b), ax,
                  util::geo::getBoundingBox(b));
    TEST(de9im, ==, "FF2F011F2");
  }

  {
    auto a = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), (3 3, 3 "
        "4, 4 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), (3 3, 3 "
        "4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FFF1FFF2");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2FFF1FFF2");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((3 3, 3 4, 4 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), (3 3, 3 "
        "4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F1F212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F112F2");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((3 3, 3 4, 4 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F1F212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F112F2");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((3 3, 3 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F11212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F11212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((1 3, 3 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212101212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212101212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((-1 3, 3 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212101212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212101212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((2 3, 3 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F01212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F01212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((2 3, 2 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F11212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F11212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 3, 0 4, 4 3, 3 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212111212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212111212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 3, 2 4, 2 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FF11F212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212F11FF2");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 3, 2 4, 2 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2), (1 5, 2 "
        "1, 2 1, 1 1)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212111212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212111212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((-1 3, 2 4, 2 3))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2)))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212111212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212111212");
  }

  {
    auto a = polygonFromWKT<int>(
        "POLYGON((1 1, 6 1, 6 6, 1 6, 1 1), (2 2, 2 5, 5 5, 5 2, 2 2))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FF1FF212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212FF1FF2");
  }

  {
    auto a = polygonFromWKT<int>(
        "POLYGON((1 1, 6 1, 6 6, 1 6, 1 1), (3 3, 2 5, 5 5, 5 2, 2 2))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (3 3, 3 4, 4 4, 4 3, 3 3))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FF10F212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212F01FF2");
  }

  {
    auto a = polygonFromWKT<int>(
        "POLYGON((1 1, 6 1, 6 6, 1 6, 1 1), (2 2, 2 5, 5 5, 5 2, 2 2))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "2FF11F212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212F11FF2");
  }

  {
    auto a = polygonFromWKT<int>(
        "POLYGON((1 1, 6 1, 6 6, 1 6, 1 1), (2 2, 3 3, 2 5, 5 5, 5 2, 2 2))");
    auto b = polygonFromWKT<int>(
        "POLYGON((0 0, 7 0, 7 7, 0 7, 0 0), (2 2, 2 5, 5 5, 5 2, 2 2))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "21211F212");

    de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "2121112F2");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 0 2, 2 2, 2 0, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((1 1, 1 3, 3 3, 3 1, 1 1))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "212101212");

    de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "212101212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((2 2, 2 3, 3 3, 3 2, 2 2))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    TEST(!std::get<0>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<1>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<2>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<3>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
        util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    TEST(!std::get<4>(geo::intersectsContainsCovers(
        bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
        util::geo::getBoundingBox(a), util::geo::outerArea(a))));

    auto de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2FF1212");

    de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2FF1212");
  }

  {
    auto a = polygonFromWKT<int>("POLYGON((0 0, 0 2, 2 2, 2 0, 0 0))");
    auto b = polygonFromWKT<int>("POLYGON((2 2, 2 3, 3 3, 3 2, 2 2))");
    XSortedPolygon<int> ax(a);
    XSortedPolygon<int> bx(b);

    auto de9im =
        geo::DE9IM(bx, util::geo::getBoundingBox(b), util::geo::outerArea(b),
                   ax, util::geo::getBoundingBox(a), util::geo::outerArea(a));
    TEST(de9im, ==, "FF2F01212");

    de9im =
        geo::DE9IM(ax, util::geo::getBoundingBox(a), util::geo::outerArea(a),
                   bx, util::geo::getBoundingBox(b), util::geo::outerArea(b));
    TEST(de9im, ==, "FF2F01212");
  }

  // just to test that it does not crash
  auto a = polygonFromWKT<int>("POLYGON((0 0, 0 0, 0 0 0 0))");

  {
    TEST(LineSegment<int>({1, 1}, {2, 1}) < LineSegment<int>({1, 2}, {2, 2}));
    TEST(LineSegment<int>({10000, 10000}, {20000, 10000}) <
         LineSegment<int>({10000, 20000}, {20000, 20000}));
    TEST(LineSegment<double>({1000000, 1000000}, {2000000, 1000000}) <
         LineSegment<double>({1000000, 2000000}, {2000000, 2000000}));
    TEST(LineSegment<int>({1000000, 1000000}, {2000000, 1000000}) <
         LineSegment<int>({1000000, 2000000}, {2000000, 2000000}));
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(8335603 60914025, 8736353 60914025)");
    auto b = lineFromWKT<int>("LINESTRING(8440562 60918151, 8441122 60913050)");

    auto c = lineFromWKT<int>("LINESTRING(8335603 60593425, 8736353 60593425)");

    TEST(LineSegment<int>(b[0], b[1]) > LineSegment<int>(a[0], a[1]));
    TEST(LineSegment<int>(b[0], b[1]) > LineSegment<int>(c[0], c[1]));

    TEST(!(LineSegment<int>(a[0], a[1]) > LineSegment<int>(b[0], b[1])));
    TEST(!(LineSegment<int>(c[0], c[1]) > LineSegment<int>(b[0], b[1])));

    TEST(LineSegment<int>(c[0], c[1]) < LineSegment<int>(a[0], a[1]));

    TEST(LineSegment<int>(a[0], a[1]) < LineSegment<int>(b[0], b[1]));
    TEST(LineSegment<int>(b[0], b[1]) > LineSegment<int>(a[0], a[1]));

    TEST(
        intersects(LineSegment<int>(a[0], a[1]), LineSegment<int>(b[0], b[1])));
    TEST(
        intersects(LineSegment<int>(a[1], a[0]), LineSegment<int>(b[0], b[1])));
    TEST(
        intersects(LineSegment<int>(a[1], a[0]), LineSegment<int>(b[1], b[0])));
    TEST(
        intersects(LineSegment<int>(a[0], a[1]), LineSegment<int>(b[1], b[0])));

    TEST(
        intersects(LineSegment<int>(b[0], b[1]), LineSegment<int>(a[0], a[1])));
    TEST(
        intersects(LineSegment<int>(b[1], b[0]), LineSegment<int>(a[0], a[1])));
    TEST(
        intersects(LineSegment<int>(b[1], b[0]), LineSegment<int>(a[1], a[0])));
    TEST(
        intersects(LineSegment<int>(b[0], b[1]), LineSegment<int>(a[1], a[0])));
  }

  {
    auto a = lineFromWKT<int>("LINESTRING(1 1, 2 1, 3 1)");
    auto b = lineFromWKT<int>("LINESTRING(2 0, 2 1, 2 2)");
    auto c = lineFromWKT<int>("LINESTRING(1 0, 2 1, 3 2)");

    TEST(intersects(a, b));
    TEST(intersects(a, c));
  }

  {
    TEST(intersects(LineSegment<double>{{2, 1}, {3, 1}},
                    LineSegment<double>{{2, 0}, {2, 1}}));
    TEST(intersects(LineSegment<double>{{2, 0}, {2, 1}},
                    LineSegment<double>{{2, 1}, {3, 1}}));

    auto ls1 = LineSegment<int>{{2, 0}, {2, 1}};
    auto ls2 = LineSegment<int>{{2, 1}, {2, 2}};
    TEST(intersects(ls1, ls2));

    {
      auto ls1 = LineSegment<int>{{2, 0}, {2, 2}};
      auto ls2 = LineSegment<int>{{2, 1}, {2, 2}};
      TEST(!intersects(ls1, ls2));
      TEST(!intersects(ls2, ls1));
    }

    {
      auto ls1 = LineSegment<int>{{2, 2}, {2, 0}};
      auto ls2 = LineSegment<int>{{2, 1}, {2, 2}};
      TEST(!intersects(ls1, ls2));
      TEST(!intersects(ls2, ls1));
    }

    {
      auto ls1 = LineSegment<int>{{2, 2}, {2, 0}};
      auto ls2 = LineSegment<int>{{2, 2}, {2, 1}};
      TEST(!intersects(ls1, ls2));
      TEST(!intersects(ls2, ls1));
    }

    {
      auto ls1 = LineSegment<int>{{2, 0}, {2, 2}};
      auto ls2 = LineSegment<int>{{2, 2}, {2, 1}};
      TEST(!intersects(ls1, ls2));
      TEST(!intersects(ls2, ls1));
    }

    {
      auto ls1 = LineSegment<int>{{2, 0}, {2, 2}};
      auto ls2 = LineSegment<int>{{2, 2}, {2, 1}};
      TEST(!intersects(ls1, ls2));
      TEST(!intersects(ls2, ls1));
    }
  }

  {
    auto a = lineFromWKT<double>(
        "LINESTRING(5.816117 51.1096889,5.8162167 51.1091479)");
    auto b = lineFromWKT<double>(
        "LINESTRING(5.8099984 51.1096468,5.8288865 51.1070398)");

    TEST(intersects(getBoundingBox(a), getBoundingBox(b)));

    auto ls1 = LineSegment<double>{a[0], a[1]};
    auto ls2 = LineSegment<double>{b[0], b[1]};

    TEST(!contains(ls1.first, ls2));
    TEST(!contains(ls1.second, ls2));
    TEST(!contains(ls2.first, ls1));
    TEST(!contains(ls2.second, ls1));

    TEST(!(
        ((crossProd(ls1.first, ls2) < 0) ^ (crossProd(ls1.second, ls2) < 0)) &&
        ((crossProd(ls2.first, ls1) < 0) ^ (crossProd(ls2.second, ls1) < 0))));

    TEST(!geo::intersects(ls1, ls2));

    TEST(!geo::intersects(a, b));
    TEST(!geo::intersects(b, a));
  }

  // x sorted polygons
  // ___________________________________________________________________________
  {
    Polygon<int> poly1({Point<int>(1, 1), Point<int>(3, 2), Point<int>(4, 3),
                        Point<int>(6, 3), Point<int>(5, 1), Point<int>(1, 1)});

    Polygon<int> poly2({Point<int>(6, 2), Point<int>(5, 4), Point<int>(7, 4),
                        Point<int>(6, 2)});

    Polygon<int> poly3({Point<int>(1, 3), Point<int>(1, 4), Point<int>(2, 4),
                        Point<int>(2, 3), Point<int>(1, 3)});

    Polygon<int> poly4({Point<int>(0, 2), Point<int>(0, 5), Point<int>(3, 5),
                        Point<int>(3, 2), Point<int>(0, 2)});

    Polygon<int> poly5(
        {Point<int>(-1, 5), Point<int>(4, 0), Point<int>(-1, 5)});

    LineSegment<int64_t> a{{9426978, 71128476}, {9432711, 71047835}};
    LineSegment<int64_t> b{{9377553, 71093079}, {9537853, 71093079}};
    LineSegment<int64_t> c{{9377553, 71253379}, {9537853, 71253379}};

    std::set<LineSegment<int64_t>> testSet{b, c};

    TEST(b < a);
    TEST(!(b > a));
    TEST(a > b);
    TEST(!(a < b));

    TEST(a < c);
    TEST(!(c < a));
    TEST(!(a > c));
    TEST(c > a);
    TEST(!(c < a));

    TEST(*testSet.lower_bound(a) == c);

    XSortedPolygon<int> spoly1(poly1);
    XSortedPolygon<int> spoly2(poly2);
    XSortedPolygon<int> spoly3(poly3);
    XSortedPolygon<int> spoly4(poly4);
    XSortedPolygon<int> spoly5(poly5);

    // point contains tests
    TEST(geo::containsCovers(Point<int>(4, 2), spoly1).first);
    TEST(!geo::containsCovers(Point<int>(3, 3), spoly1).first);

    TEST(geo::containsCovers(Point<int>(6, 3), spoly2).first);
    TEST(!geo::containsCovers(Point<int>(5, 2), spoly2).first);

    auto ap = multiPolygonFromWKT<double>(
        "MULTIPOLYGON(((6.2120237 51.5133909,6.2127637 51.4913082,6.2190321 "
        "51.4849833,6.2236759 51.4687148,6.2205935 51.4466463,6.2139367 "
        "51.4462637,6.2147615 51.4338807,6.2052570 51.3995201,6.2266681 "
        "51.4002680,6.2144248 51.3896343,6.2263867 51.3603360,6.1898779 "
        "51.3394613,6.1942347 51.3348784,6.1685573 51.3329887,6.1693785 "
        "51.3293737,6.1595899 51.3196913,6.1538213 51.3074476,6.1290121 "
        "51.2857005,6.1244995 51.2747284,6.0856172 51.2476261,6.0726534 "
        "51.2425555,6.0860218 51.2226673,6.0679863 51.2205478,6.0731453 "
        "51.1827944,6.0821902 51.1716335,6.1000891 51.1698870,6.1224059 "
        "51.1813028,6.1651704 51.1944141,6.1807243 51.1863585,6.1388017 "
        "51.1733392,6.1754219 51.1584640,6.1628485 51.1526712,6.1633395 "
        "51.1486975,6.1258299 51.1451168,6.1163882 51.1391547,6.0919960 "
        "51.1350476,6.0871032 51.1293025,6.0887723 51.1278975,6.0844241 "
        "51.1260187,6.0869921 51.1245949,6.0809012 51.1259794,6.0805510 "
        "51.1221417,6.0758834 51.1191919,6.0602933 51.1159264,6.0552379 "
        "51.1107423,6.0568320 51.1095496,6.0366919 51.0965942,6.0206756 "
        "51.0928383,6.0175749 51.0945726,5.9977935 51.0842457,5.9884518 "
        "51.0746139,5.9802940 51.0723171,5.9817297 51.0694779,5.9699784 "
        "51.0607039,5.9691978 51.0467722,5.9578301 51.0409811,5.9577783 "
        "51.0347261,5.9498013 51.0369672,5.9381295 51.0351172,5.9263166 "
        "51.0482309,5.9187317 51.0639528,5.9132078 51.0668830,5.8920004 "
        "51.0531278,5.8667370 51.0515704,5.8672852 51.0462799,5.8780282 "
        "51.0375835,5.8770005 51.0320418,5.8567106 51.0283384,5.8526142 "
        "51.0293181,5.8496511 51.0352277,5.8527429 51.0381243,5.8485262 "
        "51.0463281,5.8272517 51.0475237,5.8239605 51.0725105,5.8196232 "
        "51.0726099,5.8069344 51.0575720,5.7993389 51.0600718,5.8010586 "
        "51.0640196,5.7966202 51.0718244,5.8049752 51.0787873,5.7958236 "
        "51.0878501,5.7960227 51.0914785,5.8082580 51.0963390,5.8241431 "
        "51.0923501,5.8336723 51.1000264,5.8334053 51.1037509,5.8288865 "
        "51.1070398,5.8099984 51.1096468,5.8078497 51.1135931,5.8098503 "
        "51.1184222,5.8248993 51.1293451,5.8413514 51.1314787,5.8458406 "
        "51.1407588,5.8557834 51.1446260,5.8363544 51.1536850,5.8386107 "
        "51.1570866,5.8328812 51.1590374,5.8326946 51.1624299,5.8253200 "
        "51.1674270,5.8159712 51.1631708,5.8150566 51.1588482,5.8049355 "
        "51.1628762,5.7987806 51.1576670,5.7776462 51.1513039,5.7747592 "
        "51.1546822,5.7794826 51.1593831,5.7794024 51.1633163,5.7701886 "
        "51.1642239,5.7695650 51.1690833,5.7797360 51.1717992,5.7729522 "
        "51.1733698,5.7767286 51.1784855,5.7671605 51.1836911,5.7559633 "
        "51.1844607,5.7456413 51.1894990,5.7397405 51.1847477,5.7196764 "
        "51.1846996,5.7093393 51.1804207,5.6894115 51.1854196,5.6765411 "
        "51.1827256,5.6710369 51.1857682,5.6580417 51.1847425,5.6497400 "
        "51.1936177,5.6540832 51.1942417,5.6527338 51.1976610,5.5660454 "
        "51.2209094,5.6187816 51.2294253,5.6259723 51.2736016,5.6720479 "
        "51.3150820,5.8745432 51.3533127,5.9312876 51.3847527,5.8716953 "
        "51.4501120,5.8607237 51.4919665,5.8525522 51.5041766,5.8382385 "
        "51.5664146,5.8914662 51.5602047,5.9066627 51.5520309,5.9354450 "
        "51.5536002,6.0042529 51.5702435,6.0316689 51.5523388,6.0343207 "
        "51.5574973,6.0481208 51.5584625,6.0376590 51.5699586,6.0385724 "
        "51.5842855,6.0249435 51.5980637,6.0239180 51.6157805,6.0206301 "
        "51.6212914,5.9963523 51.6367191,5.9739312 51.6446108,5.9651055 "
        "51.6522503,5.9643928 51.6773720,5.9552911 51.7093049,5.9412963 "
        "51.7147757,5.8995668 51.7201899,5.8876474 51.7253981,5.8795605 "
        "51.7499136,5.8644689 51.7576817,5.8692448 51.7628605,5.8678933 "
        "51.7755210,5.8934093 51.7778529,5.9111146 51.7624057,5.9152848 "
        "51.7522869,5.9333232 51.7480986,5.9299483 51.7444285,5.9327669 "
        "51.7419384,5.9461343 51.7423614,5.9524661 51.7445538,5.9532879 "
        "51.7480242,5.9522944 51.7426841,5.9551552 51.7381176,5.9941968 "
        "51.7383094,6.0295222 51.7254848,6.0354244 51.7177743,6.0378809 "
        "51.7199298,6.0449392 51.7169134,6.0420092 51.7133446,6.0315200 "
        "51.7129896,6.0260539 51.7086881,6.0317806 51.6925333,6.0282478 "
        "51.6896244,6.0322344 51.6847963,6.0298231 51.6780994,6.0346725 "
        "51.6751459,6.0315405 51.6745827,6.0757244 51.6648257,6.0795451 "
        "51.6615933,6.0853449 51.6629141,6.0878884 51.6598498,6.0996545 "
        "51.6581159,6.1028380 51.6605047,6.1180876 51.6559729,6.1172476 "
        "51.6507311,6.1094122 51.6468665,6.1116826 51.6447300,6.1000122 "
        "51.6240785,6.0972292 51.6208835,6.0939341 51.6221540,6.0914239 "
        "51.6058486,6.1214855 51.5927445,6.1305600 51.5810876,6.1570325 "
        "51.5665755,6.1769020 51.5385557,6.1999290 51.5273814,6.2120237 "
        "51.5133909)))");

    auto bp = multiPolygonFromWKT<double>(
        "MULTIPOLYGON(((5.8161104 51.1097054,5.8161127 51.1097311,5.8161193 "
        "51.1097563,5.8161651 51.1097780,5.8163299 51.1098021,5.8173204 "
        "51.1099397,5.8184653 51.1100911,5.8184926 51.1100910,5.8185151 "
        "51.1100881,5.8185332 51.1100765,5.8186024 51.1099088,5.8186329 "
        "51.1098348,5.8186576 51.1098019,5.8186686 51.1097819,5.8186956 "
        "51.1097633,5.8187296 51.1097502,5.8187565 51.1097473,5.8187974 "
        "51.1097429,5.8188698 51.1097352,5.8189396 51.1097220,5.8189779 "
        "51.1097233,5.8190199 51.1097237,5.8190595 51.1097244,5.8192236 "
        "51.1097277,5.8199365 51.1097572,5.8200295 51.1097684,5.8200863 "
        "51.1097881,5.8201750 51.1098335,5.8202380 51.1098656,5.8203005 "
        "51.1099025,5.8203312 51.1099198,5.8203496 51.1099256,5.8203790 "
        "51.1099283,5.8213954 51.1099088,5.8228084 51.1098864,5.8228694 "
        "51.1098760,5.8230269 51.1098536,5.8231398 51.1098742,5.8231083 "
        "51.1093755,5.8230140 51.1090536,5.8228888 51.1090689,5.8227712 "
        "51.1090494,5.8227237 51.1090052,5.8226520 51.1088723,5.8226199 "
        "51.1088084,5.8225177 51.1087594,5.8213256 51.1087121,5.8195553 "
        "51.1086471,5.8194630 51.1086528,5.8178802 51.1086039,5.8164549 "
        "51.1085495,5.8163851 51.1085470,5.8163390 51.1085662,5.8163165 "
        "51.1085756,5.8163054 51.1085949,5.8163005 51.1086109,5.8162167 "
        "51.1091479,5.8161170 51.1096889,5.8161104 51.1097054)))");

    Polygon<int> api;
    for (const auto& p : ap[0].getOuter()) {
      auto pp = latLngToWebMerc(p);
      api.getOuter().push_back(
          {static_cast<int>(pp.getX() * 10), static_cast<int>(pp.getY() * 10)});
    }

    Polygon<int> bpi;
    for (const auto& p : bp[0].getOuter()) {
      auto pp = latLngToWebMerc(p);
      bpi.getOuter().push_back(
          {static_cast<int>(pp.getX() * 10), static_cast<int>(pp.getY() * 10)});
    }

    XSortedPolygon<double> app(ap[0]);
    XSortedPolygon<double> bpp(bp[0]);

    TEST(geo::intersects(bp[0].getOuter(), ap[0].getOuter()));

    TEST(geo::intersects(ap[0].getOuter(), bp[0].getOuter()));

    XSortedPolygon<int> appi(api);
    XSortedPolygon<int> bppi(bpi);

    auto failtest =
        polygonFromWKT<int>("POLYGON((1 6, 3 1, 7 1, 8 8, 1 8, 1 6))");
    auto bboxfail = polygonFromWKT<int>("POLYGON((2 2, 6 2, 6 6, 2 6, 2 2))");

    XSortedPolygon<int> failtestx(failtest);
    XSortedPolygon<int> bboxfailx(bboxfail);

    TEST(geo::intersects(failtest.getOuter(), bboxfail.getOuter()));
    TEST(std::get<0>(geo::intersectsContainsCovers(failtestx, bboxfailx)));
    TEST(std::get<0>(geo::intersectsContainsCovers(bboxfailx, failtestx)));

    auto bremer = polygonFromWKT<int>(
        "POLYGON((1015809 7145218, 1012312 7144769, 1002806 7139896, 999939 "
        "7140594, 990566 7138626, 983285 7139256, 975967 7141358, 967081 "
        "7150512, 958806 7148906, 950876 7137498, 947207 7124657, 942697 "
        "7112847, 943271 7104783, 947659 7096463, 946968 7095405, 950408 "
        "7089285, 946959 7086008, 942172 7086314, 933754 7090164, 929359 "
        "7097636, 924945 7098974, 920492 7097197, 917320 7092782, 915594 "
        "7079870, 908884 7079883, 909849 7083046, 907594 7095477, 900084 "
        "7104114, 895517 7106541, 893739 7128746, 878146 7200635, 882580 "
        "7204925, 966909 7220260, 972527 7224139, 977476 7222917, 981986 "
        "7224770, 983763 7221751, 983075 7211566, 980973 7209253, 979769 "
        "7203807, 980438 7201476, 982941 7195781, 986018 7191864, 989572 "
        "7194272, 992248 7194826, 993757 7192896, 995095 7186647, 996968 "
        "7181660, 994522 7178870, 988177 7179768, 980935 7176424, 984126 "
        "7167805, 988216 7160200, 994178 7152327, 997770 7149613, 1000264 "
        "7149050, 1003321 7149289, 1005576 7150397, 1012656 7148429, 1015828 "
        "7148352, 1015809 7145218, 1015809 7145218))");
    auto bbox = polygonFromWKT<int>(
        "POLYGON((889665 7133352, 905695 7133352, 905695 7149382, 889665 "
        "7149382, 889665 7133352))");

    XSortedPolygon<int> bremerx(bremer);
    XSortedPolygon<int> bboxx(bbox);

    TEST(geo::intersects(bbox.getOuter(), bremer.getOuter()));
    TEST(geo::intersectsNaive(bremerx.getOuter().rawRing(),
                              bboxx.getOuter().rawRing()));
    TEST(std::get<0>(geo::intersectsContainsCovers(bremerx, bboxx)));
    TEST(std::get<0>(geo::intersectsContainsCovers(bboxx, bremerx)));

    auto wattbbox = polygonFromWKT<int>(
        "POLYGON((7614253 71173229, 7774553 71173229, 7774553 71333529, "
        "7614253 71333529, 7614253 71173229))");

    auto watt = polygonFromWKT<int>(
        "POLYGON((7325750 71235260, 7326028 71277936, 7791418 71378183, "
        "7875482 71367056, 8146422 71330500, 8146422 71221663, 7792364 "
        "71144836, 7792364 71346733, 7325750 71235260, 7325750 71235260))");

    XSortedPolygon<int> wattx(watt);
    XSortedPolygon<int> wattbboxx(wattbbox);

    TEST(geo::intersects(watt.getOuter(), wattbbox.getOuter()));
    TEST(std::get<0>(geo::intersectsContainsCovers(wattx, wattbboxx)));
    TEST(std::get<0>(geo::intersectsContainsCovers(wattbboxx, wattx)));

    TEST(std::get<0>(geo::intersectsContainsCovers(wattx, wattx)));
    TEST(std::get<0>(geo::intersectsContainsCovers(wattbboxx, wattbboxx)));

    auto wattbbox2 = polygonFromWKT<int>(
        "POLYGON((913710 7093277, 937755 7093277, 937755 7109307, 913710 "
        "7109307, 913710 7093277))");
    auto watt2 = polygonFromWKT<int>(
        "POLYGON((1015809 7145218, 1012312 7144769, 1002806 7139896, 999939 "
        "7140594, 990566 7138626, 983285 7139256, 975967 7141358, 967081 "
        "7150512, 958806 7148906, 950876 7137498, 947207 7124657, 942697 "
        "7112847, 943271 7104783, 947659 7096463, 946968 7095405, 950408 "
        "7089285, 946959 7086008, 942172 7086314, 933754 7090164, 929359 "
        "7097636, 924945 7098974, 920492 7097197, 917320 7092782, 915594 "
        "7079870, 908884 7079883, 909849 7083046, 907594 7095477, 900084 "
        "7104114, 895517 7106541, 893739 7128746, 878146 7200635, 882580 "
        "7204925, 966909 7220260, 972527 7224139, 977476 7222917, 981986 "
        "7224770, 983763 7221751, 983075 7211566, 980973 7209253, 979769 "
        "7203807, 980438 7201476, 982941 7195781, 986018 7191864, 989572 "
        "7194272, 992248 7194826, 993757 7192896, 995095 7186647, 996968 "
        "7181660, 994522 7178870, 988177 7179768, 980935 7176424, 984126 "
        "7167805, 988216 7160200, 994178 7152327, 997770 7149613, 1000264 "
        "7149050, 1003321 7149289, 1005576 7150397, 1012656 7148429, 1015828 "
        "7148352, 1015809 7145218, 1015809 7145218)) ");

    XSortedPolygon<int> wattx2(watt2);
    XSortedPolygon<int> wattbboxx2(wattbbox2);

    TEST(geo::intersects(watt2.getOuter(), wattbbox2.getOuter()));
    TEST(std::get<0>(geo::intersectsContainsCovers(wattx2, wattbboxx2)));
    TEST(std::get<0>(geo::intersectsContainsCovers(wattbboxx2, wattx2)));

    auto anothertest = polygonFromWKT<int64_t>(
        "POLYGON((15482959 73076173, 15596431 73176015, 15706928 73045744, "
        "15749740 73081874, 15870403 73277506, 16051803 73257507, 16026155 "
        "73024804, 16292312 72722538, 16419624 72654248, 16320974 72522587, "
        "16249995 72429299, 15974662 72019053, 15849195 71940131, 15849066 "
        "71940131, 15806390 72177589, 15768752 72203056, 15758857 72275088, "
        "15702653 72305851, 15709997 72540286, 15671882 72541478, 15609994 "
        "72860919, 15598659 72894904, 15582016 72940327, 15565146 72972142, "
        "15551031 72994672, 15532330 73020790, 15498495 73060139, 15482959 "
        "73076173, 15482959 73076173))");
    auto anotherbox = polygonFromWKT<int64_t>(
        "POLYGON((15949856 72696080, 16270456 72696080, 16270456 73176980, "
        "15949856 73176980, 15949856 72696080))");

    XSortedPolygon<int64_t> anothertestx(anothertest);
    XSortedPolygon<int64_t> anotherboxx(anotherbox);

    TEST(geo::intersects(anothertest.getOuter(), anotherbox.getOuter()));
    TEST(std::get<0>(geo::intersectsContainsCovers(anothertestx, anotherboxx)));
    TEST(std::get<0>(geo::intersectsContainsCovers(anotherboxx, anothertestx)));

    auto anothertest2 = polygonFromWKT<int64_t>(
        "POLYGON((10158098 71452187, 10123128 71447696, 10028060 71398968, "
        "9999396 71405943, 9905665 71386260, 9832859 71392566, 9759670 "
        "71413587, 9670812 71505120, 9588069 71489068, 9508765 71374986, "
        "9472076 71246572, 9426978 71128476, 9432711 71047835, 9476590 "
        "70964638, 9469687 70954056, 9504084 70892859, 9469592 70860087, "
        "9421723 70863144, 9337547 70901649, 9293595 70976367, 9249453 "
        "70989743, 9204928 70971972, 9173207 70927829, 9155949 70798708, "
        "9088840 70798834, 9098490 70830467, 9075941 70954773, 9000842 "
        "71041147, 8955170 71065416, 8937399 71287466, 8781467 72006356, "
        "8825801 72049256, 9669092 72202608, 9725274 72241399, 9774767 "
        "72229170, 9819864 72247706, 9837636 72217513, 9830757 72115661, "
        "9809736 72092538, 9797698 72038077, 9804386 72014764, 9829419 "
        "71957818, 9860185 71918644, 9895728 71942722, 9922481 71948263, "
        "9937577 71928963, 9950954 71866476, 9969681 71816601, 9945221 "
        "71788701, 9881778 71797683, 9809354 71764241, 9841266 71678059, "
        "9882161 71602004, 9941781 71523274, 9977707 71496139, 10002645 "
        "71490501, 10033219 71492890, 10055768 71503973, 10126568 71484291, "
        "10158289 71483526, 10158098 71452187, 10158098 71452187))");
    auto anotherbox2 = polygonFromWKT<int64_t>(
        "POLYGON((8976803 70932779, 9137103 70932779, 9137103 71093079, "
        "8976803 71093079, 8976803 70932779))");

    Box<int64_t> anotherbox2m{{8976803, 70932779}, {9137103, 71093079}};

    XSortedPolygon<int64_t> anothertestx2(anothertest2);
    XSortedPolygon<int64_t> anotherboxx2(anotherbox2);
    XSortedPolygon<int64_t> anotherboxx2m(anotherbox2m);
    XSortedPolygon<int64_t> anotherboxx2mp(Polygon<int64_t>{anotherbox2m});

    TEST(geo::intersects(anothertest2.getOuter(), anotherbox2.getOuter()));
    TEST(std::get<0>(
        geo::intersectsContainsCovers(anothertestx2, anotherboxx2)));
    TEST(std::get<0>(
        geo::intersectsContainsCovers(anotherboxx2, anothertestx2)));

    TEST(std::get<0>(
        geo::intersectsContainsCovers(anothertestx2, anotherboxx2m)));
    TEST(std::get<0>(
        geo::intersectsContainsCovers(anotherboxx2m, anothertestx2)));

    TEST(std::get<0>(
        geo::intersectsContainsCovers(anothertestx2, anotherboxx2mp)));
    TEST(std::get<0>(
        geo::intersectsContainsCovers(anotherboxx2mp, anothertestx2)));

    {
      auto anothertest2 = polygonFromWKT<int64_t>(
          "POLYGON((10158098 71452187, 10123128 71447696, 10028060 71398968, "
          "9999396 71405943, 9905665 71386260, 9832859 71392566, 9759670 "
          "71413587, 9670812 71505120, 9588069 71489068, 9508765 71374986, "
          "9472076 71246572,9426978 71128476, 9432711 71047835, 9476590 "
          "70964638, 9469687 70954056, 9504084 70892859, 9469592 70860087, "
          "9421723 70863144, 9337547 70901649, 9293595 70976367, 9249453 "
          "70989743, 9204928 70971972, 9173207 70927829, 9155949 70798708, "
          "9088840 70798834, 9098490 70830467, 9075941 70954773, 9000842 "
          "71041147, 8955170 71065416, 8937399 71287466, 8781467 72006356, "
          "8825801 72049256, 9669092 72202608, 9725274 72241399, 9774767 "
          "72229170, 9819864 72247706, 9837636 72217513, 9830757 72115661, "
          "9809736 72092538, 9797698 72038077, 9804386 72014764, 9829419 "
          "71957818, 9860185 71918644, 9895728 71942722, 9922481 71948263, "
          "9937577 71928963, 9950954 71866476, 9969681 71816601, 9945221 "
          "71788701, 9881778 71797683, 9809354 71764241, 9841266 71678059, "
          "9882161 71602004, 9941781 71523274, 9977707 71496139, 10002645 "
          "71490501, 10033219 71492890, 10055768 71503973, 10126568 71484291, "
          "10158289 71483526, 10158098 71452187, 10158098 71452187))");
      auto anotherbox2 = polygonFromWKT<int64_t>(
          "POLYGON((9377553 71093079, 9537853 71093079, 9537853 71253379, "
          "9377553 71253379, 9377553 71093079))");

      Box<int64_t> anotherbox2m{{8976803, 70932779}, {9137103, 71093079}};

      XSortedPolygon<int64_t> anothertestx2(anothertest2);
      XSortedPolygon<int64_t> anotherboxx2(anotherbox2);
      XSortedPolygon<int64_t> anotherboxx2m(anotherbox2m);
      XSortedPolygon<int64_t> anotherboxx2mp(Polygon<int64_t>{anotherbox2m});

      TEST(geo::intersects(anothertest2.getOuter(), anotherbox2.getOuter()));
      TEST(std::get<0>(
          geo::intersectsContainsCovers(anothertestx2, anotherboxx2)));
      TEST(std::get<0>(
          geo::intersectsContainsCovers(anotherboxx2, anothertestx2)));

      TEST(std::get<0>(
          geo::intersectsContainsCovers(anothertestx2, anotherboxx2m)));
      TEST(std::get<0>(
          geo::intersectsContainsCovers(anotherboxx2m, anothertestx2)));

      TEST(std::get<0>(
          geo::intersectsContainsCovers(anothertestx2, anotherboxx2mp)));
      TEST(std::get<0>(
          geo::intersectsContainsCovers(anotherboxx2mp, anothertestx2)));
    }

    {
      auto anothertest2 = polygonFromWKT<int64_t>(
          "POLYGON((8846056 67477080, 8846056 67496954, 8882746 67520649, "
          "8917142 67545873, 8921729 67570333, 8903384 67582563, 8879688 "
          "67591736, 8870516 67617724, 8882746 67636069, 8898033 67662822, "
          "8897804 67690470, 8875102 67756075, 8900326 67781299, 8946953 "
          "67789707, 8982878 67787414, 9037149 67769069, 9093712 67729322, "
          "9138810 67696454, 9191551 67696454, 9202595 67673506, 9210957 "
          "67647543, 9213769 67600456, 9215502 67564186, 9194609 67514534, "
          "9037149 67471730, 8983643 67462557, 8931666 67458735, 8863975 "
          "67457113, 8851706 67460758, 8846056 67477080, 8846056 67477080))");
      auto anotherbox2 = polygonFromWKT<int64_t>(
          "POLYGON((8896653 67486328, 9056953 67486328, 9056953 67646628, "
          "8896653 67646628, 8896653 67486328))");

      Box<int64_t> anotherbox2m{{8896653, 67486328}, {9056953, 67646628}};

      XSortedPolygon<int64_t> anothertestx2(anothertest2);
      XSortedPolygon<int64_t> anotherboxx2(anotherbox2);
      XSortedPolygon<int64_t> anotherboxx2m(anotherbox2m);
      XSortedPolygon<int64_t> anotherboxx2mp(Polygon<int64_t>{anotherbox2m});

      TEST(geo::intersects(
          LineSegment<int64_t>{{8896653, 67486328}, {8896653, 67646628}},
          LineSegment<int64_t>{{8903384, 67582563}, {8879688, 67591736}}));

      TEST(std::get<0>(
          geo::intersectsContainsCovers(anothertestx2, anotherboxx2)));
      TEST(std::get<0>(
          geo::intersectsContainsCovers(anotherboxx2, anothertestx2)));

      TEST(std::get<0>(
          geo::intersectsContainsCovers(anothertestx2, anotherboxx2m)));
      TEST(std::get<0>(
          geo::intersectsContainsCovers(anotherboxx2m, anothertestx2)));

      TEST(std::get<0>(
          geo::intersectsContainsCovers(anothertestx2, anotherboxx2mp)));
      TEST(std::get<0>(
          geo::intersectsContainsCovers(anotherboxx2mp, anothertestx2)));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
      auto b = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<1>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<2>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<0>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(std::get<1>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(std::get<2>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((0 0, 5 0, 5 5, 0 5, 0 0))");
      auto b = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<1>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<2>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((0 0, 5 0, 5 5, 0 5, 0 0))");
      auto b = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
      TEST(!std::get<1>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((4 3, 6 3, 6 4, 4 5, 4 4, 4 3))");
      auto b = polygonFromWKT<int>("POLYGON((3 5, 4 4, 5 5, 4 6, 3 5))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
      TEST(std::get<1>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((4 4, 5 4, 5 5, 4 5, 4 4))");
      auto aa = polygonFromWKT<int>("POLYGON((5 5, 6 5, 6 6, 5 6))");
      auto b = polygonFromWKT<int>("POLYGON((2 2, 6 2, 6 4, 4 6, 2 4, 2 2))");
      auto bHole = polygonFromWKT<int>(
          "POLYGON((2 2, 6 2, 6 4, 4 6, 2 4, 2 2), (4 4, 5 4, 5 3, 4 3, 4 4))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> aax(aa);
      XSortedPolygon<int> bx(b);
      XSortedPolygon<int> bHolex(bHole);

      TEST(std::get<0>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
      TEST(!std::get<1>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));

      TEST(std::get<0>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<1>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<2>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));

      TEST(std::get<0>(util::geo::intersectsPoly(
          aax.getOuter(), bx.getOuter(), aax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(aa),
          util::geo::getBoundingBox(b), 0, 0)));
      TEST(std::get<1>(util::geo::intersectsPoly(
          aax.getOuter(), bx.getOuter(), aax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(aa),
          util::geo::getBoundingBox(b), 0, 0)));

      TEST(bHole.getInners().size() == 1);

      TEST(!std::get<1>(util::geo::intersectsPoly(
          ax.getOuter(), bHolex.getInners().front(),
          ax.getOuter().getMaxSegLen(),
          bHolex.getInners().front().getMaxSegLen(),
          util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(bHole.getInners().front()), 0, 0)));

      TEST(std::get<0>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bHolex,
          util::geo::getBoundingBox(bHole), util::geo::outerArea(bHole))));
      TEST(std::get<1>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bHolex,
          util::geo::getBoundingBox(bHole), util::geo::outerArea(bHole))));
      TEST(std::get<2>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bHolex,
          util::geo::getBoundingBox(bHole), util::geo::outerArea(bHole))));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
      auto b = polygonFromWKT<int>("POLYGON((0 0, 5 0, 5 -5, 0 -5, 0 0))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
      TEST(std::get<1>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
      auto b = polygonFromWKT<int>("POLYGON((1 0, 5 0, 5 -5, 1 -5, 1 0))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));
      TEST(std::get<1>(util::geo::intersectsPoly(
          ax.getOuter(), bx.getOuter(), ax.getOuter().getMaxSegLen(),
          bx.getOuter().getMaxSegLen(), util::geo::getBoundingBox(a),
          util::geo::getBoundingBox(b), 0, 0)));

      TEST(std::get<0>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(!std::get<1>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(!std::get<2>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    }

    {
      auto a = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
      auto b = polygonFromWKT<int>("POLYGON((4 4, 5 4, 5 5, 4 5, 4 4))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(geo::contains(b, a));
      TEST(geo::intersects(a, b));
      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(std::get<0>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<0>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<1>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
    }

    {
      auto a = polygonFromWKT<int>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 9 1, 9 9, 1 9, 1 1))");
      auto b = polygonFromWKT<int>("POLYGON((4 4, 5 4, 5 5, 4 5, 4 4))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(!geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(!geo::intersects(a, b));
      TEST(!geo::intersects(b, a));
      auto res = intersectsContainsInner(bx.getOuter(), ax.getInners().front());
      TEST(std::get<1>(res));
      TEST(!std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<0>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(!std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<0>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    }

    {
      auto a = polygonFromWKT<int>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (4 4, 5 4, 5 5, 4 5, 4 4))");
      auto b = polygonFromWKT<int>(
          "POLYGON((1 1, 9 1, 9 9, 1 9, 1 1), (3 3, 6 3, 6 6, 3 6, 3 3))");

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(geo::intersects(a, b));
      TEST(geo::intersects(b, a));
      TEST(std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }

    {
      auto a = polygonFromWKT<double>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (4 4, 5 4, 4 5, 4 4))");
      auto b = polygonFromWKT<double>("POLYGON((4.1 4.1, 5 4, 5 5, 4 5, 4 4))");

      XSortedPolygon<double> ax(a);
      XSortedPolygon<double> bx(b);

      TEST(!geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(geo::intersects(a, b));
      TEST(geo::intersects(b, a));
      TEST(std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }

    {
      auto a = polygonFromWKT<double>("POLYGON((1 1, 10 1, 10 10, 1 10, 1 1))");
      auto aa =
          polygonFromWKT<double>("POLYGON((1 1, 1 10, 10 10, 10 1, 1 1))");
      auto aaa =
          polygonFromWKT<double>("POLYGON((1 1, 1 9, 10 10, 10 1, 1 1))");

      auto bb = polygonFromWKT<double>("POLYGON((1 1, 10 10, 10 1, 1 1))");

      auto b = lineFromWKT<double>("LINESTRING(10 5, 12 5)");
      auto c = lineFromWKT<double>("LINESTRING(12 5, 10 5)");

      auto bf = lineFromWKT<double>("LINESTRING(0 5, 1 5)");
      auto cf = lineFromWKT<double>("LINESTRING(1 5, 0 5)");

      auto d = lineFromWKT<double>("LINESTRING(10 10, 12 10)");
      auto e = lineFromWKT<double>("LINESTRING(12 10, 10 10)");

      auto f = lineFromWKT<double>("LINESTRING(5 5, 12 5)");
      auto ff = lineFromWKT<double>("LINESTRING(12 5, 5 5)");

      XSortedPolygon<double> ax(a);
      XSortedPolygon<double> aax(aa);
      XSortedPolygon<double> aaax(aaa);
      XSortedPolygon<double> bbx(bb);
      XSortedLine<double> bx(b);
      XSortedLine<double> bfx(bf);
      XSortedLine<double> cfx(cf);
      XSortedLine<double> cx(c);
      XSortedLine<double> dx(d);
      XSortedLine<double> ex(e);
      XSortedLine<double> fx(f);
      XSortedLine<double> ffx(ff);

      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, aax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, aax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(bfx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bfx, aax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(cfx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(cfx, aax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(cx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(cx, aax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(dx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(dx, aax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(ex, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ex, aax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(dx, aaax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(ex, aaax)));

      TEST(!std::get<1>(geo::intersectsContainsCovers(fx, bbx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ffx, bbx)));
    }

    {
      auto a = polygonFromWKT<double>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (4 4, 5 4, 4 5, 4 4))");
      auto b = pointFromWKT<double>("POINT(4.1 4.1)");

      XSortedPolygon<double> ax(a);

      TEST(ax.getInners().size() == 1);
      TEST(ax.getInnerBoxes().size() == 1);
      TEST(ax.getInnerAreas().size() == 1);
      TEST(!geo::contains(b, a));
      TEST(!geo::containsCovers(b, ax).first);
    }

    {
      auto a = polygonFromWKT<double>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (4 4, 5 4, 4 5, 4 4))");
      auto b = pointFromWKT<double>("POINT(3.9 3.9)");

      XSortedPolygon<double> ax(a);

      TEST(ax.getInners().size() == 1);
      TEST(ax.getInnerBoxes().size() == 1);
      TEST(ax.getInnerAreas().size() == 1);
      TEST(geo::contains(b, a));
      TEST(geo::containsCovers(b, ax).first);
    }

    {
      auto a = polygonFromWKT<double>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (4 4, 5 4, 4 5, 4 4))");
      auto b = polygonFromWKT<double>("POLYGON((4.1 4.1, 5 4, 5 5, 4 5, 4 4))");

      XSortedPolygon<double> ax(a);
      XSortedPolygon<double> bx(b);

      TEST(!geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(geo::intersects(a, b));
      TEST(geo::intersects(b, a));
      TEST(std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }

    {
      auto a = polygonFromWKT<int>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (4 4, 5 4, 5 5, 4 5, 4 4))");
      auto b = polygonFromWKT<int>(
          "POLYGON((1 1, 9 1, 9 9, 1 9, 1 1), (3 3, 6 3, 6 6, 3 6, 3 3))");

      TEST(geo::getWKT(a), ==,
           "POLYGON((0 0,10 0,10 10,0 10,0 0),(4 5,5 5,5 4,4 4,4 5))");
      TEST(geo::getWKT(b.getOuter().front()), ==, "POINT(1 1)");
      TEST(geo::getWKT(a.getInners().front()), ==,
           "LINESTRING(4 5,5 5,5 4,4 4)");

      TEST(geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(geo::intersects(a, b));
      TEST(geo::intersects(b, a));

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }

    {
      auto a = polygonFromWKT<int>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (4 4, 5 4, 5 5, 4 5, 4 4))");
      auto b = polygonFromWKT<int>(
          "POLYGON((1 1, 9 1, 9 9, 1 9, 1 1), (3 3, 6 3, 6 6, 3 6, 3 3))");

      TEST(geo::getWKT(a), ==,
           "POLYGON((0 0,10 0,10 10,0 10,0 0),(4 5,5 5,5 4,4 4,4 5))");
      TEST(geo::getWKT(b.getOuter().front()), ==, "POINT(1 1)");
      TEST(geo::getWKT(a.getInners().front()), ==,
           "LINESTRING(4 5,5 5,5 4,4 4)");

      TEST(geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(geo::intersects(a, b));
      TEST(geo::intersects(b, a));

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<0>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<1>(geo::intersectsContainsCovers(
          bx, util::geo::getBoundingBox(b), util::geo::outerArea(b), ax,
          util::geo::getBoundingBox(a), util::geo::outerArea(a))));
      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(std::get<0>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(
          ax, util::geo::getBoundingBox(a), util::geo::outerArea(a), bx,
          util::geo::getBoundingBox(b), util::geo::outerArea(b))));
    }

    {
      auto a = polygonFromWKT<int>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 9 1, 9 9, 1 9, 1 1))");
      auto b = polygonFromWKT<int>("POLYGON((4 4, 5 4, 5 5, 4 5, 4 4))");

      TEST(!geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(!geo::intersects(a, b));
      TEST(!geo::intersects(b, a));

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(!std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }

    {
      auto a = polygonFromWKT<int>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 5 5, 9 9, 1 9, 1 1))");
      auto b = polygonFromWKT<int>("POLYGON((4 4, 5 4, 5 5, 4 5, 4 4))");

      TEST(!geo::contains(b, a));
      TEST(!geo::contains(a, b));
      TEST(geo::intersects(a, b));
      TEST(geo::intersects(b, a));

      XSortedPolygon<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(geo::intersectsContainsCovers(bx, ax)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(bx, ax)));
      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }

    {
      auto a = lineFromWKT<int>("LINESTRING(4 4, 5 4, 5 5, 4 5, 4 4)");
      auto b = polygonFromWKT<int>("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");

      // TEST(!geo::contains(b, a));
      // TEST(!geo::contains(a, b));
      // TEST(geo::intersects(a, b));
      // TEST(geo::intersects(b, a));

      XSortedLine<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }
    {
      auto a = lineFromWKT<int>("LINESTRING(4 4, 5 4, 5 5, 4 5, 4 4)");
      auto b = polygonFromWKT<int>(
          "POLYGON((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 9 1, 9 9, 1 9, 1 1))");

      // TEST(!geo::contains(b, a));
      // TEST(!geo::contains(a, b));
      // TEST(geo::intersects(a, b));
      // TEST(geo::intersects(b, a));

      XSortedLine<int> ax(a);
      XSortedPolygon<int> bx(b);

      TEST(!std::get<0>(geo::intersectsContainsCovers(ax, bx)));
      TEST(!std::get<1>(geo::intersectsContainsCovers(ax, bx)));
    }
  }

  // ___________________________________________________________________________
  {
    TEST(geo::frechetDist(Line<double>{{0, 0}, {10, 10}},
                          Line<double>{{0, 0}, {10, 10}}, 1) == approx(0));

    TEST(geo::frechetDist(Line<double>{{0, 0}, {10, 10}},
                          Line<double>{{0, 0}, {10, 10}}, 0.1) == approx(0));

    TEST(geo::frechetDist(Line<double>{{0, 10}, {10, 10}},
                          Line<double>{{0, 0}, {10, 0}}, 0.1) == approx(10));

    TEST(geo::frechetDist(Line<double>{{0, 10}, {10, 10}},
                          Line<double>{{0, 0}, {0, 0}}, 0.1) ==
         approx(util::geo::dist(Point<double>{0, 0}, Point<double>{10, 10})));

    TEST(geo::frechetDist(Line<double>{{0, 10}, {0, 10}},
                          Line<double>{{0, 0}, {0, 5}}, 0.1) == approx(10));

    TEST(geo::frechetDist(Line<double>{{0, 10}, {0, 10}},
                          Line<double>{{0, 5}, {0, 5}}, 0.1) == approx(5));
  }

  // ___________________________________________________________________________
  {
    Grid<int, Line, double> g(
        .5, .5, Box<double>(Point<double>(0, 0), Point<double>(3, 3)));

    Line<double> l;
    l.push_back(Point<double>(0, 0));
    l.push_back(Point<double>(1.5, 2));

    Line<double> l2;
    l2.push_back(Point<double>(2.5, 1));
    l2.push_back(Point<double>(2.5, 2));

    g.add(l, 1);
    g.add(l2, 2);

    std::set<int> ret;

    Box<double> req(Point<double>(.5, 1), Point<double>(1, 1.5));
    g.get(req, &ret);
    TEST(ret.size(), ==, (size_t)1);

    ret.clear();
    g.getNeighbors(1, 0, &ret);
    TEST(ret.size(), ==, (size_t)1);

    ret.clear();
    g.getNeighbors(1, 0.55, &ret);
    TEST(ret.size(), ==, (size_t)2);

    g.remove(1);
    ret.clear();
    g.getNeighbors(1, 0, &ret);
    TEST(ret.size(), ==, (size_t)0);

    g.remove(2);
    ret.clear();
    g.getNeighbors(1, 10, &ret);
    TEST(ret.size(), ==, (size_t)0);

    // TODO: more test cases
  }

  // ___________________________________________________________________________
  {
    RTree<int, Line, double> g;

    Line<double> l;
    l.push_back(Point<double>(0, 0));
    l.push_back(Point<double>(1.5, 2));

    Line<double> l2;
    l2.push_back(Point<double>(2.5, 1));
    l2.push_back(Point<double>(2.5, 2));

    g.add(l, 1);
    g.add(l2, 2);

    std::set<int> ret;

    Box<double> req(Point<double>(.5, 1), Point<double>(1, 1.5));
    g.get(req, &ret);
    TEST(ret.size(), ==, (size_t)1);

    ret.clear();
    Box<double> req2(Point<double>(.5, 1), Point<double>(2.5, 2));
    g.get(req2, &ret);
    TEST(ret.size(), ==, (size_t)2);

    ret.clear();
    g.getNeighbors(1, 0, &ret);
    TEST(ret.size(), ==, (size_t)1);

    ret.clear();
    g.getNeighbors(1, 1.55, &ret);
    TEST(ret.size(), ==, (size_t)2);

    g.remove(1);
    ret.clear();
    g.getNeighbors(1, 0, &ret);
    TEST(ret.size(), ==, (size_t)0);

    g.remove(2);
    ret.clear();
    g.getNeighbors(1, 10, &ret);
    TEST(ret.size(), ==, (size_t)0);

    // TODO: more test cases
  }

  // ___________________________________________________________________________
  {
    Line<double> a;
    a.push_back(Point<double>(1, 1));
    a.push_back(Point<double>(10, 1));

    auto dense = util::geo::densify(a, 1);

    TEST(dense.size(), ==, (size_t)10);

    for (int i = 0; i < 10; i++) {
      TEST(dense[i].getX(), ==, approx(i + 1.0));
    }

    dense = util::geo::simplify(dense, 0.1);
    TEST(dense.size(), ==, (size_t)2);

    Line<double> b;
    b.push_back(Point<double>(1, 1));
    b.push_back(Point<double>(5, 7));
    b.push_back(Point<double>(10, 3));

    dense = util::geo::densify(b, 1);

    dense = util::geo::simplify(dense, 0.1);
    TEST(dense.size(), ==, (size_t)3);
  }

  // ___________________________________________________________________________
  {
    Line<double> a;
    a.push_back(Point<double>(1, 1));
    a.push_back(Point<double>(2, 1));
    a.push_back(Point<double>(3, 1));
    a.push_back(Point<double>(3, 2));
    a.push_back(Point<double>(4, 2));
    a.push_back(Point<double>(4, 1));
    a.push_back(Point<double>(5, 1));
    a.push_back(Point<double>(6, 1));

    Line<double> b;
    b.push_back(Point<double>(1, 1));
    b.push_back(Point<double>(2, 1));
    b.push_back(Point<double>(3, 1));
    b.push_back(Point<double>(4, 1));
    b.push_back(Point<double>(5, 1));
    b.push_back(Point<double>(6, 1));

    double fd = util::geo::accFrechetDistC(a, b, 0.1);
    TEST(fd, ==, approx(2));
  }

  // ___________________________________________________________________________
  {
    Line<double> e;
    e.push_back(Point<double>(1, 1));
    e.push_back(Point<double>(1, 2));

    Line<double> f;
    f.push_back(Point<double>(1, 1));
    f.push_back(Point<double>(1, 2));

    double fd = util::geo::frechetDist(e, f, 0.1);

    TEST(fd, ==, approx(0));

    Line<double> a;
    a.push_back(Point<double>(1, 1));
    a.push_back(Point<double>(2, 1));
    a.push_back(Point<double>(3, 2));
    a.push_back(Point<double>(4, 2));
    a.push_back(Point<double>(5, 1));
    a.push_back(Point<double>(6, 1));

    Line<double> b;
    b.push_back(Point<double>(1, 1));
    b.push_back(Point<double>(2, 1));
    b.push_back(Point<double>(3, 1));
    b.push_back(Point<double>(4, 1));
    b.push_back(Point<double>(5, 1));
    b.push_back(Point<double>(6, 1));

    auto adense = util::geo::densify(a, 0.1);
    auto bdense = util::geo::densify(b, 0.1);

    fd = util::geo::frechetDist(a, b, 0.1);

    TEST(fd, ==, approx(1));

    Line<double> c;
    c.push_back(Point<double>(1, 1));
    c.push_back(Point<double>(2, 1));

    Line<double> d;
    d.push_back(Point<double>(3, 1));
    d.push_back(Point<double>(4, 1));

    fd = util::geo::frechetDist(c, d, 0.1);

    TEST(fd, ==, approx(2));

    Line<double> g;
    g.push_back(Point<double>(1, 1));
    g.push_back(Point<double>(10, 1));

    Line<double> h;
    h.push_back(Point<double>(1, 1));
    h.push_back(Point<double>(3, 2));
    h.push_back(Point<double>(3, 1));
    h.push_back(Point<double>(10, 1));

    fd = util::geo::frechetDist(g, h, 0.1);

    TEST(fd, ==, approx(1));
  }

  // ___________________________________________________________________________
  {
    Line<double> a;
    a.push_back(Point<double>(1, 1));
    a.push_back(Point<double>(1, 2));

    Line<double> b;
    b.push_back(Point<double>(1, 2));
    b.push_back(Point<double>(2, 2));

    Line<double> c;
    c.push_back(Point<double>(2, 2));
    c.push_back(Point<double>(2, 1));

    Line<double> d;
    d.push_back(Point<double>(2, 1));
    d.push_back(Point<double>(1, 1));

    Box<double> box(Point<double>(2, 3), Point<double>(5, 4));
    MultiLine<double> ml;
    ml.push_back(a);
    ml.push_back(b);
    ml.push_back(c);
    ml.push_back(d);

    TEST(parallelity(box, ml), ==, approx(1));
    ml = rotate(ml, 45);
    TEST(parallelity(box, ml), ==, approx(0));
    ml = rotate(ml, 45);
    TEST(parallelity(box, ml), ==, approx(1));
    ml = rotate(ml, 45);
    TEST(parallelity(box, ml), ==, approx(0));
    ml = rotate(ml, 45);
    TEST(parallelity(box, ml), ==, approx(1));
  }

  // ___________________________________________________________________________
  {
    auto p = pointFromWKT<double>("POINT(10 50)");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("POINT( 10 50)");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("POINT(10 50 30)");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("POINT(10     50 30)");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("POINT(10 50 30)");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("POINT(10    50) ");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("MPOINT(10 50 30)");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("MPOINT(10 50)");
    TEST(p.getX(), ==, approx(10));
    TEST(p.getY(), ==, approx(50));

    p = pointFromWKT<double>("POINT(10.05 50.05)");
    TEST(p.getX(), ==, approx(10.05));
    TEST(p.getY(), ==, approx(50.05));

    auto wktl = lineFromWKT<double>("LINESTRING(0 0, 1 1,2 3, 0 1)");
    TEST(wktl.size(), ==, (size_t)4);
    TEST(wktl[0].getX(), ==, approx(0));
    TEST(wktl[0].getY(), ==, approx(0));
    TEST(wktl[1].getX(), ==, approx(1));
    TEST(wktl[1].getY(), ==, approx(1));
    TEST(wktl[2].getX(), ==, approx(2));
    TEST(wktl[2].getY(), ==, approx(3));
    TEST(wktl[3].getX(), ==, approx(0));
    TEST(wktl[3].getY(), ==, approx(1));

    wktl = lineFromWKT<double>("MLINESTRING(0 0, 1 1,2 3, 0 1)");
    TEST(wktl.size(), ==, (size_t)4);
    TEST(wktl[0].getX(), ==, approx(0));
    TEST(wktl[0].getY(), ==, approx(0));
    TEST(wktl[1].getX(), ==, approx(1));
    TEST(wktl[1].getY(), ==, approx(1));
    TEST(wktl[2].getX(), ==, approx(2));
    TEST(wktl[2].getY(), ==, approx(3));
    TEST(wktl[3].getX(), ==, approx(0));
    TEST(wktl[3].getY(), ==, approx(1));

    wktl = lineFromWKT<double>("MLINESTRING(0 0, 1  1,2   3, 0 1 )");
    TEST(wktl.size(), ==, (size_t)4);
    TEST(wktl[0].getX(), ==, approx(0));
    TEST(wktl[0].getY(), ==, approx(0));
    TEST(wktl[1].getX(), ==, approx(1));
    TEST(wktl[1].getY(), ==, approx(1));
    TEST(wktl[2].getX(), ==, approx(2));
    TEST(wktl[2].getY(), ==, approx(3));
    TEST(wktl[3].getX(), ==, approx(0));
    TEST(wktl[3].getY(), ==, approx(1));
  }
  // ___________________________________________________________________________
  {
    geo::Point<double> a(1, 2);
    geo::Point<double> b(2, 3);
    geo::Point<double> c(4, 5);
    TEST(a.getX(), ==, approx(1));
    TEST(a.getY(), ==, approx(2));

    a.setX(3);
    TEST(a.getX(), ==, approx(3));
    TEST(a.getY(), ==, approx(2));

    a.setY(4);
    TEST(a.getX(), ==, approx(3));
    TEST(a.getY(), ==, approx(4));

    auto d = a + b;
    TEST(d.getX(), ==, approx(5));
    TEST(d.getY(), ==, approx(7));

    a.setX(1);
    a.setY(2);

    TEST(geo::dist(a, a), ==, approx(0));
    TEST(geo::dist(a, b), ==, approx(sqrt(2)));

    d = d + d;

    geo::Box<double> box(a, c);
    TEST(geo::contains(a, box));
    TEST(geo::contains(b, box));
    TEST(geo::contains(c, box));
    TEST(!geo::contains(d, box));

    geo::Line<double> line{a, b, c};

    TEST(geo::contains(line, box));
    line.push_back(d);
    TEST(!geo::contains(line, box));

    geo::LineSegment<double> ls{a, b};
    TEST(geo::contains(a, ls));
    TEST(geo::contains(b, ls));
    TEST(!geo::contains(c, ls));
    TEST(geo::contains(a + geo::Point<double>(.5, .5), ls));
    TEST(!geo::contains(a + geo::Point<double>(1.5, 1.5), ls));

    geo::LineSegment<double> lsa{geo::Point<double>(1, 1),
                                 geo::Point<double>(2, 2)};
    geo::LineSegment<double> lsb{geo::Point<double>(1, 2),
                                 geo::Point<double>(2, 1)};
    geo::LineSegment<double> lsc{geo::Point<double>(2.1, 2),
                                 geo::Point<double>(3, 3)};

    TEST(geo::crossProd(lsa.first, lsb), ==, approx(-1));
    TEST(geo::crossProd(lsa.second, lsb), ==, approx(1));

    TEST(geo::intersects(lsa, lsb));

    TEST(!geo::intersects(lsa, lsa));
    TEST(!geo::intersects(lsb, lsb));
    TEST(!geo::intersects(lsa, lsc));

    TEST(!geo::intersects(geo::Point<double>(871569.2, 6104550.4),
                          geo::Point<double>(871581.2, 6104536),
                          geo::Point<double>(871580.3, 6104541.3),
                          geo::Point<double>(871625.7, 6104510.1)));

    TEST(!geo::intersects(geo::Point<double>(0, 0), geo::Point<double>(1, 1),
                          geo::Point<double>(0.5, 0.5),
                          geo::Point<double>(1.5, 1.5)));

    geo::Line<double> l{geo::Point<double>(1, 1), geo::Point<double>(2, 2),
                        geo::Point<double>(2, 4)};
    TEST(!geo::contains(geo::Point<double>(1, 2), l));
    TEST(geo::contains(geo::Point<double>(2, 2), l));
    TEST(geo::contains(geo::Point<double>(2, 3), l));

    geo::Box<double> bbox(geo::Point<double>(1, 1), geo::Point<double>(3, 3));
    TEST(geo::intersects(l, bbox));
    geo::Line<double> ll{geo::Point<double>(0, 0), geo::Point<double>(4, 4)};
    TEST(geo::intersects(ll, bbox));
    geo::Line<double> lll{geo::Point<double>(0, 0), geo::Point<double>(0, 4)};
    TEST(!geo::intersects(lll, bbox));
    geo::Line<double> llll{geo::Point<double>(1.2, 0),
                           geo::Point<double>(1, 2)};
    TEST(geo::intersects(llll, bbox));

    Line<double> l5new;
    l5new.push_back(Point<double>(-10, -5));
    l5new.push_back(Point<double>(-8, -4));
    TEST(geo::getBoundingBox(l5new).getUpperRight().getX(), ==, approx(-8));
    TEST(geo::getBoundingBox(l5new).getUpperRight().getY(), ==, approx(-4));

    Line<double> l5;
    l5.push_back(Point<double>(0, 0));
    l5.push_back(Point<double>(1.5, 2));
    Box<double> req(Point<double>(.5, 1), Point<double>(1, 1.5));

    TEST(geo::getBoundingBox(l5[0]).getLowerLeft().getX(), ==, approx(0));
    TEST(geo::getBoundingBox(l5[0]).getLowerLeft().getY(), ==, approx(0));

    TEST(geo::getBoundingBox(l5).getLowerLeft().getX(), ==, approx(0));
    TEST(geo::getBoundingBox(l5).getLowerLeft().getY(), ==, approx(0));
    TEST(geo::getBoundingBox(l5).getUpperRight().getX(), ==, approx(1.5));
    TEST(geo::getBoundingBox(l5).getUpperRight().getY(), ==, approx(2));
    TEST(geo::intersects(geo::getBoundingBox(l5),
                         geo::getBoundingBox(Line<double>{
                             Point<double>(.5, 1), Point<double>(1, 1)})));
    TEST(geo::intersects(
        l5, Line<double>{Point<double>(.5, 1), Point<double>(1, 1)}));
    TEST(geo::intersects(l5, req));

    Box<double> boxa(Point<double>(1, 1), Point<double>(2, 2));
    TEST(geo::intersects(
        boxa, Box<double>(Point<double>(1.5, 1.5), Point<double>(1.7, 1.7))));
    TEST(geo::intersects(
        boxa, Box<double>(Point<double>(0, 0), Point<double>(3, 3))));
    TEST(geo::intersects(
        boxa, Box<double>(Point<double>(1.5, 1.5), Point<double>(3, 3))));
    TEST(geo::intersects(
        boxa, Box<double>(Point<double>(0, 0), Point<double>(1.5, 1.5))));

    TEST(geo::intersects(
        Box<double>(Point<double>(1.5, 1.5), Point<double>(1.7, 1.7)), boxa));
    TEST(geo::intersects(Box<double>(Point<double>(0, 0), Point<double>(3, 3)),
                         boxa));
    TEST(geo::intersects(
        Box<double>(Point<double>(1.5, 1.5), Point<double>(3, 3)), boxa));
    TEST(geo::intersects(
        Box<double>(Point<double>(0, 0), Point<double>(1.5, 1.5)), boxa));

    Polygon<double> poly({Point<double>(1, 1), Point<double>(3, 2),
                          Point<double>(4, 3), Point<double>(6, 3),
                          Point<double>(5, 1)});
    TEST(geo::getWKT(poly), ==, "POLYGON((5 1,6 3,4 3,3 2,1 1,5 1))");
    TEST(geo::contains(Point<double>(4, 2), poly));
    TEST(!geo::contains(Point<double>(3, 3), poly));
    TEST(geo::contains(Point<double>(1, 1), poly));
    TEST(geo::contains(Point<double>(3, 2), poly));
    TEST(geo::contains(Point<double>(4, 3), poly));
    TEST(geo::contains(Point<double>(6, 3), poly));
    TEST(geo::contains(Point<double>(5, 1), poly));

    TEST(geo::contains(Line<double>{Point<double>(6, 3), Point<double>(5, 1)},
                       poly));
    TEST(!geo::contains(Line<double>{Point<double>(6, 3), Point<double>(50, 1)},
                        poly));
    TEST(geo::contains(Line<double>{Point<double>(4, 2), Point<double>(4.5, 2)},
                       poly));
    TEST(geo::contains(Line<double>{Point<double>(4, 2), Point<double>(5, 1)},
                       poly));

    Box<double> polybox(Point<double>(1, 1), Point<double>(6, 4));
    TEST(geo::centroid(polybox).getX(), ==, approx(3.5));
    TEST(geo::centroid(polybox).getY(), ==, approx(2.5));
    TEST(geo::contains(poly, polybox));
    TEST(!geo::contains(polybox, poly));
    Box<double> polybox2(Point<double>(4, 1), Point<double>(5, 2));
    TEST(geo::contains(polybox2, poly));
    TEST(geo::contains(poly, getBoundingBox(poly)));

    Point<double> rotP(2, 2);
    TEST(geo::dist(geo::rotate(rotP, 180, Point<double>(1, 1)),
                   Point<double>(0, 0)) == approx(0));
    TEST(geo::dist(geo::rotate(rotP, 360, Point<double>(1, 1)), rotP) ==
         approx(0));

    Line<double> rotLine({{1, 1}, {3, 3}});
    TEST(geo::rotate(rotLine, 90, Point<double>(2, 2))[0].getX(), ==,
         approx(1));
    TEST(geo::rotate(rotLine, 90, Point<double>(2, 2))[0].getY(), ==,
         approx(3));
    TEST(geo::rotate(rotLine, 90, Point<double>(2, 2))[1].getX(), ==,
         approx(3));
    TEST(geo::rotate(rotLine, 90, Point<double>(2, 2))[1].getY(), ==,
         approx(1));

    MultiLine<double> multiRotLine({{{1, 1}, {3, 3}}, {{1, 3}, {3, 1}}});
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[0][0].getX() ==
         approx(1));
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[0][0].getY() ==
         approx(3));
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[0][1].getX() ==
         approx(3));
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[0][1].getY() ==
         approx(1));
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[1][0].getX() ==
         approx(3));
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[1][0].getY() ==
         approx(3));
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[1][1].getX() ==
         approx(1));
    TEST(geo::rotate(multiRotLine, 90, Point<double>(2, 2))[1][1].getY() ==
         approx(1));

    TEST(geo::getWKT(multiRotLine) == "MULTILINESTRING((1 1,3 3),(1 3,3 1))");

    TEST(geo::contains(
        multiRotLine[0],
        geo::move(geo::move(multiRotLine, 1.0, 2.0), -1.0, -2.0)[0]));
    TEST(geo::contains(multiRotLine, geo::getBoundingBox(Line<double>{
                                         {1, 1}, {3, 3}, {1, 3}, {3, 1}})));

    TEST(geo::contains(
        getBoundingBox(multiRotLine),
        geo::getBoundingBox(Line<double>{{1, 1}, {3, 3}, {1, 3}, {3, 1}})));
    TEST(geo::contains(
        geo::getBoundingBox(Line<double>{{1, 1}, {3, 3}, {1, 3}, {3, 1}}),
        getBoundingBox(multiRotLine)));

    TEST(geo::dist(geo::centroid(rotP), rotP), ==, approx(0));
    TEST(geo::dist(geo::centroid(rotLine), rotP), ==, approx(0));
    TEST(geo::dist(geo::centroid(polybox), Point<double>(3.5, 2.5)) ==
         approx(0));
    TEST(geo::dist(geo::centroid(Polygon<double>({{0, 0}, {3, 4}, {4, 3}})),
                   Point<double>(7.0 / 3.0, 7.0 / 3.0)) == approx(0));

    auto polyy = Polygon<double>({{0, 0}, {3, 4}, {4, 3}});
    MultiPolygon<double> mpoly{polyy, polyy};

    TEST(geo::getWKT(polyy), ==, "POLYGON((4 3,3 4,0 0,4 3))");
    TEST(geo::getWKT(mpoly) ==
         "MULTIPOLYGON(((4 3,3 4,0 0,4 3)),((4 3,3 4,0 0,4 3)))");

    TEST(geo::getWKT(geo::centroid(mpoly)) ==
         geo::getWKT(geo::centroid(polyy)));

    auto hull = geo::convexHull(Line<double>{
        {0.1, 3}, {1, 1}, {2, 2}, {4, 4}, {0, 0}, {1, 2}, {3, 1}, {3, 3}});
    TEST(hull.getOuter().size(), ==, size_t(4));
    TEST(geo::contains(geo::convexHull(geo::getBoundingBox(poly)),
                       geo::getBoundingBox(poly)));
    TEST(geo::contains(geo::getBoundingBox(poly),
                       geo::convexHull(geo::getBoundingBox(poly))));

    auto hull2 = geo::convexHull(Line<double>{{0.1, 3},
                                              {1, 1},
                                              {2, 2},
                                              {4, 4},
                                              {0, 0},
                                              {1, 2},
                                              {3, 1},
                                              {3, 3},
                                              {-0.1, 1}});
    TEST(hull2.getOuter().size(), ==, size_t(5));
    TEST(hull2.getOuter()[4].getX(), ==, approx(-.1));
    TEST(hull2.getOuter()[4].getY(), ==, approx(1));
    TEST(hull2.getOuter()[3].getX(), ==, approx(0.1));
    TEST(hull2.getOuter()[3].getY(), ==, approx(3));
    TEST(hull2.getOuter()[2].getX(), ==, approx(4));
    TEST(hull2.getOuter()[2].getY(), ==, approx(4));
    TEST(hull2.getOuter()[1].getX(), ==, approx(3));
    TEST(hull2.getOuter()[1].getY(), ==, approx(1));
    TEST(hull2.getOuter()[0].getX(), ==, approx(0));
    TEST(hull2.getOuter()[0].getY(), ==, approx(0));

    auto hull3 =
        geo::convexHull(Line<double>{{0.1, 3}, {4, 4}, {0, 0}, {1, 2}, {3, 1}});
    TEST(hull3.getOuter().size(), ==, size_t(4));
    TEST(hull3.getOuter()[0].getX(), ==, approx(3));
    TEST(hull3.getOuter()[0].getY(), ==, approx(1));
    TEST(hull3.getOuter()[1].getX(), ==, approx(4));
    TEST(hull3.getOuter()[1].getY(), ==, approx(4));
    TEST(hull3.getOuter()[2].getX(), ==, approx(0.1));
    TEST(hull3.getOuter()[2].getY(), ==, approx(3));
    TEST(hull3.getOuter()[3].getX(), ==, approx(0));
    TEST(hull3.getOuter()[3].getY(), ==, approx(0));

    hull3 = geo::convexHull(
        Line<double>{{0.1, 3}, {4, 4}, {2, 1}, {3, 2}, {0, 0}, {1, 2}, {3, 1}});
    TEST(hull3.getOuter().size(), ==, size_t(4));
    TEST(hull3.getOuter()[0].getX(), ==, approx(3));
    TEST(hull3.getOuter()[0].getY(), ==, approx(1));
    TEST(hull3.getOuter()[1].getX(), ==, approx(4));
    TEST(hull3.getOuter()[1].getY(), ==, approx(4));
    TEST(hull3.getOuter()[2].getX(), ==, approx(0.1));
    TEST(hull3.getOuter()[2].getY(), ==, approx(3));
    TEST(hull3.getOuter()[3].getX(), ==, approx(0));
    TEST(hull3.getOuter()[3].getY(), ==, approx(0));

    hull3 = geo::convexHull(Line<double>{
        {4, 4}, {1, 2}, {2, 1}, {3, 2}, {0.1, 3}, {0, 0}, {1, 2}, {3, 1}});
    TEST(hull3.getOuter().size(), ==, size_t(4));
    TEST(hull3.getOuter()[0].getX(), ==, approx(3));
    TEST(hull3.getOuter()[0].getY(), ==, approx(1));
    TEST(hull3.getOuter()[1].getX(), ==, approx(4));
    TEST(hull3.getOuter()[1].getY(), ==, approx(4));
    TEST(hull3.getOuter()[2].getX(), ==, approx(0.1));
    TEST(hull3.getOuter()[2].getY(), ==, approx(3));
    TEST(hull3.getOuter()[3].getX(), ==, approx(0));
    TEST(hull3.getOuter()[3].getY(), ==, approx(0));

    hull3 = geo::convexHull(Line<double>{{4, 4}, {1, 2}, {3, 1}});
    TEST(hull3.getOuter().size(), ==, size_t(3));
    TEST(hull3.getOuter()[0].getX(), ==, approx(3));
    TEST(hull3.getOuter()[0].getY(), ==, approx(1));
    TEST(hull3.getOuter()[1].getX(), ==, approx(4));
    TEST(hull3.getOuter()[1].getY(), ==, approx(4));
    TEST(hull3.getOuter()[2].getX(), ==, approx(1));
    TEST(hull3.getOuter()[2].getY(), ==, approx(2));

    hull3 = geo::convexHull(Line<double>{{4, 4}, {1, 2}, {3, 10}});
    TEST(hull3.getOuter().size(), ==, size_t(3));
    TEST(hull3.getOuter()[0].getX(), ==, approx(4));
    TEST(hull3.getOuter()[0].getY(), ==, approx(4));
    TEST(hull3.getOuter()[1].getX(), ==, approx(3));
    TEST(hull3.getOuter()[1].getY(), ==, approx(10));
    TEST(hull3.getOuter()[2].getX(), ==, approx(1));
    TEST(hull3.getOuter()[2].getY(), ==, approx(2));

    Line<double> test{{0.3215348546593775, 0.03629583077160248},
                      {0.02402358131857918, -0.2356728797179394},
                      {0.04590851212470659, -0.4156409924995536},
                      {0.3218384001607433, 0.1379850698988746},
                      {0.11506479756447, -0.1059521474930943},
                      {0.2622539999543261, -0.29702873322836},
                      {-0.161920957418085, -0.4055339716426413},
                      {0.1905378631228002, 0.3698601009043493},
                      {0.2387090918968516, -0.01629827079949742},
                      {0.07495888748668034, -0.1659825110491202},
                      {0.3319341836794598, -0.1821814101954749},
                      {0.07703635755650362, -0.2499430638271785},
                      {0.2069242999022122, -0.2232970760420869},
                      {0.04604079532068295, -0.1923573186549892},
                      {0.05054295812784038, 0.4754929463150845},
                      {-0.3900589168910486, 0.2797829520700341},
                      {0.3120693385713448, -0.0506329867529059},
                      {0.01138812723698857, 0.4002504701728471},
                      {0.009645149586391732, 0.1060251100976254},
                      {-0.03597933197019559, 0.2953639456959105},
                      {0.1818290866742182, 0.001454397571696298},
                      {0.444056063372694, 0.2502497166863175},
                      {-0.05301752458607545, -0.06553921621808712},
                      {0.4823896228171788, -0.4776170002088109},
                      {-0.3089226845734964, -0.06356112199235814},
                      {-0.271780741188471, 0.1810810595574612},
                      {0.4293626522918815, 0.2980897964891882},
                      {-0.004796652127799228, 0.382663812844701},
                      {0.430695573269106, -0.2995073500084759},
                      {0.1799668387323309, -0.2973467472915973},
                      {0.4932166845474547, 0.4928094162538735},
                      {-0.3521487911717489, 0.4352656197131292},
                      {-0.4907368011686362, 0.1865826865533206},
                      {-0.1047924716070224, -0.247073392148198},
                      {0.4374961861758457, -0.001606279519951237},
                      {0.003256207800708899, -0.2729194320486108},
                      {0.04310378203457577, 0.4452604050238248},
                      {0.4916198379282093, -0.345391701297268},
                      {0.001675087028811806, 0.1531837672490476},
                      {-0.4404289572876217, -0.2894855991839297}

    };
    hull3 = geo::convexHull(test);
    TEST(geo::contains(test, hull3));
    TEST(hull3.getOuter().size(), ==, size_t(8));
    TEST(geo::contains(
        Polygon<double>({{-0.161920957418085, -0.4055339716426413},
                         {0.05054295812784038, 0.4754929463150845},
                         {0.4823896228171788, -0.4776170002088109},
                         {0.4932166845474547, 0.4928094162538735},
                         {-0.3521487911717489, 0.4352656197131292},
                         {-0.4907368011686362, 0.1865826865533206},
                         {0.4916198379282093, -0.345391701297268},
                         {-0.4404289572876217, -0.2894855991839297}}),
        hull3));
    TEST(geo::contains(
        hull3, Polygon<double>({{-0.161920957418085, -0.4055339716426413},
                                {0.05054295812784038, 0.4754929463150845},
                                {0.4823896228171788, -0.4776170002088109},
                                {0.4932166845474547, 0.4928094162538735},
                                {-0.3521487911717489, 0.4352656197131292},
                                {-0.4907368011686362, 0.1865826865533206},
                                {0.4916198379282093, -0.345391701297268},
                                {-0.4404289572876217, -0.2894855991839297}})));

    hull3 = geo::convexHull(Line<double>{{3, 6},
                                         {8, 10},
                                         {3, 5},
                                         {20, -10},
                                         {-4, 5},
                                         {10, 2},
                                         {5, 1},
                                         {45, 1},
                                         {30, -9},
                                         {3, 14},
                                         {25, -5.5}});
    TEST(hull3.getOuter().size(), ==, size_t(5));
    TEST(hull3.getOuter()[0].getX(), ==, approx(20));
    TEST(hull3.getOuter()[0].getY(), ==, approx(-10));
    TEST(hull3.getOuter()[1].getX(), ==, approx(30));
    TEST(hull3.getOuter()[1].getY(), ==, approx(-9));
    TEST(hull3.getOuter()[2].getX(), ==, approx(45));
    TEST(hull3.getOuter()[2].getY(), ==, approx(1));
    TEST(hull3.getOuter()[3].getX(), ==, approx(3));
    TEST(hull3.getOuter()[3].getY(), ==, approx(14));
    TEST(hull3.getOuter()[4].getX(), ==, approx(-4));
    TEST(hull3.getOuter()[4].getY(), ==, approx(5));

    hull3 = geo::convexHull(Line<double>{
        {7, 7}, {7, -7}, {-7, -7}, {-7, 7}, {9, 0}, {-9, 0}, {0, 9}, {0, -9}});
    TEST(hull3.getOuter().size(), ==, size_t(8));
    TEST(geo::contains(geo::Polygon<double>({{-9, 0},
                                             {-7, -7},
                                             {0, -9},
                                             {7, -7},
                                             {9, 0},
                                             {7, 7},
                                             {0, 9},
                                             {-7, 7}}),
                       hull3));
    TEST(geo::contains(hull3, geo::Polygon<double>({{-9, 0},
                                                    {-7, -7},
                                                    {0, -9},
                                                    {7, -7},
                                                    {9, 0},
                                                    {7, 7},
                                                    {0, 9},
                                                    {-7, 7}})));

    hull3 = geo::convexHull(Line<double>{{7, 7},
                                         {7, -7},
                                         {-7, -7},
                                         {-7, 7},
                                         {9, 0},
                                         {-9, 0},
                                         {0, 9},
                                         {0, -9},
                                         {0, 0},
                                         {1, 2},
                                         {-2, 1},
                                         {-1, -1},
                                         {3, 4},
                                         {4, 3},
                                         {-5, 4},
                                         {6, 5}});
    TEST(hull3.getOuter().size(), ==, size_t(8));
    TEST(geo::contains(geo::Polygon<double>({{-9, 0},
                                             {-7, -7},
                                             {0, -9},
                                             {7, -7},
                                             {9, 0},
                                             {7, 7},
                                             {0, 9},
                                             {-7, 7}}),
                       hull3));
    TEST(geo::contains(hull3, geo::Polygon<double>({{-9, 0},
                                                    {-7, -7},
                                                    {0, -9},
                                                    {7, -7},
                                                    {9, 0},
                                                    {7, 7},
                                                    {0, 9},
                                                    {-7, 7}})));

    hull3 = geo::convexHull(Line<double>{
        {0, 0},   {1, 2},  {-2, 1}, {-1, -1}, {3, 4},   {4, 3},   {-5, 4},
        {6, 5},   {7, 7},  {7, -7}, {-7, -7}, {-7, 7},  {9, 0},   {-9, 0},
        {0, 9},   {0, -9}, {-8, 0}, {8, 0},   {-7, 0},  {7, 0},   {-6, 0},
        {6, 0},   {-5, 0}, {5, 0},  {-4, 0},  {4, 0},   {-3, 0},  {3, 0},
        {-2, 0},  {2, 0},  {-1, 0}, {1, 0},   {0, -8},  {0, 8},   {0, -7},
        {0, 7},   {0, -6}, {0, 6},  {0, -5},  {0, 5},   {0, -4},  {0, 4},
        {0, -3},  {0, 3},  {0, -2}, {0, 2},   {0, -1},  {0, 1},   {1, 1},
        {2, 2},   {3, 3},  {4, 4},  {5, 5},   {6, 6},   {1, -1},  {2, -2},
        {3, -3},  {4, -4}, {5, -5}, {6, -6},  {-1, 1},  {-2, 2},  {-3, 3},
        {-4, 4},  {-5, 5}, {-6, 6}, {-1, -1}, {-2, -2}, {-3, -3}, {-4, -4},
        {-5, -5}, {-6, -6}});
    TEST(hull3.getOuter().size(), ==, size_t(8));
    TEST(geo::contains(geo::Polygon<double>({{-9, 0},
                                             {-7, -7},
                                             {0, -9},
                                             {7, -7},
                                             {9, 0},
                                             {7, 7},
                                             {0, 9},
                                             {-7, 7}}),
                       hull3));
    TEST(geo::contains(hull3, geo::Polygon<double>({{-9, 0},
                                                    {-7, -7},
                                                    {0, -9},
                                                    {7, -7},
                                                    {9, 0},
                                                    {7, 7},
                                                    {0, 9},
                                                    {-7, 7}})));

    TEST(geo::area(geo::Point<double>(1, 2)), ==, approx(0));
    TEST(geo::area(geo::Line<double>{{1, 2}, {2, 5}}), ==, approx(0));
    TEST(geo::area(geo::Box<double>({0, 0}, {1, 1})), ==, approx(1));
    TEST(geo::area(geo::Box<double>({1, 1}, {1, 1})), ==, approx(0));
    TEST(geo::area(geo::Box<double>({0, 0}, {2, 2})), ==, approx(4));
    TEST(geo::area(geo::Polygon<double>({{0, 0}, {1, 0}, {1, 1}, {0, 1}})) ==
         approx(1));
    TEST(geo::area(geo::Polygon<double>({{0, 0}, {1, 0}, {1, 1}})) ==
         approx(0.5));

    auto obox =
        geo::getOrientedEnvelope(geo::Line<double>{{0, 0}, {1, 1}, {1.5, 0.5}});
    // TEST(geo::contains(
    // geo::Line<double>{{0, 0}, {1, 1}, {1.5, 0.5}}, geo::convexHull(obox)));
    TEST(geo::area(geo::convexHull(obox)), ==, approx(1));

    obox = geo::getOrientedEnvelope(geo::Polygon<double>({{0.0, 1.0},
                                                          {0.9, 1.1},
                                                          {1.0, 2.0},
                                                          {1.1, 1.1},
                                                          {2.0, 1.0},
                                                          {1.1, 0.9},
                                                          {1.0, 0.0},
                                                          {0.9, 0.9}}));
    // TEST(geo::contains(
    // geo::Polygon<double>({{0.0, 1.0}, {0.9, 1.1},
    // {1.0, 2.0}, {1.1, 1.1},
    // {2.0, 1.0}, {1.1, 0.9},
    // {1.0, 0.0}, {0.9, 0.9}}), geo::convexHull(obox)));
    TEST(geo::area(geo::convexHull(obox)), ==, approx(2));

    obox = geo::getOrientedEnvelope(std::vector<geo::Polygon<double>>(
        {geo::Polygon<double>({{0.0, 1.0},
                               {0.9, 1.1},
                               {1.0, 2.0},
                               {1.1, 1.1},
                               {2.0, 1.0},
                               {1.1, 0.9},
                               {1.0, 0.0},
                               {0.9, 0.9}})}));

    TEST(geo::dist(geo::LineSegment<double>{{1, 1}, {3, 1}},
                   geo::LineSegment<double>{{2, 2}, {2, 0}}) == approx(0));
    TEST(geo::dist(geo::LineSegment<double>{{1, 1}, {3, 1}},
                   geo::LineSegment<double>{{2, 4}, {2, 2}}) == approx(1));
    TEST(geo::dist(geo::LineSegment<double>{{1, 1}, {3, 1}},
                   geo::LineSegment<double>{{1, 1}, {3, 1}}) == approx(0));
    TEST(geo::dist(geo::LineSegment<double>{{1, 1}, {3, 1}},
                   geo::LineSegment<double>{{1, 2}, {3, 2}}) == approx(1));
    TEST(geo::dist(geo::LineSegment<double>{{1, 1}, {3, 1}},
                   geo::LineSegment<double>{{1, 2}, {3, 5}}) == approx(1));

    TEST(geo::dist(geo::Line<double>{{1, 1}, {3, 1}},
                   geo::Point<double>{2, 1}) == approx(0));
    TEST(geo::dist(geo::Line<double>{{1, 1}, {3, 1}},
                   geo::Point<double>{2, 2}) == approx(1));
    TEST(geo::dist(geo::Line<double>{{1, 1}, {3, 1}},
                   geo::Point<double>{3, 1}) == approx(0));
    TEST(geo::dist(geo::Line<double>{{1, 1}, {3, 1}},
                   geo::Point<double>{1, 1}) == approx(0));

    TEST(geo::dist(Line<double>{{7, 7},
                                {7, -7},
                                {-7, -7},
                                {-7, 7},
                                {9, 0},
                                {-9, 0},
                                {0, 9},
                                {0, -9}},
                   Line<double>{{7, 7},
                                {7, -7},
                                {-7, -7},
                                {-7, 7},
                                {9, 0},
                                {-9, 0},
                                {0, 9},
                                {0, -9}}) == approx(0));
    TEST(geo::dist(Line<double>{{7, 7},
                                {7, -7},
                                {-7, -7},
                                {-7, 7},
                                {9, 0},
                                {-9, 0},
                                {0, 9},
                                {0, -9}},
                   LineSegment<double>{{6, 7}, {8, -7}}) == approx(0));
    TEST(geo::dist(Line<double>{{7, 7},
                                {7, -7},
                                {-7, -7},
                                {-7, 7},
                                {9, 0},
                                {-9, 0},
                                {0, 9},
                                {0, -9}},
                   Point<double>{7, 4}) == approx(0));
    TEST(geo::dist(Line<double>{{0, 0}, {1, 1}, {2, 0}},
                   Line<double>{{1.5, 0.5}, {1.5, 100}}) == approx(0));
    TEST(geo::dist(Line<double>{{0, 0}, {1, 1}, {2, 0}},
                   Line<double>{{2, 0.5}, {2, 100}}) == approx(0.353553));

    TEST(geo::contains(util::geo::Point<double>{1.5, 0.5},
                       util::geo::LineSegment<double>{{1, 1}, {1.5, 0.5}}));
    TEST(geo::contains(util::geo::Point<double>{1.5, 0.5},
                       util::geo::LineSegment<double>{{1, 1}, {1.5, 0.5}}));

    auto polyTest =
        geo::Polygon<double>({{1, 1}, {3, 1}, {2, 2}, {3, 3}, {1, 3}});
    TEST(!geo::contains(util::geo::LineSegment<double>({2.5, 1.3}, {2.5, 2.6}),
                        polyTest));

    TEST(!geo::contains(util::geo::LineSegment<double>{{2.5, 1.3}, {2.5, 2.6}},
                        polyTest));
    TEST(geo::contains(util::geo::LineSegment<double>{{2.5, 2.6}, {1.5, 2}},
                       polyTest));
    TEST(!geo::contains(
        util::geo::Line<double>{{2.5, 1.3}, {2.5, 2.6}, {1.5, 2}}, polyTest));
    TEST(geo::contains(
        util::geo::Line<double>{{2.5, 1.3}, {1.5, 2}, {2.5, 2.6}}, polyTest));

    TEST(!geo::contains(util::geo::Box<double>{{1, 1}, {2.5, 2.6}}, polyTest));

    TEST(
        geo::intersects(Box<double>(Point<double>(0, 0), Point<double>(10, 10)),
                        Box<double>(Point<double>(2, 2), Point<double>(8, 8))));
    TEST(geo::intersects(
        Box<double>(Point<double>(0, 0), Point<double>(10, 10)),
        Box<double>(Point<double>(-2, -2), Point<double>(8, 8))));
    TEST(geo::intersects(
        Box<double>(Point<double>(0, 0), Point<double>(10, 10)),
        Box<double>(Point<double>(-2, -2), Point<double>(12, 12))));
    TEST(geo::intersects(
        Box<double>(Point<double>(0, 0), Point<double>(10, 10)),
        Box<double>(Point<double>(5, 5), Point<double>(12, 12))));

    TEST(!geo::intersects(
        Box<double>(Point<double>(0, 0), Point<double>(10, 10)),
        Box<double>(Point<double>(15, 15), Point<double>(12, 12))));

    double rad = 10.0;
    int n = 20;
    util::geo::MultiPoint<double> mp;

    for (int i = 0; i < n; i++) {
      double x = rad * cos((2.0 * M_PI / static_cast<double>(n)) *
                           static_cast<double>(i));
      double y = rad * sin((2.0 * M_PI / static_cast<double>(n)) *
                           static_cast<double>(i));

      mp.push_back(util::geo::DPoint(x, y));
    }

    auto h = util::geo::convexHull(mp);

    TEST(geo::contains(mp, h));

    TEST(geo::dist(geo::interpolate(DPoint(1.0, 1.0), DPoint(1.0, 3.0), 0.5),
                   DPoint(1.0, 2.0)),
         ==, approx(0));
    TEST(geo::dist(geo::interpolate(DPoint(1.0, 1.0), DPoint(2.0, 2.0), 0),
                   DPoint(1.0, 1.0)),
         ==, approx(0));
    TEST(geo::dist(geo::interpolate(DPoint(1.0, 1.0), DPoint(2.0, 2.0), 0.5),
                   DPoint(1.5, 1.5)),
         ==, approx(0));
    TEST(geo::dist(geo::interpolate(DPoint(1.0, 1.0), DPoint(2.0, 2.0), 1),
                   DPoint(2, 2)),
         ==, approx(0));
    TEST(geo::dist(geo::pointAtDist(DLine{{0, 0}, {0, 1}, {0, 2}}, 1),
                   DPoint{0, 1}),
         ==, approx(0));
    TEST(geo::dist(geo::pointAtDist(DLine{{0, 0}, {0, 1}, {0, 2}}, 2),
                   DPoint{0, 2}),
         ==, approx(0));
    TEST(geo::dist(geo::pointAtDist(DLine{{0, 0}, {0, 1}, {0, 2}}, 0),
                   DPoint{0, 0}),
         ==, approx(0));

    TEST(geo::getWKT(geo::orthoLineAtDist(DLine{{0, 0}, {0, 1}, {0, 2}}, 1, 1)),
         ==, "LINESTRING(-0.5 1,0.5 1)");

    TEST(geo::getWKT(geo::segment(DLine{{0, 0}, {0, 1}, {0, 2}}, 0, 0.5)), ==,
         "LINESTRING(0 0,0 1)");
    TEST(geo::getWKT(geo::segment(DLine{{0, 0}, {0, 1}, {0, 2}}, 0.5, 1)), ==,
         "LINESTRING(0 1,0 2)");
  }

  {
    std::stringstream ss;
    util::geo::output::GeoJsonOutput out(ss);

    Polygon<int> poly{{{1, 1}, {3, 2}, {4, 3}, {6, 3}, {5, 1}, {1, 1}}};
    Polygon<int> poly3{{{1, 3}, {3, 4}, {4, 5}, {6, 5}, {5, 3}, {1, 3}}};

    out.print(poly, {});
    out.flush();
    std::string a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\","
         "\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[[5,1],[6,3],[4,"
         "3],[3,2],[1,1],[5,1]]]},\"properties\":{}}]}");

    Polygon<int> poly2{{{1, 1}, {3, 2}, {4, 3}, {6, 3}, {5, 1}, {1, 1}},
                       {{{1, 1}, {1, 2}, {2, 2}, {1, 1}}}};

    ss.str("");
    out.print(poly2, {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"Polygon\","
         "\"coordinates\":[[[5,1],[6,3],[4,3],[3,2],[1,1],[5,1]],[[1,1],[1,2],["
         "2,2],[1,1]]]},\"properties\":{}}");

    ss.str("");
    out.print(util::geo::MultiPolygon<int>{poly3, poly2}, {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPolygon\","
         "\"coordinates\":[[[[5,3],[6,5],[4,5],[3,4],[1,3],[5,3]]],[[[5,1],[6,"
         "3],[4,3],[3,2],[1,1],[5,1]],[[1,1],[1,2],[2,2],[1,1]]]]},"
         "\"properties\":{}}");

    ss.str("");
    out.print(geo::lineFromWKT<int>("LINESTRING(0 0,1 1)"), {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"LineString\","
         "\"coordinates\":[[0,0],[1,1]]},\"properties\":{}}");

    ss.str("");
    out.print(
        util::geo::MultiLine<int>{geo::lineFromWKT<int>("LINESTRING(0 0,1 1)"),
                                  geo::lineFromWKT<int>("LINESTRING(2 2,3 3)")},
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\","
         "\"coordinates\":[[[0,0],[1,1]],[[2,2],[3,3]]]},\"properties\":{}}");

    ss.str("");
    out.print(
        geo::multiLineFromWKT<int>("MULTILINESTRING((0 0, 1 1), (2 2, 3 3))"),
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\","
         "\"coordinates\":[[[0,0],[1,1]],[[2,2],[3,3]]]},\"properties\":{}}");

    ss.str("");
    out.print(geo::multiPointFromWKT<int>("MULTIPOINT(0 0,1 1,2 2,3 3)"), {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPoint\","
         "\"coordinates\":[[0,0],[1,1],[2,2],[3,3]]},\"properties\":{}}");

    ss.str("");
    out.print(
        geo::multiPointFromWKT<int>("MULTIPOINT((0 0),(1 1),(2 2),(3 3))"), {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPoint\","
         "\"coordinates\":[[0,0],[1,1],[2,2],[3,3]]},\"properties\":{}}");
  }

  {
    std::stringstream ss;
    util::geo::output::GeoJsonOutput out(ss);

    Polygon<int> poly{{{1, 1}, {3, 2}, {4, 3}, {6, 3}, {5, 1}, {1, 1}}};
    Polygon<int> poly3{{{1, 3}, {3, 4}, {4, 5}, {6, 5}, {5, 3}, {1, 3}}};

    out.print(poly, {});
    out.flush();
    std::string a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\","
         "\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[[5,1],[6,3],[4,"
         "3],[3,2],[1,1],[5,1]]]},\"properties\":{}}]}");

    Polygon<int> poly2{{{1, 1}, {3, 2}, {4, 3}, {6, 3}, {5, 1}, {1, 1}},
                       {{{1, 1}, {1, 2}, {2, 2}, {1, 1}}}};

    ss.str("");
    out.print(poly2, {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"Polygon\","
         "\"coordinates\":[[[5,1],[6,3],[4,3],[3,2],[1,1],[5,1]],[[1,1],[1,2],["
         "2,2],[1,1]]]},\"properties\":{}}");

    ss.str("");
    out.print(util::geo::MultiPolygon<int>{poly3, poly2}, {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPolygon\","
         "\"coordinates\":[[[[5,3],[6,5],[4,5],[3,4],[1,3],[5,3]]],[[[5,1],[6,"
         "3],[4,3],[3,2],[1,1],[5,1]],[[1,1],[1,2],[2,2],[1,1]]]]},"
         "\"properties\":{}}");

    ss.str("");
    out.print(geo::lineFromWKT<int>("LINESTRING(0 0, 1 1)"), {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"LineString\","
         "\"coordinates\":[[0,0],[1,1]]},\"properties\":{}}");

    ss.str("");
    out.print(
        util::geo::MultiLine<int>{
            geo::lineFromWKT<int>("LINESTRING(0 0, 1 1)"),
            geo::lineFromWKT<int>("LINESTRING(2 2, 3 3)")},
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\","
         "\"coordinates\":[[[0,0],[1,1]],[[2,2],[3,3]]]},\"properties\":{}}");

    ss.str("");
    out.print(
        geo::multiLineFromWKT<int>("MULTILINESTRING((0 0, 1 1), (2 2, 3 3))"),
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\","
         "\"coordinates\":[[[0,0],[1,1]],[[2,2],[3,3]]]},\"properties\":{}}");

    ss.str("");
    out.print(geo::multiPointFromWKT<int>("MULTIPOINT(0 0, 1 1, 2 2, 3 3)"),
              {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPoint\","
         "\"coordinates\":[[0,0],[1,1],[2,2],[3,3]]},\"properties\":{}}");

    ss.str("");
    out.print(
        geo::multiPointFromWKT<int>("MULTIPOINT((0 0), (1 1), (2 2), (3 3))"),
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPoint\","
         "\"coordinates\":[[0,0],[1,1],[2,2],[3,3]]},\"properties\":{}}");
  }

  {
    std::stringstream ss;
    util::geo::output::GeoJsonOutput out(ss);

    Polygon<int> poly{{{1, 1}, {3, 2}, {4, 3}, {6, 3}, {5, 1}, {1, 1}}};
    Polygon<int> poly3{{{1, 3}, {3, 4}, {4, 5}, {6, 5}, {5, 3}, {1, 3}}};

    out.print(poly, {});
    out.flush();
    std::string a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\","
         "\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[[5,1],[6,3],[4,"
         "3],[3,2],[1,1],[5,1]]]},\"properties\":{}}]}");

    Polygon<int> poly2{{{1, 1}, {3, 2}, {4, 3}, {6, 3}, {5, 1}, {1, 1}},
                       {{{1, 1}, {1, 2}, {2, 2}, {1, 1}}}};

    ss.str("");
    out.print(poly2, {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"Polygon\","
         "\"coordinates\":[[[5,1],[6,3],[4,3],[3,2],[1,1],[5,1]],[[1,1],[1,2],["
         "2,2],[1,1]]]},\"properties\":{}}");

    ss.str("");
    out.print(util::geo::MultiPolygon<int>{poly3, poly2}, {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPolygon\","
         "\"coordinates\":[[[[5,3],[6,5],[4,5],[3,4],[1,3],[5,3]]],[[[5,1],[6,"
         "3],[4,3],[3,2],[1,1],[5,1]],[[1,1],[1,2],[2,2],[1,1]]]]},"
         "\"properties\":{}}");

    ss.str("");
    out.print(geo::lineFromWKT<int>("LINESTRING(0 0, 1 1)"), {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"LineString\","
         "\"coordinates\":[[0,0],[1,1]]},\"properties\":{}}");

    ss.str("");
    out.print(
        util::geo::MultiLine<int>{
            geo::lineFromWKT<int>("LINESTRING(0 0, 1 1)"),
            geo::lineFromWKT<int>("LINESTRING(2 2, 3 3)")},
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\","
         "\"coordinates\":[[[0,0],[1,1]],[[2,2],[3,3]]]},\"properties\":{}}");

    ss.str("");
    out.print(
        geo::multiLineFromWKT<int>("MULTILINESTRING((0 0, 1 1), (2 2, 3 3))"),
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\","
         "\"coordinates\":[[[0,0],[1,1]],[[2,2],[3,3]]]},\"properties\":{}}");

    ss.str("");
    out.print(geo::multiPointFromWKT<int>("MULTIPOINT(0 0, 1 1, 2 2, 3 3)"),
              {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPoint\","
         "\"coordinates\":[[0,0],[1,1],[2,2],[3,3]]},\"properties\":{}}");

    ss.str("");
    out.print(
        geo::multiPointFromWKT<int>("MULTIPOINT((0 0), (1 1), (2 2), (3 3))"),
        {});
    out.flush();
    a = ss.str();
    replaceAll(a, " ", "");
    replaceAll(a, "\n", "");
    TEST(a, ==,
         "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPoint\","
         "\"coordinates\":[[0,0],[1,1],[2,2],[3,3]]},\"properties\":{}}");
  }

  // geometrycollections
  {
    util::geo::Collection<double> coll;
    util::geo::Collection<double> coll2;
    util::geo::Collection<double> coll3;
    util::geo::Collection<double> coll4;

    coll.push_back(util::geo::Point<double>{3, 2});
    TEST(util::geo::getWKT(coll), ==, "GEOMETRYCOLLECTION(POINT(3 2))");

    coll.push_back(util::geo::Line<double>{{3, 2}, {1, 1}});
    TEST(util::geo::getWKT(coll), ==,
         "GEOMETRYCOLLECTION(POINT(3 2),LINESTRING(3 2,1 1))");

    coll2.push_back(util::geo::Point<double>{3, 2});

    coll3.push_back(util::geo::Line<double>{{3, 2}, {1, 1}});

    coll4.push_back(util::geo::Point<double>{3, 2});
    coll4.push_back(util::geo::Point<double>{5, 4});

    TEST(util::geo::getWKT(util::geo::getBoundingBox(coll)), ==,
         "POLYGON((1 1,3 1,3 2,1 2,1 1))");
    TEST(util::geo::getWKT(util::geo::convexHull(coll)), ==,
         "POLYGON((1 1,3 2,1 1))");
    TEST(util::geo::dimension(coll), ==, 2);
    TEST(util::geo::getWKT(util::geo::centroid(coll)), ==, "POINT(2 1.5)");
    TEST(util::geo::getWKT(
             util::geo::convexHull(util::geo::getOrientedEnvelope(coll))),
         ==, "POLYGON((1 1,1 1,3 2,3 2,1 1))");

    TEST(util::geo::getWKT(util::geo::centroid(coll2)), ==, "POINT(3 2)");
    TEST(util::geo::getWKT(util::geo::centroid(coll3)), ==, "POINT(2 1.5)");
    TEST(util::geo::getWKT(util::geo::centroid(coll4)), ==, "POINT(4 3)");
  }

  // centroids
  {
    TEST(util::geo::getWKT(util::geo::centroid(
             util::geo::pointFromWKT<double>("POINT(3.4 4.6)"))),
         ==, "POINT(3.4 4.6)");
    TEST(util::geo::getWKT(util::geo::centroid(
             util::geo::lineFromWKT<double>("LINESTRING(0 0, 1 1)"))),
         ==, "POINT(0.5 0.5)");
    TEST(
        util::geo::getWKT(util::geo::centroid(util::geo::polygonFromWKT<double>(
            "POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))"))),
        ==, "POINT(0.5 0.5)");
    TEST(util::geo::getWKT(
             util::geo::centroid(util::geo::multiPolygonFromWKT<double>(
                 "MULTIPOLYGON(POLYGON((0 0,1 0,1 1,0 1,0 0)),POLYGON((0 0,1 "
                 "0,1 1,0 1,0 0)))"))),
         ==, "POINT(0.5 0.5)");

    TEST(util::geo::getWKT(
             util::geo::ringCentroid(util::geo::polygonFromWKT<double>(
                                         "POLYGON((5 5,5 10,10 10,10 5,5 5))")
                                         .getOuter())),
         ==, "POINT(7.5 7.5)");

    TEST(util::geo::getWKT(util::geo::ringCentroid(
             util::geo::polygonFromWKT<double>(
                 "POLYGON((0.5 0.5,0.5 1,1 1,1 0.5,0.5 0.5))")
                 .getOuter())),
         ==, "POINT(0.75 0.75)");
    TEST(
        util::geo::getWKT(util::geo::centroid(util::geo::polygonFromWKT<double>(
            "POLYGON((0.5 0.5,0.5 1,1 1,1 0.5,0.5 0.5))"))),
        ==, "POINT(0.75 0.75)");
    TEST(util::geo::getWKT(util::geo::ringCentroid(
             util::geo::polygonFromWKT<double>(
                 "POLYGON((0.5 0.5,1 0.5,1 1,0.5 1,0.5 0.5))")
                 .getOuter())),
         ==, "POINT(0.75 0.75)");
    TEST(
        util::geo::getWKT(util::geo::centroid(util::geo::polygonFromWKT<double>(
            "POLYGON((0.5 0.5,1 0.5,1 1,0.5 1,0.5 0.5))"))),
        ==, "POINT(0.75 0.75)");

    TEST(
        util::geo::getWKT(util::geo::centroid(util::geo::polygonFromWKT<double>(
            "POLYGON((0 0,1 0,1 1,0 1,0 0),(0.5 0.5,1 0.5,1 1,0.5 1,0.5 "
            "0.5),(0.5 0.5,0.5 0,1 0,1 0.5,0.5 0.5),(0 0.5,0.5 0.5,0.5 1, 0 "
            "1,0 0.5))"))),
        ==, "POINT(0.25 0.25)");
    TEST(util::geo::getWKT(util::geo::centroid(
             util::geo::polygonFromWKT<double>("POLYGON((0 0,1 1,0 0)"))),
         ==, "POINT(0.5 0.5)");

    TEST(util::geo::getWKT(util::geo::ringCentroid(
             util::geo::polygonFromWKT<double>(
                 "POLYGON((7.8386229 48.0081521,7.8386718 48.0081234,7.8387136 "
                 "48.0081553,7.8386647 48.008184,7.8386229 48.0081521))")
                 .getOuter())),
         ==, "POINT(7.838668 48.008154)");
  }
}

// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

// _____________________________________________________________________________
template <template <typename> class Geometry, typename T>
void GeoJsonOutput::print(const Geometry<T>& g, json::Val attrs) {
  _wr.obj();
  _wr.keyVal("type", "Feature");
  _wr.key("geometry");

  printGeom(g);

  _wr.key("properties");
  _wr.val(attrs);
  _wr.close();
}

// _____________________________________________________________________________
template <template <typename> class Geometry, typename T>
void GeoJsonOutput::print(const std::vector<Geometry<T>>& g, json::Val attrs) {
  _wr.obj();
  _wr.keyVal("type", "Feature");
  _wr.key("geometry");

  printGeom(g);

  _wr.key("properties");
  _wr.val(attrs);
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeom(const Point<T>& p) {
  _wr.obj();
  _wr.keyVal("type", "Point");
  _wr.key("coordinates");
  _wr.arr();
  _wr.val(p.getX());
  _wr.val(p.getY());
  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeom(const MultiPoint<T>& ps) {
  _wr.obj();
  _wr.keyVal("type", "MultiPoint");
  _wr.key("coordinates");
  _wr.arr();
  for (auto p : ps) {
    _wr.arr();
    _wr.val(p.getX());
    _wr.val(p.getY());
    _wr.close();
  }
  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeom(const Line<T>& line) {
  _wr.obj();
  _wr.keyVal("type", "LineString");
  _wr.key("coordinates");
  _wr.arr();
  for (auto p : line) {
    _wr.arr();
    _wr.val(p.getX());
    _wr.val(p.getY());
    _wr.close();
  }
  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeom(const MultiLine<T>& lines) {
  _wr.obj();
  _wr.keyVal("type", "MultiLineString");
  _wr.key("coordinates");
  _wr.arr();
  for (auto l : lines) {
    _wr.arr();
    for (auto p : l) {
      _wr.arr();
      _wr.val(p.getX());
      _wr.val(p.getY());
      _wr.close();
    }
    _wr.close();
  }
  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeom(const Polygon<T>& poly) {
  _wr.obj();
  _wr.keyVal("type", "Polygon");
  _wr.key("coordinates");
  _wr.arr();

  _wr.arr();
  for (auto p : poly.getOuter()) {
    _wr.arr();
    _wr.val(p.getX());
    _wr.val(p.getY());
    _wr.close();
  }

  // ensure ring is closed
  if (poly.getOuter().back() != poly.getOuter().front()) {
    _wr.arr();
    _wr.val(poly.getOuter().front().getX());
    _wr.val(poly.getOuter().front().getY());
    _wr.close();
  }
  _wr.close();

  for (const auto& inner : poly.getInners()) {
    _wr.arr();
    for (auto p : inner) {
      _wr.arr();
      _wr.val(p.getX());
      _wr.val(p.getY());
      _wr.close();
    }

    // ensure ring is closed
    if (inner.back() != inner.front()) {
      _wr.arr();
      _wr.val(inner.front().getX());
      _wr.val(inner.front().getY());
      _wr.close();
    }
    _wr.close();
  }

  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeom(const MultiPolygon<T>& mpoly) {
  _wr.obj();
  _wr.keyVal("type", "MultiPolygon");
  _wr.key("coordinates");

  _wr.arr();
  for (auto poly : mpoly) {
    _wr.arr();
    _wr.arr();
    for (auto p : poly.getOuter()) {
      _wr.arr();
      _wr.val(p.getX());
      _wr.val(p.getY());
      _wr.close();
    }
    // ensure ring is closed
    if (poly.getOuter().back() != poly.getOuter().front()) {
      _wr.arr();
      _wr.val(poly.getOuter().front().getX());
      _wr.val(poly.getOuter().front().getY());
      _wr.close();
    }
    _wr.close();

    for (const auto& inner : poly.getInners()) {
      _wr.arr();
      for (auto p : inner) {
        _wr.arr();
        _wr.val(p.getX());
        _wr.val(p.getY());
        _wr.close();
      }
      // ensure ring is closed
      if (inner.back() != inner.front()) {
        _wr.arr();
        _wr.val(inner.front().getX());
        _wr.val(inner.front().getY());
        _wr.close();
      }
      _wr.close();
    }
    _wr.close();
  }
  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeom(const Collection<T>& col) {
  _wr.obj();
  _wr.keyVal("type", "GeometryCollection");
  _wr.key("geometries");
  _wr.arr();

  for (const auto& g : col) {
    if (g.getType() == 0) printGeom(g.getPoint());
    if (g.getType() == 1) printGeom(g.getLine());
    if (g.getType() == 2) printGeom(g.getPolygon());
    if (g.getType() == 3) printGeom(g.getMultiLine());
    if (g.getType() == 4) printGeom(g.getMultiPolygon());
    if (g.getType() == 5) printGeom(g.getCollection());
    if (g.getType() == 6) printGeom(g.getMultiPoint());
  }

  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeomLatLng(const Point<T>& p) {
  auto projP = util::geo::webMercToLatLng<T>(p.getX(), p.getY());
  printGeom(projP);
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeomLatLng(const MultiPoint<T>& ps) {
  MultiPoint<T> projPs;
  for (auto p : ps)
    projPs.push_back(util::geo::webMercToLatLng<T>(p.getX(), p.getY()));

  printGeom(projPs);
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeomLatLng(const Line<T>& line) {
  Line<T> projL;
  for (auto p : line)
    projL.push_back(util::geo::webMercToLatLng<T>(p.getX(), p.getY()));

  printGeom(projL);
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeomLatLng(const MultiLine<T>& mline) {
  MultiLine<T> projLs;
  for (auto line : mline) {
    Line<T> projL;
    projL.reserve(line.size());
    for (auto p : line) {
      projL.push_back(util::geo::webMercToLatLng<T>(p.getX(), p.getY()));
    }
    projLs.push_back(projL);
  }

  printGeom(projLs);
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeomLatLng(const Polygon<T>& poly) {
  Polygon<T> projP;
  for (auto p : poly.getOuter())
    projP.getOuter().push_back(
        util::geo::webMercToLatLng<T>(p.getX(), p.getY()));

  for (auto pi : poly.getInners()) {
    Line<T> inner;
    for (auto p : pi) {
      inner.push_back(util::geo::webMercToLatLng<T>(p.getX(), p.getY()));
    }
    projP.getInners().push_back(inner);
  }

  printGeom(projP);
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeomLatLng(const MultiPolygon<T>& mpoly) {
  MultiPolygon<T> projPolys;
  for (auto poly : mpoly) {
    Polygon<T> projP;
    for (auto p : poly.getOuter()) {
      projP.getOuter().push_back(
          util::geo::webMercToLatLng<T>(p.getX(), p.getY()));
    }
    for (auto pi : poly.getInners()) {
      Line<T> inner;
      for (auto p : pi) {
        inner.push_back(util::geo::webMercToLatLng<T>(p.getX(), p.getY()));
      }
      projP.getInners().push_back(inner);
    }
    projPolys.push_back(projP);
  }

  printGeom(projPolys);
}

// _____________________________________________________________________________
template <template <typename> class Geometry, typename T>
void GeoJsonOutput::printLatLng(const Geometry<T>& g, json::Val attrs) {
  _wr.obj();
  _wr.keyVal("type", "Feature");
  _wr.key("geometry");

  printGeomLatLng(g);

  _wr.key("properties");
  _wr.val(attrs);
  _wr.close();
}

// _____________________________________________________________________________
template <typename T>
void GeoJsonOutput::printGeomLatLng(const Collection<T>& col) {
  if (!col.size()) return;
  _wr.obj();
  _wr.keyVal("type", "GeometryCollection");
  _wr.key("geometries");
  _wr.arr();

  for (const auto& g : col) {
    if (g.getType() == 0) printGeomLatLng(g.getPoint());
    if (g.getType() == 1) printGeomLatLng(g.getLine());
    if (g.getType() == 2) printGeomLatLng(g.getPolygon());
    if (g.getType() == 3) printGeomLatLng(g.getMultiLine());
    if (g.getType() == 4) printGeomLatLng(g.getMultiPolygon());
    if (g.getType() == 5) printGeomLatLng(g.getCollection());
    if (g.getType() == 6) printGeomLatLng(g.getMultiPoint());
  }

  _wr.close();
  _wr.close();
}

// _____________________________________________________________________________
template <template <typename> class Geometry, typename T>
void GeoJsonOutput::printLatLng(const std::vector<Geometry<T>>& g,
                                json::Val attrs) {
  _wr.obj();
  _wr.keyVal("type", "Feature");
  _wr.key("geometry");

  printGeomLatLng(g);

  _wr.key("properties");
  _wr.val(attrs);
  _wr.close();
}

// Copyright 2016
// Author: Patrick Brosi
//

#include <string>
#include <clocale>
#include "util/Misc.h"
#include "util/Test.h"
#include "util/Nullable.h"
#include "util/String.h"
#include "util/tests/QuadTreeTest.h"
#include "util/tests/GeoTest.h"
#include "util/graph/Algorithm.h"
#include "util/graph/Dijkstra.h"
#include "util/graph/BiDijkstra.h"
#include "util/graph/DirGraph.h"
#include "util/graph/EDijkstra.h"
#include "util/graph/UndirGraph.h"
#include "util/json/Writer.h"

using namespace util;
using namespace util::graph;

using util::approx;

// _____________________________________________________________________________
int main(int argc, char** argv) {
	UNUSED(argc);
	UNUSED(argv);

  std::setlocale(LC_ALL, "en_US.utf8");
  TEST(strncicmp("LINESTRING", "LINESTRING(0 0, 1 1))", 10), ==, 0);



  QuadTreeTest quadTreeTest;
  quadTreeTest.run();

  GeoTest geoTest;
  geoTest.run();


  // ___________________________________________________________________________
  {
    TEST(util::btsSimi("", ""), ==, approx(1));
    TEST(util::btsSimi("Hallo", "Test"), ==, approx(0));
    TEST(util::btsSimi("Test", "Hallo"), ==, approx(0));
    TEST(util::btsSimi("Test", "Test"), ==, approx(1));
    TEST(util::btsSimi("Milner Road / Wandlee Road", "Wandlee Road"), ==, approx(1));
    TEST(util::btsSimi("bla blubb blob", "blubb blib"), ==, approx(0.9));
    TEST(util::btsSimi("St Pancras International", "London St Pancras"), ==, approx(0.588235));
    TEST(util::btsSimi("Reiterstrasse", "Reiterstrasse Freiburg im Breisgau"), ==, approx(1));
    TEST(util::btsSimi("Reiterstrasse", "Reiter Freiburg im Breisgau"), ==, approx(.53333333));
    TEST(util::btsSimi("AA", "Reiterstrasse, Freiburg im Breisgau"), ==, approx(0));
    TEST(util::btsSimi("blibb blabbel bla blubb blob", "blubb blib blabb"), ==, approx(0.875));
    TEST(util::btsSimi("blibb blabbel bla blubb blobo", "blubb blib blabb blabo"), ==, approx(0.84));
    TEST(util::btsSimi("blubb blib blabb", "blibb blabbel bla blubb blob"), ==, approx(0.875));
    TEST(util::btsSimi("blubbb blib blabb blobo", "blibb blabbel bla blubb blobo"), ==, approx(0.84));
    TEST(util::btsSimi("Reiter Freiburg im Breisgau", "Reiter Frei burg im Brei sgau"), ==, approx(0.931034));
    // fallback to jaccard
    TEST(util::btsSimi("Freiburg im Breisgau, Germany, Main Railway Station", "Main Railway Station Freiburg im Breisgau, Germany"), ==, approx(1));

  }

  // ___________________________________________________________________________
  {
    std::string test = "Zuerich, Hauptbahnhof (Nord)";
    auto tokens = util::tokenize(test);

    TEST(tokens.size(), ==, 3);

    TEST(util::jaccardSimi("Zuerich Hauptbahnhof Nord", "Zuerich, Hauptbahnhof (Nord)"), ==, approx(1));
    TEST(util::jaccardSimi("Zuerich Hauptbahnhof", "Zuerich, Hauptbahnhof ()"), ==, approx(1));
    TEST(util::jaccardSimi("Zuerich Hauptbahnhof", "Zuerich, Hauptbahnhof (Nord)"), ==, approx(2./3.));
  }

  // ___________________________________________________________________________
  {
    TEST(util::atof("45.534215"), ==, approx(45.534215));
    TEST(util::atof("5.534"), ==, approx(5.534));
    TEST(util::atof("534"), ==, approx(534));
    TEST(util::atof("-534"), ==, approx(-534));
    TEST(util::atof("-45.534215"), ==, approx(-45.534215));
    TEST(util::atof("-45.534215", 2), ==, approx(-45.53));
    TEST(util::atof("-45.53421544345"), ==, approx(-45.5342154435));

    // TODO: more test cases
  }

  // ___________________________________________________________________________
  {
    std::stringstream ss;
    util::json::Writer wr(&ss, 2, false);

    util::json::Val a("bla");
    util::json::Val b(1);
    util::json::Val c(1.0);
    util::json::Val d("a");
    util::json::Val e({"a", "b", "c"});

    util::json::Val f({1, json::Array{2, 3, 4}, 3});

    ss.str("");
    wr = util::json::Writer(&ss, 2, false);
    util::json::Val i({1, json::Array{2, json::Null(), 4}, true});
    wr.val(i);
    wr.closeAll();
    TEST(ss.str(), ==, "[1,[2,null,4],true]");

    ss.str("");
    wr = util::json::Writer(&ss, 2, false);
    i = util::json::Val({1, json::Array{2, json::Null(), 4}, false});
    wr.val(i);
    wr.closeAll();
    TEST(ss.str(), ==, "[1,[2,null,4],false]");

    ss.str("");
    wr = util::json::Writer(&ss, 2, false);
    i = util::json::Val({1, json::Array{2, json::Null(), 4}, false});
    wr.val(i);
    wr.closeAll();
    TEST(ss.str(), ==, "[1,[2,null,4],false]");

    ss.str("");
    wr = util::json::Writer(&ss, 2, false);
    i = util::json::Val({1, json::Array{2.13, "", 4}, 0});
    wr.val(i);
    wr.closeAll();
    TEST(ss.str(), ==, "[1,[2.13,\"\",4],0]");

    ss.str("");
    wr = util::json::Writer(&ss, 2, false);
    i = util::json::Val(
        {1, json::Array{2.13, json::Dict{{"a", 1}, {"B", 2.123}}, 4}, 0});
    wr.val(i);
    wr.closeAll();
    TEST((ss.str() == "[1,[2.13,{\"a\":1,\"B\":2.12},4],0]" ||
            ss.str() == "[1,[2.13,{\"B\":2.12,\"a\":1},4],0]"));
  }

  // ___________________________________________________________________________
  {
    DirGraph<int, int> g;

    DirNode<int, int>* a = new DirNode<int, int>(0);
    DirNode<int, int>* b = new DirNode<int, int>(0);
    g.addNd(a);
    TEST(g.getNds().size(), ==, (size_t)1);
    g.addNd(b);
    TEST(g.getNds().size(), ==, (size_t)2);

    g.addEdg(a, b);
    TEST(a->getDeg(), ==, (size_t)1);
    TEST(b->getDeg(), ==, (size_t)0);

    auto c = g.addNd();

    g.addEdg(a, c);
    g.addEdg(c, b);
    TEST(a->getDeg(), ==, (size_t)2);
    TEST(b->getDeg(), ==, (size_t)0);
    TEST(c->getDeg(), ==, (size_t)1);

    g.delEdg(a, c);

    TEST(a->getDeg(), ==, (size_t)1);
    TEST(b->getDeg(), ==, (size_t)0);
    TEST(c->getDeg(), ==, (size_t)1);

    g.addEdg(a, a);
    TEST(a->getDeg(), ==, (size_t)2);

    g.delEdg(a, a);
    TEST(a->getDeg(), ==, (size_t)1);

    g.delEdg(a, a);
    TEST(a->getDeg(), ==, (size_t)1);

    // TODO: more test cases
  }

  // ___________________________________________________________________________
  {
    UndirGraph<int, int> g;

    UndirNode<int, int>* a = new UndirNode<int, int>(0);
    UndirNode<int, int>* b = new UndirNode<int, int>(0);
    g.addNd(a);
    TEST(g.getNds().size(), ==, (size_t)1);
    g.addNd(b);
    TEST(g.getNds().size(), ==, (size_t)2);

    g.addEdg(a, b);
    TEST(a->getDeg(), ==, (size_t)1);
    TEST(b->getDeg(), ==, (size_t)1);

    auto c = g.addNd();

    g.addEdg(a, c);
    g.addEdg(c, b);
    TEST(a->getDeg(), ==, (size_t)2);
    TEST(b->getDeg(), ==, (size_t)2);
    TEST(c->getDeg(), ==, (size_t)2);

    g.delEdg(a, c);

    TEST(a->getDeg(), ==, (size_t)1);
    TEST(b->getDeg(), ==, (size_t)2);
    TEST(c->getDeg(), ==, (size_t)1);

    g.delNd(b);

    TEST(a->getDeg(), ==, (size_t)0);
    TEST(c->getDeg(), ==, (size_t)0);

    g.addEdg(a, a);
    TEST(a->getDeg(), ==, (size_t)1);

    g.delEdg(a, a);
    TEST(a->getDeg(), ==, (size_t)0);

    // TODO: more test cases
  }


  // ___________________________________________________________________________
  {
    TEST("zürich", ==, util::urlDecode("z%C3%BCrich"));
    TEST("!@$%^*()", ==, util::urlDecode("!%40%24%25%5E*()"));
    TEST("Løkken", ==, util::urlDecode("L%C3%B8kken"));
    TEST("á é", ==, util::urlDecode("%C3%A1%20%C3%A9"));
    TEST("á é", ==, util::urlDecode("%C3%A1+%C3%A9"));
  }

  // ___________________________________________________________________________
  {
    TEST("Hello\\\\Goodbye!" == util::jsonStringEscape("Hello\\Goodbye!"));
    TEST("\\\"Hello\\\"" == util::jsonStringEscape("\"Hello\""));
  }

  // ___________________________________________________________________________
  {
    TEST(util::split("hello,again", ',').size(), ==, (size_t)2);
    TEST(util::split("hello,,again", ',').size(), ==, (size_t)3);
    TEST(util::split("hello,,again", ',')[0], ==, "hello");
    TEST(util::split("hello,,again", ',')[1], ==, "");
    TEST(util::split("hello,,again", ',')[2], ==, "again");
    TEST(util::split("hello", ',').size(), ==, (size_t)1);
    TEST(util::split("", ',').size(), ==, (size_t)0);
  }

  // ___________________________________________________________________________
  {
    TEST(util::split("hello,again", ',', 0).size(), ==, (size_t)0);
    TEST(util::split("hello,again", ',', 1).size(), ==, (size_t)1);
    TEST(util::split("hello,again", ',', 2).size(), ==, (size_t)2);
    TEST(util::split("hello,,again", ',', 2).size(), ==, (size_t)2);
    TEST(util::split("hello,,again", ',', 3).size(), ==, (size_t)3);
    TEST(util::split("hello,,again", ',', -1).size(), ==, (size_t)3);
    TEST(util::split("hello,,again", ',', 2)[0], ==, "hello");
    TEST(util::split("hello,,again", ',', 2)[1], ==, ",again");
    TEST(util::split("hello,,again", ',', 1).size(), ==, (size_t)1);
    TEST(util::split("hello,,again", ',', 1)[0], ==, "hello,,again");
    TEST(util::split("hello", ',').size(), ==, (size_t)1);
  }

  // ___________________________________________________________________________
  {
    TEST(util::editDist("hello", "mello"), ==, (size_t)1);
    TEST(util::editDist("mello", "hello"), ==, (size_t)1);
    TEST(util::editDist("abcde", "abfde"), ==, (size_t)1);
    TEST(util::editDist("abcd", "abcde"), ==, (size_t)1);
    TEST(util::editDist("xabcd", "abcde"), ==, (size_t)2);
    TEST(util::editDist("abcd", "abcdes"), ==, (size_t)2);
    TEST(util::editDist("hello", "hello"), ==, (size_t)0);
  }

  // ___________________________________________________________________________
  {
    TEST(util::prefixEditDist("hello", "hello", 0), ==, (size_t)0);
    TEST(util::prefixEditDist("hello", "hello", 100), ==, (size_t)0);
    TEST(util::prefixEditDist("hello", "hello"), ==, (size_t)0);
    TEST(util::prefixEditDist("hel", "hello"), ==, (size_t)0);
    TEST(util::prefixEditDist("hel", "hello", 0), ==, (size_t)0);
    TEST(util::prefixEditDist("hel", "hello", 1), ==, (size_t)0);
    TEST(util::prefixEditDist("hel", "hello", 2), ==, (size_t)0);
    TEST(util::prefixEditDist("hal", "hello", 2), ==, (size_t)1);
    TEST(util::prefixEditDist("hal", "hello", 1), ==, (size_t)1);
    TEST(util::prefixEditDist("hal", "hello", 0), >, (size_t)0);
    TEST(util::prefixEditDist("fel", "hello", 0), >, (size_t)0);
    TEST(util::prefixEditDist("fel", "hello", 1), ==, (size_t)1);
    TEST(util::prefixEditDist("fel", "hello", 2), ==, (size_t)1);
    TEST(util::prefixEditDist("fal", "hello", 2), ==, (size_t)2);
    TEST(util::prefixEditDist("fal", "hello", 1), >, (size_t)1);
    TEST(util::prefixEditDist("fal", "hello", 0), >, (size_t)0);
    TEST(util::prefixEditDist("far", "hello", 0), >, (size_t)0);
    TEST(util::prefixEditDist("far", "hello", 1), >, (size_t)1);
    TEST(util::prefixEditDist("far", "hello", 2), >, (size_t)2);
    TEST(util::prefixEditDist("far", "hello", 3), ==, (size_t)3);
    TEST(util::prefixEditDist("far", "hello", 4), ==, (size_t)3);
    TEST(util::prefixEditDist("far", "hello"), ==, (size_t)3);
    TEST(util::prefixEditDist("hefar", "hello"), ==, (size_t)3);
    TEST(util::prefixEditDist("hefaree", "hello"), ==, (size_t)5);
    TEST(util::prefixEditDist("helloo", "hello"), ==, (size_t)1);
    TEST(util::prefixEditDist("helloo", "hello", 0), >, (size_t)0);
    TEST(util::prefixEditDist("helloo", "hello", 1), ==, (size_t)1);
    TEST(util::prefixEditDist("helloo", "hello", 2), ==, (size_t)1);
    TEST(util::prefixEditDist("", "hello", 2), ==, (size_t)0);
    TEST(util::prefixEditDist("e", "hello", 2), ==, (size_t)1);
    TEST(util::prefixEditDist("el", "hello", 2), ==, (size_t)1);
    TEST(util::prefixEditDist("ello", "hello", 2), ==, (size_t)1);
    TEST(util::prefixEditDist("hell", "hello", 2), ==, (size_t)0);
    TEST(util::prefixEditDist("hell", "", 2), >, (size_t)2);
    TEST(util::prefixEditDist("hell", ""), ==, (size_t)4);
  }

  // ___________________________________________________________________________
  {
    TEST(util::toString(34) == "34");
    TEST(util::toString("34") == "34");
  }

  // ___________________________________________________________________________
  {
    std::string a("lorem ipsum ipsum lorem");

    TEST(util::replace(a, "ips", "aa"));
    TEST(a, ==, "lorem aaum ipsum lorem");

    TEST(!util::replace(a, "blablabla", ""));
    TEST(a, ==, "lorem aaum ipsum lorem");

    TEST(util::replace(a, "m", ""));
    TEST(a, ==, "lore aaum ipsum lorem");

    TEST(!util::replace(a, "", ""));
    TEST(a, ==, "lore aaum ipsum lorem");

    std::string b("lorem ipsum ipsum lorem");
    TEST(util::replaceAll(b, "ips", "aa"));
    TEST(b, ==, "lorem aaum aaum lorem");

    TEST(util::replaceAll(b, "m", ""));
    TEST(b, ==, "lore aau aau lore");

    TEST(util::replaceAll(b, "a", "aa"));
    TEST(b, ==, "lore aaaau aaaau lore");

    TEST(util::replaceAll(b, "e", "e"));
    TEST(b, ==, "lore aaaau aaaau lore");

    TEST(util::replaceAll(b, "e", "ee"));
    TEST(b, ==, "loree aaaau aaaau loree");

    TEST(!util::replaceAll(b, "", "ee"));
    TEST(b, ==, "loree aaaau aaaau loree");
  }

  // ___________________________________________________________________________
  {
    UndirGraph<std::string, int> g;

    auto a = g.addNd("A");
    auto b = g.addNd("B");
    auto c = g.addNd("C");
    auto d = g.addNd("D");
    auto e = g.addNd("E");

    g.addEdg(a, c, 1);
    g.addEdg(a, b, 5);
    g.addEdg(d, c, 1);
    g.addEdg(d, b, 3);
    g.addEdg(e, d, 1);
    g.addEdg(e, b, 1);

    auto comps = util::graph::Algorithm::connectedComponents(g);

    TEST(comps.size(), ==, static_cast<size_t>(1));
    TEST(comps[0].count(a));
    TEST(comps[0].count(b));
    TEST(comps[0].count(c));
    TEST(comps[0].count(d));
    TEST(comps[0].count(e));

    auto f = g.addNd("F");
    comps = util::graph::Algorithm::connectedComponents(g);
    TEST(comps.size(), ==, static_cast<size_t>(2));

    auto gn = g.addNd("G");
    comps = util::graph::Algorithm::connectedComponents(g);
    TEST(comps.size(), ==, static_cast<size_t>(3));

    g.addEdg(f, gn, 1);
    comps = util::graph::Algorithm::connectedComponents(g);
    TEST(comps.size(), ==, static_cast<size_t>(2));

    g.addEdg(f, a, 1);
    comps = util::graph::Algorithm::connectedComponents(g);
    TEST(comps.size(), ==, static_cast<size_t>(1));
  }

  // ___________________________________________________________________________
  {
    DirGraph<std::string, int> g;

    auto a = g.addNd("A");
    auto b = g.addNd("B");
    auto c = g.addNd("C");
    auto d = g.addNd("D");
    auto e = g.addNd("E");

    auto eAC = g.addEdg(a, c, 1);
    auto eAB = g.addEdg(a, b, 5);
    auto eDC = g.addEdg(d, c, 1);
    auto eDB = g.addEdg(d, b, 3);
    auto eED = g.addEdg(e, d, 1);
    auto eEB = g.addEdg(e, b, 1);

    UNUSED(eAC);
    UNUSED(eDC);
    UNUSED(eDB);
    UNUSED(eED);
    UNUSED(eEB);

    struct CostFunc : public EDijkstra::CostFunc<std::string, int, int> {
      using EDijkstra::CostFunc<std::string, int, int>::operator();

      int operator()(const Edge<std::string, int>* from,
                     const Node<std::string, int>* n,
                     const Edge<std::string, int>* to) const {
        UNUSED(from);

        // dont count cost of start edge
        if (n) return to->pl();
        return 0;
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    auto cost = EDijkstra::shortestPath(eAB, cFunc);

    for (auto u : cost) {
      int single = EDijkstra::shortestPath(eAB, u.first, cFunc);
      TEST(single, ==, u.second);
    }

    // all to 1
    auto eBC = g.addEdg(b, c, 10);

    auto costb = EDijkstra::shortestPathRev(eBC, cFunc);
    for (auto u : costb) {
      int single = EDijkstra::shortestPath(u.first, eBC, cFunc);
      TEST(single, ==, u.second);
    }
  }

  // ___________________________________________________________________________
  {
    UndirGraph<std::string, int> g;

    auto a = g.addNd("A");
    auto b = g.addNd("B");
    auto c = g.addNd("C");
    auto d = g.addNd("D");
    auto e = g.addNd("E");

    auto eAC = g.addEdg(a, c, 1);
    auto eAB = g.addEdg(a, b, 5);
    auto eDC = g.addEdg(d, c, 1);
    auto eDB = g.addEdg(d, b, 3);
    auto eED = g.addEdg(e, d, 1);
    auto eEB = g.addEdg(e, b, 1);

    UNUSED(eAC);
    UNUSED(eDC);
    UNUSED(eDB);
    UNUSED(eED);
    UNUSED(eEB);

    struct CostFunc : public EDijkstra::CostFunc<std::string, int, int> {
      using EDijkstra::CostFunc<std::string, int, int>::operator();

      int operator()(const Edge<std::string, int>* from,
                     const Node<std::string, int>* n,
                     const Edge<std::string, int>* to) const {
        UNUSED(from);

        // dont count cost of start edge
        if (n) return to->pl();
        return 0;
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    EDijkstra::NList<std::string, int> res;
    EDijkstra::EList<std::string, int> resE;
    int cost = EDijkstra::shortestPath(eAB, d, cFunc, &resE, &res);

    TEST(cost, ==, 2);

    TEST(resE.size(), ==, (size_t)3);
    TEST(res.size(), ==, (size_t)3);
    TEST((*(res.rbegin()))->pl(), ==, "A");
    TEST((*(++res.rbegin()))->pl(), ==, "C");
    TEST((*(++++res.rbegin()))->pl(), ==, "D");

    TEST((*(resE.rbegin())), ==, eAB);
    TEST((*(++resE.rbegin())), ==, eAC);
    TEST((*(++++resE.rbegin())), ==, eDC);

    cost = EDijkstra::shortestPath(eAB, b, cFunc, &resE, &res);
    TEST(cost, ==, 0);
  }

  // ___________________________________________________________________________
  {
    UndirGraph<std::string, int> g;

    auto a = g.addNd("A");
    auto b = g.addNd("B");
    auto c = g.addNd("C");
    auto d = g.addNd("D");
    auto e = g.addNd("E");

    auto eAC = g.addEdg(a, c, 1);
    auto eAB = g.addEdg(a, b, 5);
    auto eDC = g.addEdg(d, c, 1);
    auto eDB = g.addEdg(d, b, 3);
    auto eED = g.addEdg(e, d, 1);
    auto eEB = g.addEdg(e, b, 1);

    UNUSED(eAC);
    UNUSED(eDC);
    UNUSED(eDB);
    UNUSED(eED);
    UNUSED(eEB);

    struct CostFunc : public EDijkstra::CostFunc<std::string, int, int> {
      using EDijkstra::CostFunc<std::string, int, int>::operator();

      int operator()(const Edge<std::string, int>* from,
                     const Node<std::string, int>* n,
                     const Edge<std::string, int>* to) const {
        UNUSED(from);

        // dont count cost of start edge
        if (n) return to->pl();
        return 0;
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    std::set<Node<std::string, int>*> tos;
    tos.insert(d);
    tos.insert(b);
    tos.insert(b);

    EDijkstra::NList<std::string, int> res;
    EDijkstra::EList<std::string, int> resE;
    int cost = EDijkstra::shortestPath(eAB, tos, cFunc, &resE, &res);
    TEST(cost, ==, 0);
  }

  // ___________________________________________________________________________
  {
    UndirGraph<std::string, int> g;

    auto a = g.addNd("A");
    auto b = g.addNd("B");
    auto c = g.addNd("C");
    auto d = g.addNd("D");
    auto e = g.addNd("E");

    g.addEdg(a, c, 1);
    auto eAB = g.addEdg(a, b, 5);
    auto eDC = g.addEdg(d, c, 1);
    g.addEdg(d, b, 3);
    auto eED = g.addEdg(e, d, 1);
    g.addEdg(e, b, 1);

    struct CostFunc : public EDijkstra::CostFunc<std::string, int, int> {
      using EDijkstra::CostFunc<std::string, int, int>::operator();

      int operator()(const Edge<std::string, int>* from,
                     const Node<std::string, int>* n,
                     const Edge<std::string, int>* to) const {
        UNUSED(from);

        // dont count cost of start edge
        if (n) return to->pl();
        return 0;
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    std::set<Edge<std::string, int>*> tos;
    tos.insert(eDC);
    tos.insert(eED);

    std::unordered_map<Edge<std::string, int>*,
                       EDijkstra::EList<std::string, int>*>
        resE;
    resE[eDC] = new EDijkstra::EList<std::string, int>();
    resE[eED] = new EDijkstra::EList<std::string, int>();
    std::unordered_map<Edge<std::string, int>*,
                       EDijkstra::NList<std::string, int>*>
        res;
    res[eDC] = new EDijkstra::NList<std::string, int>();
    res[eED] = new EDijkstra::NList<std::string, int>();
    auto hFunc = ZeroHeurFunc<std::string, int, int>();
    std::unordered_map<Edge<std::string, int>*, int> cost =
        EDijkstra::shortestPath(eAB, tos, cFunc, hFunc, resE, res);

    TEST(cost[eDC], ==, 2);
    TEST(cost[eED], ==, 2);

    TEST(resE[eDC]->size(), ==, (size_t)3);
    TEST(res[eED]->size(), ==, (size_t)3);

    TEST(resE[eDC]->size(), ==, (size_t)3);
    TEST(res[eED]->size(), ==, (size_t)3);
  }

  // ___________________________________________________________________________
  {
    UndirGraph<std::string, int> g;

    auto a = g.addNd("A");
    auto b = g.addNd("B");
    auto c = g.addNd("C");
    auto d = g.addNd("D");
    auto e = g.addNd("E");

    g.addEdg(a, c, 1);
    g.addEdg(a, b, 5);
    g.addEdg(d, c, 1);
    g.addEdg(d, b, 3);
    g.addEdg(e, d, 1);
    g.addEdg(e, b, 1);

    struct CostFunc : public EDijkstra::CostFunc<std::string, int, int> {
      using EDijkstra::CostFunc<std::string, int, int>::operator();

      int operator()(const Edge<std::string, int>* fr,
                     const Node<std::string, int>* n,
                     const Edge<std::string, int>* to) const {
        UNUSED(fr);
        UNUSED(n);
        return to->pl();
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    EDijkstra::NList<std::string, int> res;
    EDijkstra::EList<std::string, int> resE;
    int cost = EDijkstra::shortestPath(a, b, cFunc, &resE, &res);

    TEST(res.size(), ==, (size_t)5);
    TEST((*(res.rbegin()))->pl(), ==, "A");
    TEST((*(++res.rbegin()))->pl(), ==, "C");
    TEST((*(++++res.rbegin()))->pl(), ==, "D");
    TEST((*(++++++res.rbegin()))->pl(), ==, "E");
    TEST((*(++++++++res.rbegin()))->pl(), ==, "B");
    TEST(cost, ==, 4);
    TEST((*(resE.rbegin()))->getFrom()->pl(), ==, "A");
    TEST((*(++resE.rbegin()))->getFrom()->pl(), ==, "D");
    TEST((*(++++resE.rbegin()))->getFrom()->pl(), ==, "E");
    TEST((*(++++++resE.rbegin()))->getTo()->pl(), ==, "B");

    TEST(resE.size(), ==, (size_t)4);

    cost = EDijkstra::shortestPath(d, b, cFunc, &res);
    TEST(cost, ==, 2);

    cost = EDijkstra::shortestPath(b, d, cFunc, &res);
    TEST(cost, ==, 2);

    cost = EDijkstra::shortestPath(e, b, cFunc, &res);
    TEST(cost, ==, 1);

    cost = EDijkstra::shortestPath(b, e, cFunc, &res);
    TEST(cost, ==, 1);

    cost = EDijkstra::shortestPath(b, a, cFunc, &res);
    TEST(cost, ==, 4);

    cost = EDijkstra::shortestPath(c, a, cFunc, &res);
    TEST(cost, ==, 1);

    cost = EDijkstra::shortestPath(a, c, cFunc, &res);
    TEST(cost, ==, 1);

    cost = EDijkstra::shortestPath(a, d, cFunc, &res);
    TEST(cost, ==, 2);
  }

  // ___________________________________________________________________________
  {
    DirGraph<int, int> g;

    DirNode<int, int>* a = new DirNode<int, int>(1);
    DirNode<int, int>* b = new DirNode<int, int>(4);
    g.addNd(a);
    g.addNd(b);

    auto c = g.addNd(2);
    auto d = g.addNd(3);
    auto x = g.addNd();

    g.addEdg(a, d, 4);
    g.addEdg(a, c, 1);
    g.addEdg(c, b, 1);
    g.addEdg(b, d, 1);

    struct CostFunc : public EDijkstra::CostFunc<int, int, int> {
      using EDijkstra::CostFunc<int, int, int>::operator();

      int operator()(const Edge<int, int>* fr, const Node<int, int>* n,
                     const Edge<int, int>* to) const {
        UNUSED(fr);
        UNUSED(n);
        return to->pl();
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    EDijkstra::NList<int, int> res;
    int cost = EDijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 3);

    g.addEdg(c, d, 3);
    cost = EDijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 3);

    g.addEdg(a, b, 1);
    g.addEdg(x, a, 1);
    cost = EDijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 2);
  }

  // ___________________________________________________________________________
  {
    DirGraph<int, int> g;

    auto source = g.addNd();
    auto target = g.addNd();
    auto a = g.addNd();
    auto b = g.addNd();

    g.addEdg(source, a, 4);
    g.addEdg(source, b, 5);
    g.addEdg(a, target, 3);
    g.addEdg(b, target, 1);

    struct CostFunc : public BiDijkstra::CostFunc<int, int, int> {
      using BiDijkstra::CostFunc<int, int, int>::operator();

      int operator()(const Node<int, int>* fr, const Edge<int, int>* e,
                     const Node<int, int>* to) const {
        UNUSED(fr);
        UNUSED(to);
        return e->pl();
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    BiDijkstra::NList<int, int> res;
    int cost = BiDijkstra::shortestPath(source, target, cFunc, &res);

    TEST(cost, ==, 6);
  }

  // ___________________________________________________________________________
  {
    DirGraph<int, int> g;

    DirNode<int, int>* a = new DirNode<int, int>(1);
    DirNode<int, int>* b = new DirNode<int, int>(0);
    g.addNd(a);
    g.addNd(b);

    auto c = g.addNd();
    auto d = g.addNd(4);
    auto x = g.addNd();

    g.addEdg(a, d, 4);
    g.addEdg(a, c, 1);
    g.addEdg(c, b, 1);
    g.addEdg(b, d, 1);

    struct CostFunc : public BiDijkstra::CostFunc<int, int, int> {
      using BiDijkstra::CostFunc<int, int, int>::operator();

      int operator()(const Node<int, int>* fr, const Edge<int, int>* e,
                     const Node<int, int>* to) const {
        UNUSED(fr);
        UNUSED(to);
        return e->pl();
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    BiDijkstra::NList<int, int> res;
    int cost = BiDijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 3);
    TEST(res.size(), ==, (size_t)4);

    g.addEdg(c, d, 3);
    cost = BiDijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 3);

    g.addEdg(a, b, 1);
    g.addEdg(x, a, 1);
    cost = BiDijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 2);

    // const std::set<Node<int, int>*> to{b, c, d, x};
    // std::unordered_map<Node<int, int>*, BiDijkstra::EList<int, int>*> resEdges;
    // std::unordered_map<Node<int, int>*, BiDijkstra::NList<int, int>*> resNodes;

    // for (auto n : to) {
      // resEdges[n] = new BiDijkstra::EList<int, int>();
      // resNodes[n] = new BiDijkstra::NList<int, int>();
    // }

    // auto costs = BiDijkstra::shortestPath(a, to, cFunc, resEdges, resNodes);

    // TEST(costs[b], ==, 1);
    // TEST(costs[c], ==, 1);
    // TEST(costs[d], ==, 2);
    // TEST(costs[x], ==, 999);
  }

  // ___________________________________________________________________________
  {
    DirGraph<int, int> g;

    DirNode<int, int>* a = new DirNode<int, int>(1);
    DirNode<int, int>* b = new DirNode<int, int>(0);
    g.addNd(a);
    g.addNd(b);

    auto c = g.addNd();
    auto d = g.addNd(4);
    auto x = g.addNd();

    g.addEdg(a, d, 4);
    g.addEdg(a, c, 1);
    g.addEdg(c, b, 1);
    g.addEdg(b, d, 1);

    struct CostFunc : public Dijkstra::CostFunc<int, int, int> {
      using Dijkstra::CostFunc<int, int, int>::operator();

      int operator()(const Node<int, int>* fr, const Edge<int, int>* e,
                     const Node<int, int>* to) const {
        UNUSED(fr);
        UNUSED(to);
        return e->pl();
      };
      int inf() const { return 999; };
    };

    CostFunc cFunc;

    Dijkstra::NList<int, int> res;
    int cost = Dijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 3);
    TEST(res.size(), ==, (size_t)4);

    g.addEdg(c, d, 3);
    cost = Dijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 3);

    g.addEdg(a, b, 1);
    g.addEdg(x, a, 1);
    cost = Dijkstra::shortestPath(a, d, cFunc, &res);

    TEST(cost, ==, 2);

    const std::set<Node<int, int>*> to{b, c, d, x};
    std::unordered_map<Node<int, int>*, Dijkstra::EList<int, int>*> resEdges;
    std::unordered_map<Node<int, int>*, Dijkstra::NList<int, int>*> resNodes;

    for (auto n : to) {
      resEdges[n] = new Dijkstra::EList<int, int>();
      resNodes[n] = new Dijkstra::NList<int, int>();
    }

    auto costs = Dijkstra::shortestPath(a, to, cFunc, resEdges, resNodes);

    TEST(costs[b], ==, 1);
    TEST(costs[c], ==, 1);
    TEST(costs[d], ==, 2);
    TEST(costs[x], ==, 999);
  }

  // ___________________________________________________________________________
  {{util::Nullable<std::string> nullable;
  TEST(nullable.isNull());
}

{
  util::Nullable<std::string> nullable(0);
  TEST(nullable.isNull());
}

{
  std::string str = "aa";
  util::Nullable<std::string> nullable(&str);
  TEST(!nullable.isNull());

  TEST(nullable == "aa");
  TEST(!(nullable == "aaa"));
  TEST(!(nullable != "aa"));
  TEST(nullable == "aa");

  TEST(nullable.get(), ==, "aa");
  TEST(std::string(nullable), ==, "aa");
}

{
  int a = 23;
  util::Nullable<int> nullable(a);
  util::Nullable<int> nullable2(24);
  TEST(!nullable.isNull());

  TEST(nullable, ==, 23);
  TEST(nullable, >=, 23);
  TEST(nullable, <=, 23);
  TEST(nullable, <, 24);
  TEST(nullable, <, 24);
  TEST(!(nullable < 22));
  TEST(nullable, !=, nullable2);
  TEST(nullable, <, nullable2);
  TEST(nullable2, >, nullable);

  util::Nullable<int> nullable3(nullable);
  TEST(nullable == nullable3);

  nullable3 = nullable2;
  TEST(nullable2 == nullable3);
  TEST(nullable3 == 24);
  TEST(nullable2 == 24);
  TEST(nullable2 == nullable2.get());
  TEST(int(nullable2) == nullable2.get());
  TEST(!nullable3.isNull());
  TEST(!nullable2.isNull());

  util::Nullable<int> voidnull;
  TEST(voidnull.isNull());
}
}


  // inversion count
  std::vector<int> test = {2, 1};
  TEST(inversions(test), ==, 1);

  test = {};
  TEST(inversions(test), ==, 0);

  test = {2};
  TEST(inversions(test), ==, 0);

  test = {2, 1};
  TEST(inversions(test), ==, 1);

  test = {1, 2};
  TEST(inversions(test), ==, 0);

  test = {2, 1, 3};
  TEST(inversions(test), ==, 1);

  test = {2, 3, 1};
  TEST(inversions(test), ==, 2);

  test = {3, 2, 1};
  TEST(inversions(test), ==, 3);

  test = {1, 2, 3};
  TEST(inversions(test), ==, 0);

  test = {1, 3, 2, 6, 5, 4, 8, 7, 9};
  TEST(inversions(test), ==, 5);

  test = {1, 2, 3, 4, 5, 6};
  TEST(inversions(test), ==, 0);

  test = {9, 8, 7, 6, 5, 4, 3, 2, 1};
  TEST(inversions(test), ==, 8 + 7 + 6 + 5 + 4 + 3 + 2 + 1);

  // nice float formatting
	TEST(formatFloat(15.564, 3), ==, "15.564");
	TEST(formatFloat(15.564, 0), ==, "16");
	TEST(formatFloat(15.0000, 10), ==, "15");
	TEST(formatFloat(15.0100, 10), ==, "15.01");
	TEST(formatFloat(0.0000, 10), ==, "0");
	TEST(formatFloat(-1.0000, 10), ==, "-1");
	TEST(formatFloat(-15.000001, 10), ==, "-15.000001");

}

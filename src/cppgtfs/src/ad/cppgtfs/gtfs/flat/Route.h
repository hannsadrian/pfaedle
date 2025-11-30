// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_FLAT_ROUTE_H_
#define AD_CPPGTFS_GTFS_FLAT_ROUTE_H_

#include <stdint.h>

#include <algorithm>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace ad {
namespace cppgtfs {
namespace gtfs {
namespace flat {

struct RouteFlds {
  size_t routeIdFld;
  size_t routeLongNameFld;
  size_t routeShortNameFld;
  size_t routeTypeFld;
  size_t routeUrlFld;
  size_t routeDescFld;
  size_t agencyIdFld;
  size_t routeColorFld;
  size_t routeTextColorFld;
  size_t routeSortOrderFld;
  size_t continuousDropOffFld;
  size_t continuousPickupFld;
  std::vector<size_t> addHeaders;
};

struct Route {
  enum TYPE : uint16_t {
    // standard types
    TRAM = 0,
    SUBWAY = 1,
    RAIL = 2,
    BUS = 3,
    FERRY = 4,
    CABLE_CAR = 5,
    GONDOLA = 6,
    FUNICULAR = 7,
    TROLLEYBUS = 11,
    MONORAIL = 12,
    COACH = 200,
    AIR = 199,
    NONE = 99,

    EXT_100 = 100,
    EXT_101 = 101,
    EXT_102 = 102,
    EXT_103 = 103,
    EXT_104 = 104,
    EXT_105 = 105,
    EXT_106 = 106,
    EXT_107 = 107,
    EXT_108 = 108,
    EXT_109 = 109,
    EXT_110 = 110,
    EXT_111 = 111,
    EXT_112 = 112,
    EXT_113 = 113,
    EXT_114 = 114,
    EXT_115 = 115,
    EXT_117 = 117,
    EXT_300 = 300,
    EXT_400 = 400,
    EXT_403 = 403,
    EXT_404 = 404,
    EXT_1503 = 1503,
    EXT_1305 = 1305,
    EXT_1504 = 1504,
    EXT_201 = 201,
    EXT_202 = 202,
    EXT_203 = 203,
    EXT_204 = 204,
    EXT_205 = 205,
    EXT_206 = 206,
    EXT_207 = 207,
    EXT_208 = 208,
    EXT_209 = 209,
    EXT_700 = 700,
    EXT_701 = 701,
    EXT_702 = 702,
    EXT_703 = 703,
    EXT_704 = 704,
    EXT_705 = 705,
    EXT_706 = 706,
    EXT_707 = 707,
    EXT_708 = 708,
    EXT_709 = 709,
    EXT_710 = 710,
    EXT_711 = 711,
    EXT_712 = 712,
    EXT_713 = 713,
    EXT_714 = 714,
    EXT_715 = 715,
    EXT_716 = 716,
    EXT_717 = 717,
    EXT_1500 = 1500,
    EXT_1501 = 1501,
    EXT_1505 = 1505,
    EXT_1506 = 1506,
    EXT_1507 = 1507,
    EXT_401 = 401,
    EXT_402 = 402,
    EXT_500 = 500,
    EXT_600 = 600,
    EXT_900 = 900,
    EXT_901 = 901,
    EXT_902 = 902,
    EXT_903 = 903,
    EXT_904 = 904,
    EXT_905 = 905,
    EXT_906 = 906,
    EXT_1000 = 1000,
    EXT_1001 = 1001,
    EXT_1002 = 1002,
    EXT_1003 = 1003,
    EXT_1004 = 1004,
    EXT_1005 = 1005,
    EXT_1006 = 1006,
    EXT_1007 = 1007,
    EXT_1008 = 1008,
    EXT_1009 = 1009,
    EXT_1010 = 1010,
    EXT_1011 = 1011,
    EXT_1012 = 1012,
    EXT_1013 = 1013,
    EXT_1014 = 1014,
    EXT_1015 = 1015,
    EXT_1016 = 1016,
    EXT_1017 = 1017,
    EXT_1018 = 1018,
    EXT_1019 = 1019,
    EXT_1020 = 1020,
    EXT_1021 = 1021,
    EXT_1200 = 1200,
    EXT_1502 = 1502,
    EXT_1300 = 1300,
    EXT_1301 = 1301,
    EXT_1304 = 1304,
    EXT_1306 = 1306,
    EXT_1307 = 1307,
    EXT_1101 = 1101,
    EXT_1102 = 1102,
    EXT_1103 = 1103,
    EXT_1104 = 1104,
    EXT_1105 = 1105,
    EXT_1106 = 1106,
    EXT_1107 = 1107,
    EXT_1108 = 1108,
    EXT_1109 = 1109,
    EXT_1110 = 1110,
    EXT_1111 = 1111,
    EXT_1112 = 1112,
    EXT_1113 = 1113,
    EXT_1114 = 1114,
    EXT_116 = 116,
    EXT_1303 = 1303,
    EXT_1302 = 1302,
    EXT_1400 = 1400,
    EXT_800 = 800,
    EXT_405 = 405,
    EXT_1700 = 1700,
    EXT_1702 = 1702
  };

  std::string id;
  std::string agency;
  std::string short_name;
  std::string long_name;
  std::string desc;
  Route::TYPE type;
  std::string url;
  uint32_t color;
  uint32_t text_color;
  int64_t sort_order;
  uint8_t continuous_pickup;
  uint8_t continuous_drop_off;

  static std::string getHexColorString(uint32_t color) {
    if (color == std::numeric_limits<uint32_t>::max()) return "";
    // using stringstream here, because it doesnt add "0x" to the front
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(6) << color;
    return ss.str();
  }

  static std::string getTypeString(flat::Route::TYPE t) {
    if (t == flat::Route::COACH) return "coach";
    if (t == flat::Route::TROLLEYBUS) return "trolleybus";
    if (t == flat::Route::MONORAIL) return "monorail";

    if (t > 7) return std::to_string(t);

    std::string names[8] = {"tram",  "subway",   "rail",    "bus",
                            "ferry", "cablecar", "gondola", "funicular"};
    return names[t];
  }

  static flat::Route::TYPE getRouteType(int t) {
    return static_cast<flat::Route::TYPE>(t);
  }

  static flat::Route::TYPE getStandardRouteType(int t) {
    switch (t) {
      case 2:
      case 100:
      case 101:
      case 102:
      case 103:
      case 104:
      case 105:
      case 106:
      case 107:
      case 108:
      case 109:
      case 110:
      case 111:
      case 112:
      case 113:
      case 114:
      case 115:
      case 117:
      case 300:
      case 1503:
        return Route::TYPE::RAIL;
      case 3:
        return Route::TYPE::BUS;
      case 200:
      case 201:
      case 202:
      case 203:
      case 204:
      case 205:
      case 206:
      case 207:
      case 208:
      case 209:
        return Route::TYPE::COACH;
      case 700:
      case 701:
      case 702:
      case 703:
      case 704:
      case 705:
      case 706:
      case 707:
      case 708:
      case 709:
      case 710:
      case 711:
      case 712:
      case 713:
      case 714:
      case 715:
      case 716:
      case 717:
      case 1500:
      case 1501:
      case 1505:
      case 1506:
      case 1507:
        return Route::TYPE::BUS;
      case 1:
      case 400:
      case 401:
      case 402:
      case 403:
      case 404:
      case 500:
      case 600:
        return Route::TYPE::SUBWAY;
      case 0:
      case 900:
      case 901:
      case 902:
      case 903:
      case 904:
      case 905:
      case 906:
        return Route::TYPE::TRAM;
      case 4:
      case 1000:
      case 1001:
      case 1002:
      case 1003:
      case 1004:
      case 1005:
      case 1006:
      case 1007:
      case 1008:
      case 1009:
      case 1010:
      case 1011:
      case 1012:
      case 1013:
      case 1014:
      case 1015:
      case 1016:
      case 1017:
      case 1018:
      case 1019:
      case 1020:
      case 1021:
      case 1200:
      case 1502:
        return Route::TYPE::FERRY;
      // TODO(patrick): from here on not complete!
      case 6:
      case 1300:
      case 1301:
      case 1304:
      case 1305:
      case 1306:
      case 1307:
        return Route::TYPE::GONDOLA;
      case 1101:
      case 1102:
      case 1103:
      case 1104:
      case 1105:
      case 1106:
      case 1107:
      case 1108:
      case 1109:
      case 1110:
      case 1111:
      case 1112:
      case 1113:
      case 1114:
        return Route::TYPE::GONDOLA;
      case 7:
      case 116:
      case 1303:
      case 1302:
      case 1400:
        return Route::TYPE::FUNICULAR;
      case 5:
        return Route::TYPE::CABLE_CAR;
      case 11:
      case 800:
        return Route::TYPE::TROLLEYBUS;
      case 12:
      case 405:
        return Route::TYPE::MONORAIL;
      default:
        return Route::TYPE::NONE;
    }
  }

  static std::set<flat::Route::TYPE> getTypesFromString(std::string name) {
    std::set<flat::Route::TYPE> ret;

    if (name.empty()) return ret;

    char* rem;
    uint64_t num = strtol(name.c_str(), &rem, 10);
    if (!*rem) {
      auto i = getRouteType(num);
      if (i != Route::TYPE::NONE) ret.insert(i);
      return ret;
    }

    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    if (name == "all") {
      ret.insert(flat::Route::TYPE::RAIL);
      ret.insert(flat::Route::TYPE::EXT_100);
      ret.insert(flat::Route::TYPE::EXT_101);
      ret.insert(flat::Route::TYPE::EXT_102);
      ret.insert(flat::Route::TYPE::EXT_103);
      ret.insert(flat::Route::TYPE::EXT_104);
      ret.insert(flat::Route::TYPE::EXT_105);
      ret.insert(flat::Route::TYPE::EXT_106);
      ret.insert(flat::Route::TYPE::EXT_107);
      ret.insert(flat::Route::TYPE::EXT_108);
      ret.insert(flat::Route::TYPE::EXT_109);
      ret.insert(flat::Route::TYPE::EXT_110);
      ret.insert(flat::Route::TYPE::EXT_111);
      ret.insert(flat::Route::TYPE::EXT_112);
      ret.insert(flat::Route::TYPE::EXT_113);
      ret.insert(flat::Route::TYPE::EXT_114);
      ret.insert(flat::Route::TYPE::EXT_115);
      ret.insert(flat::Route::TYPE::EXT_117);
      ret.insert(flat::Route::TYPE::EXT_300);
      ret.insert(flat::Route::TYPE::EXT_400);
      ret.insert(flat::Route::TYPE::EXT_403);
      ret.insert(flat::Route::TYPE::EXT_404);
      ret.insert(flat::Route::TYPE::EXT_1503);
      ret.insert(flat::Route::TYPE::EXT_1504);
      ret.insert(flat::Route::TYPE::BUS);
      ret.insert(flat::Route::TYPE::COACH);
      ret.insert(flat::Route::TYPE::EXT_201);
      ret.insert(flat::Route::TYPE::EXT_202);
      ret.insert(flat::Route::TYPE::EXT_203);
      ret.insert(flat::Route::TYPE::EXT_204);
      ret.insert(flat::Route::TYPE::EXT_205);
      ret.insert(flat::Route::TYPE::EXT_206);
      ret.insert(flat::Route::TYPE::EXT_207);
      ret.insert(flat::Route::TYPE::EXT_208);
      ret.insert(flat::Route::TYPE::EXT_209);
      ret.insert(flat::Route::TYPE::EXT_700);
      ret.insert(flat::Route::TYPE::EXT_701);
      ret.insert(flat::Route::TYPE::EXT_702);
      ret.insert(flat::Route::TYPE::EXT_703);
      ret.insert(flat::Route::TYPE::EXT_704);
      ret.insert(flat::Route::TYPE::EXT_705);
      ret.insert(flat::Route::TYPE::EXT_706);
      ret.insert(flat::Route::TYPE::EXT_707);
      ret.insert(flat::Route::TYPE::EXT_708);
      ret.insert(flat::Route::TYPE::EXT_709);
      ret.insert(flat::Route::TYPE::EXT_710);
      ret.insert(flat::Route::TYPE::EXT_711);
      ret.insert(flat::Route::TYPE::EXT_712);
      ret.insert(flat::Route::TYPE::EXT_713);
      ret.insert(flat::Route::TYPE::EXT_714);
      ret.insert(flat::Route::TYPE::EXT_715);
      ret.insert(flat::Route::TYPE::EXT_716);
      ret.insert(flat::Route::TYPE::EXT_717);
      ret.insert(flat::Route::TYPE::EXT_1500);
      ret.insert(flat::Route::TYPE::EXT_1501);
      ret.insert(flat::Route::TYPE::EXT_1505);
      ret.insert(flat::Route::TYPE::EXT_1506);
      ret.insert(flat::Route::TYPE::EXT_1507);
      ret.insert(flat::Route::TYPE::SUBWAY);
      ret.insert(flat::Route::TYPE::EXT_401);
      ret.insert(flat::Route::TYPE::EXT_402);
      ret.insert(flat::Route::TYPE::EXT_500);
      ret.insert(flat::Route::TYPE::EXT_600);
      ret.insert(flat::Route::TYPE::TRAM);
      ret.insert(flat::Route::TYPE::EXT_900);
      ret.insert(flat::Route::TYPE::EXT_901);
      ret.insert(flat::Route::TYPE::EXT_902);
      ret.insert(flat::Route::TYPE::EXT_903);
      ret.insert(flat::Route::TYPE::EXT_904);
      ret.insert(flat::Route::TYPE::EXT_905);
      ret.insert(flat::Route::TYPE::EXT_906);
      ret.insert(flat::Route::TYPE::FERRY);
      ret.insert(flat::Route::TYPE::EXT_1000);
      ret.insert(flat::Route::TYPE::EXT_1001);
      ret.insert(flat::Route::TYPE::EXT_1002);
      ret.insert(flat::Route::TYPE::EXT_1003);
      ret.insert(flat::Route::TYPE::EXT_1004);
      ret.insert(flat::Route::TYPE::EXT_1005);
      ret.insert(flat::Route::TYPE::EXT_1006);
      ret.insert(flat::Route::TYPE::EXT_1007);
      ret.insert(flat::Route::TYPE::EXT_1008);
      ret.insert(flat::Route::TYPE::EXT_1009);
      ret.insert(flat::Route::TYPE::EXT_1010);
      ret.insert(flat::Route::TYPE::EXT_1011);
      ret.insert(flat::Route::TYPE::EXT_1012);
      ret.insert(flat::Route::TYPE::EXT_1013);
      ret.insert(flat::Route::TYPE::EXT_1014);
      ret.insert(flat::Route::TYPE::EXT_1015);
      ret.insert(flat::Route::TYPE::EXT_1016);
      ret.insert(flat::Route::TYPE::EXT_1017);
      ret.insert(flat::Route::TYPE::EXT_1018);
      ret.insert(flat::Route::TYPE::EXT_1019);
      ret.insert(flat::Route::TYPE::EXT_1020);
      ret.insert(flat::Route::TYPE::EXT_1021);
      ret.insert(flat::Route::TYPE::EXT_1200);
      ret.insert(flat::Route::TYPE::EXT_1502);
      ret.insert(flat::Route::TYPE::GONDOLA);
      ret.insert(flat::Route::TYPE::EXT_1300);
      ret.insert(flat::Route::TYPE::EXT_1301);
      ret.insert(flat::Route::TYPE::EXT_1304);
      ret.insert(flat::Route::TYPE::EXT_1305);
      ret.insert(flat::Route::TYPE::EXT_1306);
      ret.insert(flat::Route::TYPE::EXT_1307);
      ret.insert(flat::Route::TYPE::EXT_1101);
      ret.insert(flat::Route::TYPE::EXT_1102);
      ret.insert(flat::Route::TYPE::EXT_1103);
      ret.insert(flat::Route::TYPE::EXT_1104);
      ret.insert(flat::Route::TYPE::EXT_1105);
      ret.insert(flat::Route::TYPE::EXT_1106);
      ret.insert(flat::Route::TYPE::EXT_1107);
      ret.insert(flat::Route::TYPE::EXT_1108);
      ret.insert(flat::Route::TYPE::EXT_1109);
      ret.insert(flat::Route::TYPE::EXT_1110);
      ret.insert(flat::Route::TYPE::EXT_1111);
      ret.insert(flat::Route::TYPE::EXT_1112);
      ret.insert(flat::Route::TYPE::EXT_1113);
      ret.insert(flat::Route::TYPE::EXT_1114);
      ret.insert(flat::Route::TYPE::FUNICULAR);
      ret.insert(flat::Route::TYPE::EXT_116);
      ret.insert(flat::Route::TYPE::EXT_1303);
      ret.insert(flat::Route::TYPE::EXT_1302);
      ret.insert(flat::Route::TYPE::EXT_1400);
      ret.insert(flat::Route::TYPE::CABLE_CAR);
      ret.insert(flat::Route::TYPE::TROLLEYBUS);
      ret.insert(flat::Route::TYPE::EXT_800);
      ret.insert(flat::Route::TYPE::MONORAIL);
      ret.insert(flat::Route::TYPE::EXT_405);
      ret.insert(flat::Route::TYPE::EXT_1700);
      ret.insert(flat::Route::TYPE::EXT_1702);
      return ret;
    }

    if (name == "bus") {
      ret.insert(flat::Route::TYPE::BUS);
      ret.insert(flat::Route::TYPE::EXT_700);
      ret.insert(flat::Route::TYPE::EXT_701);
      ret.insert(flat::Route::TYPE::EXT_702);
      ret.insert(flat::Route::TYPE::EXT_703);
      ret.insert(flat::Route::TYPE::EXT_704);
      ret.insert(flat::Route::TYPE::EXT_705);
      ret.insert(flat::Route::TYPE::EXT_706);
      ret.insert(flat::Route::TYPE::EXT_707);
      ret.insert(flat::Route::TYPE::EXT_708);
      ret.insert(flat::Route::TYPE::EXT_709);
      ret.insert(flat::Route::TYPE::EXT_710);
      ret.insert(flat::Route::TYPE::EXT_711);
      ret.insert(flat::Route::TYPE::EXT_712);
      ret.insert(flat::Route::TYPE::EXT_713);
      ret.insert(flat::Route::TYPE::EXT_714);
      ret.insert(flat::Route::TYPE::EXT_715);
      ret.insert(flat::Route::TYPE::EXT_716);
      ret.insert(flat::Route::TYPE::EXT_717);
      ret.insert(flat::Route::TYPE::EXT_1500);
      ret.insert(flat::Route::TYPE::EXT_1501);
      ret.insert(flat::Route::TYPE::EXT_1505);
      ret.insert(flat::Route::TYPE::EXT_1506);
      ret.insert(flat::Route::TYPE::EXT_1507);
      return ret;
    }

    if (name == "tram" || name == "streetcar" || name == "light_rail" ||
        name == "lightrail" || name == "light-rail") {
      ret.insert(flat::Route::TYPE::TRAM);
      ret.insert(flat::Route::TYPE::EXT_900);
      ret.insert(flat::Route::TYPE::EXT_901);
      ret.insert(flat::Route::TYPE::EXT_902);
      ret.insert(flat::Route::TYPE::EXT_903);
      ret.insert(flat::Route::TYPE::EXT_904);
      ret.insert(flat::Route::TYPE::EXT_905);
      ret.insert(flat::Route::TYPE::EXT_906);
      return ret;
    }

    if (name == "train" || name == "rail") {
      ret.insert(flat::Route::TYPE::RAIL);
      ret.insert(flat::Route::TYPE::EXT_100);
      ret.insert(flat::Route::TYPE::EXT_101);
      ret.insert(flat::Route::TYPE::EXT_102);
      ret.insert(flat::Route::TYPE::EXT_103);
      ret.insert(flat::Route::TYPE::EXT_104);
      ret.insert(flat::Route::TYPE::EXT_105);
      ret.insert(flat::Route::TYPE::EXT_106);
      ret.insert(flat::Route::TYPE::EXT_107);
      ret.insert(flat::Route::TYPE::EXT_108);
      ret.insert(flat::Route::TYPE::EXT_109);
      ret.insert(flat::Route::TYPE::EXT_110);
      ret.insert(flat::Route::TYPE::EXT_111);
      ret.insert(flat::Route::TYPE::EXT_112);
      ret.insert(flat::Route::TYPE::EXT_113);
      ret.insert(flat::Route::TYPE::EXT_114);
      ret.insert(flat::Route::TYPE::EXT_115);
      ret.insert(flat::Route::TYPE::EXT_117);
      ret.insert(flat::Route::TYPE::EXT_300);
      ret.insert(flat::Route::TYPE::EXT_1503);
      return ret;
    }

    if (name == "ferry" || name == "boat" || name == "ship") {
      ret.insert(flat::Route::TYPE::FERRY);
      ret.insert(flat::Route::TYPE::EXT_1000);
      ret.insert(flat::Route::TYPE::EXT_1001);
      ret.insert(flat::Route::TYPE::EXT_1002);
      ret.insert(flat::Route::TYPE::EXT_1003);
      ret.insert(flat::Route::TYPE::EXT_1004);
      ret.insert(flat::Route::TYPE::EXT_1005);
      ret.insert(flat::Route::TYPE::EXT_1006);
      ret.insert(flat::Route::TYPE::EXT_1007);
      ret.insert(flat::Route::TYPE::EXT_1008);
      ret.insert(flat::Route::TYPE::EXT_1009);
      ret.insert(flat::Route::TYPE::EXT_1010);
      ret.insert(flat::Route::TYPE::EXT_1011);
      ret.insert(flat::Route::TYPE::EXT_1012);
      ret.insert(flat::Route::TYPE::EXT_1013);
      ret.insert(flat::Route::TYPE::EXT_1014);
      ret.insert(flat::Route::TYPE::EXT_1015);
      ret.insert(flat::Route::TYPE::EXT_1016);
      ret.insert(flat::Route::TYPE::EXT_1017);
      ret.insert(flat::Route::TYPE::EXT_1018);
      ret.insert(flat::Route::TYPE::EXT_1019);
      ret.insert(flat::Route::TYPE::EXT_1020);
      ret.insert(flat::Route::TYPE::EXT_1021);
      ret.insert(flat::Route::TYPE::EXT_1200);
      ret.insert(flat::Route::TYPE::EXT_1502);
      return ret;
    }

    if (name == "subway" || name == "metro") {
      ret.insert(flat::Route::TYPE::SUBWAY);
      ret.insert(flat::Route::TYPE::EXT_400);
      ret.insert(flat::Route::TYPE::EXT_401);
      ret.insert(flat::Route::TYPE::EXT_402);
      ret.insert(flat::Route::TYPE::EXT_403);
      ret.insert(flat::Route::TYPE::EXT_404);
      ret.insert(flat::Route::TYPE::EXT_500);
      ret.insert(flat::Route::TYPE::EXT_600);
      return ret;
    }

    if (name == "cablecar" || name == "cable_car" || name == "cable-car") {
      ret.insert(flat::Route::TYPE::CABLE_CAR);
      ret.insert(flat::Route::TYPE::EXT_1302);
      return ret;
    }

    if (name == "gondola") {
      ret.insert(flat::Route::TYPE::GONDOLA);
      ret.insert(flat::Route::TYPE::EXT_1300);
      ret.insert(flat::Route::TYPE::EXT_1301);
      ret.insert(flat::Route::TYPE::EXT_1304);
      ret.insert(flat::Route::TYPE::EXT_1306);
      ret.insert(flat::Route::TYPE::EXT_1307);
      ret.insert(flat::Route::TYPE::EXT_1101);
      ret.insert(flat::Route::TYPE::EXT_1102);
      ret.insert(flat::Route::TYPE::EXT_1103);
      ret.insert(flat::Route::TYPE::EXT_1104);
      ret.insert(flat::Route::TYPE::EXT_1105);
      ret.insert(flat::Route::TYPE::EXT_1106);
      ret.insert(flat::Route::TYPE::EXT_1107);
      ret.insert(flat::Route::TYPE::EXT_1108);
      ret.insert(flat::Route::TYPE::EXT_1109);
      ret.insert(flat::Route::TYPE::EXT_1110);
      ret.insert(flat::Route::TYPE::EXT_1111);
      ret.insert(flat::Route::TYPE::EXT_1112);
      ret.insert(flat::Route::TYPE::EXT_1113);
      ret.insert(flat::Route::TYPE::EXT_1114);
      return ret;
    }

    if (name == "air") {
      ret.insert(flat::Route::TYPE::AIR);
      return ret;
    }

    if (name == "funicular") {
      ret.insert(flat::Route::TYPE::FUNICULAR);
      ret.insert(flat::Route::TYPE::EXT_116);
      ret.insert(flat::Route::TYPE::EXT_1303);
      ret.insert(flat::Route::TYPE::EXT_1302);
      ret.insert(flat::Route::TYPE::EXT_1400);
      return ret;
    }

    if (name == "coach") {
      ret.insert(flat::Route::TYPE::COACH);
      ret.insert(flat::Route::TYPE::EXT_201);
      ret.insert(flat::Route::TYPE::EXT_202);
      ret.insert(flat::Route::TYPE::EXT_203);
      ret.insert(flat::Route::TYPE::EXT_204);
      ret.insert(flat::Route::TYPE::EXT_205);
      ret.insert(flat::Route::TYPE::EXT_206);
      ret.insert(flat::Route::TYPE::EXT_207);
      ret.insert(flat::Route::TYPE::EXT_208);
      ret.insert(flat::Route::TYPE::EXT_209);
      return ret;
    }

    if (name == "mono-rail" || name == "monorail") {
      ret.insert(flat::Route::TYPE::MONORAIL);
      ret.insert(flat::Route::TYPE::EXT_405);
      return ret;
    }

    if (name == "trolley" || name == "trolleybus" || name == "trolley-bus") {
      ret.insert(flat::Route::TYPE::TROLLEYBUS);
      ret.insert(flat::Route::TYPE::EXT_800);
      return ret;
    }

    return ret;
  }
};

}  // namespace flat
}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_ROUTE_H_

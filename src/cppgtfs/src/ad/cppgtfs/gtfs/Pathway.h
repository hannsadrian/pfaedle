// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef AD_CPPGTFS_GTFS_PATHWAY_H_
#define AD_CPPGTFS_GTFS_PATHWAY_H_

#include <stdint.h>

#include <string>

#include "Stop.h"
#include "flat/Pathway.h"

using std::exception;
using std::string;

namespace ad {
namespace cppgtfs {
namespace gtfs {

class Pathway {
 public:
  typedef Pathway* Ref;
  static std::string getId(Ref r) { return r->getId(); }

  Pathway() {}

  Pathway(const std::string& id, Stop* from_stop_id, Stop* to_stop_id,
          uint8_t pathway_mode, bool is_bidirectional, double length,
          int64_t traversal_time, int64_t stair_count, double max_slope,
          double min_width, const std::string& signposted_as,
          const std::string& reversed_signposted_as)
      : _id(id),
        _from_stop_id(from_stop_id),
        _to_stop_id(to_stop_id),
        _pathway_mode(pathway_mode),
        _is_bidirectional(is_bidirectional),
        _length(length),
        _traversal_time(traversal_time),
        _stair_count(stair_count),
        _max_slope(max_slope),
        _min_width(min_width),
        _signposted_as(signposted_as),
        _reversed_signposted_as(reversed_signposted_as) {}

  const std::string& getId() const { return _id; }

  flat::Pathway getFlat() const {
    flat::Pathway r;
    r.id = _id;
    r.from_stop_id = _from_stop_id->getId();
    r.to_stop_id = _to_stop_id->getId();
    r.pathway_mode = _pathway_mode;
    r.is_bidirectional = _is_bidirectional;
    r.length = _length;
    r.traversal_time = _traversal_time;
    r.stair_count = _stair_count;
    r.max_slope = _max_slope;
    r.min_width = _min_width;
    r.signposted_as = _signposted_as;
    r.reversed_signposted_as = _reversed_signposted_as;
    return r;
  }

  // TODO(patrick): implement setters

 private:
  std::string _id;
  Stop* _from_stop_id;
  Stop* _to_stop_id;
  uint8_t _pathway_mode;
  bool _is_bidirectional;
  double _length;
  int64_t _traversal_time;
  int64_t _stair_count;
  double _max_slope;
  double _min_width;
  std::string _signposted_as;
  std::string _reversed_signposted_as;
};

}  // namespace gtfs
}  // namespace cppgtfs
}  // namespace ad

#endif  // AD_CPPGTFS_GTFS_PATHWAY_H_

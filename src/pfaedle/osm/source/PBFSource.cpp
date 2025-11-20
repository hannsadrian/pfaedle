// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include <cassert>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <thread>

#include <osmium/handler.hpp>
#include <osmium/thread/pool.hpp>
#include <osmium/visitor.hpp>

#include "pfaedle/osm/source/PBFSource.h"
#include "util/log/Log.h"

using pfaedle::osm::source::OsmSourceAttr;
using pfaedle::osm::source::OsmSourceNode;
using pfaedle::osm::source::OsmSourceRelation;
using pfaedle::osm::source::OsmSourceRelationMember;
using pfaedle::osm::source::OsmSourceWay;
using pfaedle::osm::source::PBFSource;

// _____________________________________________________________________________
PBFSource::PBFSource(const std::string &path)
    : _path(path), _buffer(), _curNode(nullptr), _curWay(nullptr),
      _curRel(nullptr) {

  initReader();

  auto header = _reader->header();
  if (header.has_multiple_object_versions()) {
    throw std::runtime_error("PBF file contains history data, not supported");
  }

  auto boxes = header.boxes();
  if (!boxes.empty()) {
    const auto &box = boxes.front();
    _bbox = util::geo::Box<double>(
        util::geo::Point<double>(box.bottom_left().lon(),
                                 box.bottom_left().lat()),
        util::geo::Point<double>(box.top_right().lon(), box.top_right().lat()));
  } else {
    _bbox = util::geo::Box<double>(util::geo::Point<double>(-180.0, -90.0),
                                   util::geo::Point<double>(180.0, 90.0));
  }

  readNextBuffer();
}

// _____________________________________________________________________________
PBFSource::~PBFSource() {
  if (_reader) {
    _reader->close();
  }
}

// _____________________________________________________________________________
// _____________________________________________________________________________
void PBFSource::initReader(osmium::osm_entity_bits::type entities) {
  osmium::io::File input_file{_path};

  // Create reader with multi-threaded decompression enabled
  // Osmium automatically uses multiple threads for PBF decompression
  _reader = std::make_unique<osmium::io::Reader>(
      input_file, entities,
      osmium::io::read_meta::no // Don't read metadata (version, timestamp,
                                // etc.) for speed
  );

  const int num_threads = std::thread::hardware_concurrency();
  LOG(util::DEBUG) << "Initialized PBF reader (osmium uses up to "
                   << num_threads << " threads for decompression)";
}

// _____________________________________________________________________________
void PBFSource::readNextBuffer() {
  if (_reader) {
    _buffer = _reader->read();
    if (_buffer) {
      _bufferIt = _buffer.begin();
    }
  }
}

// _____________________________________________________________________________
void PBFSource::advanceToNextEntity(osmium::item_type type) {
  // Check if we need a new buffer
  while (!_buffer || _bufferIt == _buffer.end()) {
    readNextBuffer();
    if (!_buffer || _buffer.committed() == 0) {
      return;
    }
  }

  // Find the next entity of the requested type
  while (_bufferIt != _buffer.end() && _bufferIt->type() != type) {
    ++_bufferIt;
    while (!_buffer || _bufferIt == _buffer.end()) {
      readNextBuffer();
      if (!_buffer || _buffer.committed() == 0) {
        return;
      }
    }
  }
}

// _____________________________________________________________________________
const OsmSourceNode *PBFSource::nextNode() {
  advanceToNextEntity(osmium::item_type::node);

  if (!_buffer || _bufferIt == _buffer.end()) {
    return nullptr;
  }

  _curNode = &static_cast<const osmium::Node &>(*_bufferIt);
  ++_bufferIt;

  _retNode.id = _curNode->id();
  _retNode.lat = _curNode->location().lat();
  _retNode.lon = _curNode->location().lon();

  _curTagIt = _curNode->tags().begin();
  _curTagEnd = _curNode->tags().end();

  return &_retNode;
}

// _____________________________________________________________________________
const OsmSourceAttr PBFSource::nextAttr() {
  if (_curNode) {
    if (_curTagIt != _curTagEnd) {
      _retAttr.key = _curTagIt->key();
      _retAttr.value = _curTagIt->value();
      ++_curTagIt;
      return _retAttr;
    }
  } else if (_curWay) {
    if (_curTagIt != _curTagEnd) {
      _retAttr.key = _curTagIt->key();
      _retAttr.value = _curTagIt->value();
      ++_curTagIt;
      return _retAttr;
    }
  } else if (_curRel) {
    if (_curTagIt != _curTagEnd) {
      _retAttr.key = _curTagIt->key();
      _retAttr.value = _curTagIt->value();
      ++_curTagIt;
      return _retAttr;
    }
  }

  _retAttr.key = nullptr;
  _retAttr.value = nullptr;
  return _retAttr;
}

// _____________________________________________________________________________
const OsmSourceWay *PBFSource::nextWay() {
  advanceToNextEntity(osmium::item_type::way);

  if (!_buffer || _bufferIt == _buffer.end()) {
    return nullptr;
  }

  _curWay = &static_cast<const osmium::Way &>(*_bufferIt);
  ++_bufferIt;

  _retWay.id = _curWay->id();

  _curTagIt = _curWay->tags().begin();
  _curTagEnd = _curWay->tags().end();
  _curWayNodeIt = _curWay->nodes().begin();
  _curWayNodeEnd = _curWay->nodes().end();

  return &_retWay;
}

// _____________________________________________________________________________
uint64_t PBFSource::nextMemberNode() {
  if (_curWay && _curWayNodeIt != _curWayNodeEnd) {
    uint64_t id = _curWayNodeIt->ref();
    ++_curWayNodeIt;
    return id;
  }
  return 0;
}

// _____________________________________________________________________________
const OsmSourceRelationMember *PBFSource::nextMember() {
  if (_curRel && _curRelMemberIt != _curRelMemberEnd) {
    _retMember.id = _curRelMemberIt->ref();

    switch (_curRelMemberIt->type()) {
    case osmium::item_type::node:
      _retMember.type = 0;
      break;
    case osmium::item_type::way:
      _retMember.type = 1;
      break;
    case osmium::item_type::relation:
      _retMember.type = 2;
      break;
    default:
      _retMember.type = 0;
    }

    _retMember.role = _curRelMemberIt->role();
    ++_curRelMemberIt;
    return &_retMember;
  }
  return nullptr;
}

// _____________________________________________________________________________
const OsmSourceRelation *PBFSource::nextRel() {
  advanceToNextEntity(osmium::item_type::relation);

  if (!_buffer || _bufferIt == _buffer.end()) {
    return nullptr;
  }

  _curRel = &static_cast<const osmium::Relation &>(*_bufferIt);
  ++_bufferIt;

  _retRel.id = _curRel->id();

  _curTagIt = _curRel->tags().begin();
  _curTagEnd = _curRel->tags().end();
  _curRelMemberIt = _curRel->members().begin();
  _curRelMemberEnd = _curRel->members().end();

  return &_retRel;
}

// _____________________________________________________________________________
// _____________________________________________________________________________
void PBFSource::seekNodes() {
  _reader->close();
  initReader(osmium::osm_entity_bits::node);
  readNextBuffer();

  _curNode = nullptr;
  _curWay = nullptr;
  _curRel = nullptr;
}

// _____________________________________________________________________________
// _____________________________________________________________________________
void PBFSource::seekWays() {
  _reader->close();
  initReader(osmium::osm_entity_bits::way);
  readNextBuffer();

  _curNode = nullptr;
  _curWay = nullptr;
  _curRel = nullptr;
}

// _____________________________________________________________________________
// _____________________________________________________________________________
void PBFSource::resetWayIterators() {
  if (_curWay) {
    _curTagIt = _curWay->tags().begin();
    _curWayNodeIt = _curWay->nodes().begin();
  }
}

// _____________________________________________________________________________
void PBFSource::readWaysParallel(
    std::function<void(const osmium::Way &, int)> callback) {
  _reader->close();
  initReader(osmium::osm_entity_bits::way);

  const int num_threads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;

  // Thread-safe queue for buffers
  std::queue<osmium::memory::Buffer> buffer_queue;
  std::mutex queue_mutex;
  std::condition_variable queue_cv;
  std::condition_variable producer_cv; // For backpressure
  const size_t MAX_QUEUE_SIZE = 10;    // Limit memory usage
  bool done = false;

  // Worker function
  auto worker = [&](int thread_id) {
    while (true) {
      osmium::memory::Buffer buffer;
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        queue_cv.wait(lock, [&] { return !buffer_queue.empty() || done; });

        if (buffer_queue.empty() && done) {
          return;
        }

        buffer = std::move(buffer_queue.front());
        buffer_queue.pop();
      }
      producer_cv.notify_one(); // Notify producer that space is available

      // Process buffer
      for (const auto &entity : buffer) {
        if (entity.type() == osmium::item_type::way) {
          callback(static_cast<const osmium::Way &>(entity), thread_id);
        }
      }
    }
  };

  // Start workers
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker, i);
  }

  // Producer (main thread)
  while (osmium::memory::Buffer buffer = _reader->read()) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      // Wait if queue is full
      producer_cv.wait(lock,
                       [&] { return buffer_queue.size() < MAX_QUEUE_SIZE; });
      buffer_queue.push(std::move(buffer));
    }
    queue_cv.notify_one();
  }

  // Signal completion
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    done = true;
  }
  queue_cv.notify_all();

  // Join threads
  for (auto &t : threads) {
    t.join();
  }

  _curNode = nullptr;
  _curWay = nullptr;
  _curRel = nullptr;
}

// _____________________________________________________________________________
void PBFSource::seekRels() {
  _reader->close();
  initReader(osmium::osm_entity_bits::relation);
  readNextBuffer();

  _curNode = nullptr;
  _curWay = nullptr;
  _curRel = nullptr;
}

// _____________________________________________________________________________
util::geo::Box<double> PBFSource::getBounds() { return _bbox; }

// _____________________________________________________________________________
std::string PBFSource::decode(const char *str) const {
  return std::string(str);
}

// _____________________________________________________________________________
std::string PBFSource::decode(const std::string &str) const { return str; }

// _____________________________________________________________________________
// Handler for building the location index
class LocationIndexHandler : public osmium::handler::Handler {
public:
  LocationIndexHandler(PBFSource::LocationIndex &index,
                       const util::geo::Box<double> &bbox)
      : _index(index), _bbox(bbox), _nodeCount(0) {}

  void node(const osmium::Node &node) {
    // Only index nodes in bbox (with small padding for ways that cross
    // boundary)
    util::geo::Point<double> pt(node.location().lon(), node.location().lat());
    if (util::geo::contains(pt, _bbox)) {
      _index.set(node.positive_id(), node.location());
      _nodeCount++;
    }
  }

  size_t nodeCount() const { return _nodeCount; }

private:
  PBFSource::LocationIndex &_index;
  util::geo::Box<double> _bbox;
  size_t _nodeCount;
};

// _____________________________________________________________________________
void PBFSource::buildLocationIndex(const util::geo::Box<double> &bbox) {
  LOG(util::INFO) << "Building location index for bounding box "
                  << bbox.getLowerLeft().getY() << ", "
                  << bbox.getLowerLeft().getX() << " / "
                  << bbox.getUpperRight().getY() << ", "
                  << bbox.getUpperRight().getX() << "...";

  // Create the location index
  _locationIndex = std::make_unique<LocationIndex>();

  // Create a new reader for building the index
  osmium::io::File input_file{_path};
  osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node};

  // Build the index
  LocationIndexHandler handler(*_locationIndex, bbox);
  osmium::apply(reader, handler);
  reader.close();

  LOG(util::INFO) << "Location index built with " << handler.nodeCount()
                  << " nodes";
}

// _____________________________________________________________________________
bool PBFSource::hasNodeLocation(osmid id) const {
  if (!_locationIndex)
    return false;

  try {
    osmium::Location loc = _locationIndex->get_noexcept(id);
    return loc.valid();
  } catch (...) {
    return false;
  }
}

// _____________________________________________________________________________
bool PBFSource::getNodeLocation(osmid id, double *lat, double *lon) const {
  if (!_locationIndex)
    return false;

  try {
    // Use get_noexcept if available, or catch the exception
    // Note: SparseMemArray throws not_found if element is missing.
    // Ideally we would use a non-throwing method if available to avoid
    // overhead. Checking documentation: SparseMemArray::get throws. However, we
    // can check if it exists first? No, that's not exposed efficiently. Wait,
    // osmium::index::map::SparseMemArray DOES throw. But checking the header,
    // maybe there is a way? Actually, for SparseMemArray, get() throws. But we
    // can catch it. The issue is the overhead. Let's try to use get_noexcept if
    // it exists in the version we have. If not, we might need to use a
    // different index type or accept the overhead? But wait, the user said
    // "osmium extract" is fast. It uses FlexMem or similar. Let's assume
    // get_noexcept is available as per plan.

    osmium::Location loc = _locationIndex->get_noexcept(id);
    if (loc.valid()) {
      *lat = loc.lat();
      *lon = loc.lon();
      return true;
    }
  } catch (...) {
    // Fallback if get_noexcept is not available or throws (should not happen)
    return false;
  }

  return false;
}

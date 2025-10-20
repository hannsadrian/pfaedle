// Trip-level caching for shape computation results.

#ifndef PFAEDLE_ROUTER_TRIPCACHE_H_
#define PFAEDLE_ROUTER_TRIPCACHE_H_

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "pfaedle/Def.h"
#include "pfaedle/config/MotConfig.h"
#include "pfaedle/config/PfaedleConfig.h"
#include "pfaedle/gtfs/Feed.h"
#include "pfaedle/router/Misc.h"
#include "pfaedle/trgraph/Graph.h"

namespace pfaedle {
namespace router {

struct TripCacheOptions {
  bool enabled = false;
  std::string baseDir;
  uint64_t maxBytes = 0;
  std::string graphHash;
  std::string paramsHash;
};

struct TripCacheStats {
  size_t hits = 0;
  size_t misses = 0;
  size_t stores = 0;
  size_t storeSkipped = 0;
  size_t errors = 0;
  size_t evictions = 0;
  uint64_t bytesRead = 0;
  uint64_t bytesWritten = 0;
};

class TripCache {
 public:
  TripCache(const TripCacheOptions& options, const trgraph::Graph* graph);

  bool enabled() const { return _enabled; }

  bool lookup(const std::string& key, EdgeListHops* hops);
  void store(const std::string& key, const EdgeListHops& hops);
  void remove(const std::string& key);

  TripCacheStats stats() const;

  static std::string computeGraphFingerprint(const trgraph::Graph* graph);
  static std::string computeGraphFingerprint(
    const config::Config& cfg, const config::MotConfig& motCfg,
    const std::vector<std::string>& cfgPaths,
    const std::string& paramsHash);
  static std::string computeParamsFingerprint(
      const config::Config& cfg, const config::MotConfig& motCfg,
      const std::vector<std::string>& cfgPaths);
  static std::string computeTripKey(const gtfs::Trip* trip);
  static unsigned deriveDeterministicSeed(const std::string& graphHash,
                                          const std::string& paramsHash);

 private:
  struct EdgeHash {
    uint64_t lo = 0;
    uint64_t hi = 0;
    bool operator==(const EdgeHash& other) const {
      return lo == other.lo && hi == other.hi;
    }
  };

  struct EdgeHashHasher {
    size_t operator()(const EdgeHash& h) const {
      return std::hash<uint64_t>{}(h.lo) ^ (std::hash<uint64_t>{}(h.hi) << 1);
    }
  };

  static EdgeHash computeEdgeHash(const trgraph::Edge* edge);
  static EdgeHash computeEdgeHash(const trgraph::Edge* edge, bool reversed);
  const trgraph::Edge* resolveEdge(const EdgeHash& hash) const;

  std::string entryPath(const std::string& key) const;
  bool readEntry(const std::string& path, EdgeListHops* hops,
                 std::string* errorReason = nullptr);
  bool writeEntry(const std::string& path, const EdgeListHops& hops);
  void pruneIfNeeded();
  void rebuildEdgeIndex();

  static uint64_t directorySize(const std::string& root);
  static std::string hashString(const std::string& value);

  bool _enabled;
  std::string _root;
  uint64_t _maxBytes;
  const trgraph::Graph* _graph;

  mutable std::mutex _mutex;
  std::unordered_map<EdgeHash, std::vector<const trgraph::Edge*>,
                     EdgeHashHasher>
      _edgeIndex;

  std::atomic<size_t> _hits{0};
  std::atomic<size_t> _misses{0};
  std::atomic<size_t> _stores{0};
  std::atomic<size_t> _storeSkipped{0};
  std::atomic<size_t> _errors{0};
  std::atomic<size_t> _evictions{0};
  std::atomic<uint64_t> _bytesRead{0};
  std::atomic<uint64_t> _bytesWritten{0};

  uint64_t _currentBytes = 0;
};

}  // namespace router
}  // namespace pfaedle

#endif  // PFAEDLE_ROUTER_TRIPCACHE_H_

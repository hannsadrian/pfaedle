// Minimal PBF ingestion entry point. For now, it constructs the same graph
// interface as OsmBuilder::read, but uses libosmium streaming when available.
#ifndef PFAEDLE_OSM_OSMPBFREADER_H_
#define PFAEDLE_OSM_OSMPBFREADER_H_

#include <string>

#include "pfaedle/osm/BBoxIdx.h"
#include "pfaedle/osm/OsmReadOpts.h"
#include "pfaedle/osm/Restrictor.h"
#include "pfaedle/trgraph/Graph.h"

namespace pfaedle {
namespace osm {

class OsmPbfReader {
 public:
  OsmPbfReader() = default;

  // Read OSM PBF at path and build graph g respecting bbox and options.
  // Throws std::runtime_error on failure or when PBF support is disabled.
  void read(const std::string& path, const OsmReadOpts& opts, trgraph::Graph* g,
            const BBoxIdx& bbox, double gridSize, Restrictor* res);

    // Utility: Convert any supported OSM input (XML/PBF) to PBF output.
    // Throws when OSMIUM_ENABLED is not defined or on IO errors.
    static void convertToPbf(const std::string& inPath,
                                                     const std::string& outPath);

        // Utility: Convert any supported OSM input (XML/PBF) to XML (.osm) output.
        // Useful for piping PBF input through the existing XML-only filterWrite path.
        static void convertToXml(const std::string& inPath,
                                                                                                         const std::string& outPath);

                // Streaming filter: Read XML or PBF input and write a filtered PBF directly.
                // Mirrors OsmBuilder::filterWrite semantics (keep/drop rules, bbox, relations trimmed to kept members).
                static void filterToPbf(const std::string& inPath,
                                                                const std::string& outPath,
                                                                const std::vector<OsmReadOpts>& opts,
                                                                const BBoxIdx& bbox);
};

}  // namespace osm
}  // namespace pfaedle

#endif  // PFAEDLE_OSM_OSMPBFREADER_H_

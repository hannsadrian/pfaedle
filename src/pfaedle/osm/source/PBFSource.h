// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef PFAEDLE_OSM_SOURCE_PBFSOURCE_H_
#define PFAEDLE_OSM_SOURCE_PBFSOURCE_H_

#include "pfaedle/osm/source/OsmSource.h"
#include "util/geo/Geo.h"


namespace pfaedle {
namespace osm {
namespace source {

class PBFSource : public OsmSource {
  enum VarType : uint8_t {
    V = 0,
    D = 1,
    S = 2,
    I = 5,
  };

  struct OSMHeader {
    util::geo::Box<double> bbox;
    std::vector<std::string> requiredFeatures;
    std::vector<std::string> optionalFeatures;
    std::string writingProgram;
  };

  struct PrimitiveBlock {
    std::vector<std::string> stringTable;
    std::vector<unsigned char*> primitiveGroups;
    uint32_t granularity = 100;
    uint64_t latOffset = 0;
    uint64_t lonOffset = 0;
    uint32_t dateGranularity = 1000;
  };

  struct BlobHeader {
    std::string type;
    uint32_t datasize;
  };

  struct Blob {
    const char* content;
    uint32_t datasize;
  };

 public:
  PBFSource(const std::string& path);
  virtual ~PBFSource();
  virtual const OsmSourceNode* nextNode();
  virtual const OsmSourceAttr nextAttr();
  virtual const OsmSourceWay* nextWay();
  virtual uint64_t nextMemberNode();
  virtual const OsmSourceRelationMember* nextMember();
  virtual const OsmSourceRelation* nextRel();
  virtual void cont();

  virtual void seekNodes();
  virtual void seekWays();
  virtual void seekRels();

  virtual util::geo::Box<double> getBounds();

  virtual std::string decode(const char* str) const;
  virtual std::string decode(const std::string& str) const;
 private:
  std::string _path;
  int _file;

  unsigned char** _buf;
  unsigned char* _c;

  unsigned char* _blockbuf;

  uint8_t _which = 0;

  void getNextBlock();
  OSMHeader parseOSMHeader(const Blob& blob);
  PrimitiveBlock parseOSMData(const Blob& blob);
  BlobHeader parseBlobHeader(size_t len);
  std::vector<std::string> parseStringTable(unsigned char *& c);
  Blob parseBlob(size_t len);
  std::pair<VarType, uint8_t> nextTypeAndId();
  std::pair<VarType, uint8_t> nextTypeAndId(unsigned char *& c);
  std::string parseString();
  std::string parseString(unsigned char *& c);
  uint32_t parseFixedUInt32();
  uint32_t parseFixedUInt32(unsigned char *& c);
  uint64_t parseFixedUInt64();
  uint64_t parseFixedUInt64(unsigned char *& c);
  int64_t parseVarInt();
  int64_t parseVarInt(unsigned char *& c);
  uint64_t parseVarUInt();
  uint64_t parseVarUInt(unsigned char *& c);
  uint64_t parseUInt(std::pair<VarType, uint8_t> typeId);
  void skipType(VarType type, unsigned char *& c);
  void skipType(VarType type);
  util::geo::Box<double> parseHeaderBBox(unsigned char*& c);
};

}  // namespace source
}  // namespace osm
}  // namespace pfaedle

#endif

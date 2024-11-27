// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include <fcntl.h>
#include <unistd.h>

#include "pfaedle/osm/source/PBFSource.h"
#include "util/Misc.h"
#ifndef PFXML_NO_ZLIB
#include <zlib.h>
#endif

using pfaedle::osm::source::OsmSourceAttr;
using pfaedle::osm::source::OsmSourceNode;
using pfaedle::osm::source::OsmSourceRelation;
using pfaedle::osm::source::OsmSourceRelationMember;
using pfaedle::osm::source::OsmSourceWay;
using pfaedle::osm::source::PBFSource;

static const size_t BUFFER_S = 32 * 1024 * 1024;

// _____________________________________________________________________________
PBFSource::PBFSource(const std::string& path) : _path(path) {
  std::cout << "Init PBF source" << std::endl;

  _file = open(_path.c_str(), O_RDONLY);
  if (_file < 0) throw std::runtime_error(std::string("could not open file"));

  _buf = new unsigned char*[2];
  _buf[0] = new unsigned char[BUFFER_S + 1];
  _buf[1] = new unsigned char[BUFFER_S + 1];
  _blockbuf = new unsigned char[BUFFER_S + 1];

#ifdef __unix__
  posix_fadvise(_file, 0, 0, POSIX_FADV_SEQUENTIAL);
#endif

  /// ___
  read(_file, _buf[_which], BUFFER_S);
  _c = _buf[_which];

  // get header block
  getNextBlock();

  // get first content block
  checkGroup();
}

// _____________________________________________________________________________
bool PBFSource::getNextBlock() {
  // block begins by big-endian 32bit length of BlobHeader
  uint32_t blobHeaderLength = ((*(_c + 3)) << 0) | ((*(_c + 2)) << 8) |
                              ((*(_c + 1)) << 16) | ((*_c) << 24);
  std::cout << "blob header length: " << blobHeaderLength << " bytes"
            << std::endl;
  _c += 4;

  auto header = parseBlobHeader(blobHeaderLength);

  std::cout << "BLOB: <" << header.type << ">, len " << header.datasize
            << std::endl;

  if (header.type == "OSMHeader") {
    parseOSMHeader(parseBlob(header.datasize));
    return true;
  }

  if (header.type == "OSMData") {
    _curBlock = parseOSMData(parseBlob(header.datasize));
    return true;
  }

  return false;
}

// _____________________________________________________________________________
util::geo::Box<double> PBFSource::parseHeaderBBox(unsigned char*& c) {
  auto len = parseVarUInt(c);
  auto start = c;

  int64_t llx, lly, urx, ury;

  while ((size_t)(c - start) < len) {
    auto typeId = nextTypeAndId(c);

    if (typeId.second == 1) llx = parseVarInt(c);
    else if (typeId.second == 4) lly = parseVarInt(c);
    else if (typeId.second == 2) urx = parseVarInt(c);
    else if (typeId.second == 3) ury = parseVarInt(c);
    else skipType(typeId.first, c);
  }

  return {{llx * 1.0, lly * 1.0}, {urx * 1.0, ury * 1.0}};
}

// _____________________________________________________________________________
PBFSource::PrimitiveBlock PBFSource::parseOSMData(const Blob& blob) {
  auto c = reinterpret_cast<unsigned char*>(const_cast<char*>(blob.content));
  auto start = c;

  PBFSource::PrimitiveBlock block;

  while ((size_t)(c - start) < blob.datasize) {
    auto typeId = nextTypeAndId(c);

    if (typeId.second == 1) {
      block.stringTable = parseStringTable(c);
    } else if (typeId.second == 2) {
      block.primitiveGroups.push(c);
      size_t len = parseVarUInt(c);
      c += len;
    } else if (typeId.second == 17) {
      block.granularity = parseVarUInt(c);
    } else if (typeId.second == 19) {
      block.latOffset = parseVarUInt(c);
    } else if (typeId.second == 20) {
      block.lonOffset = parseVarUInt(c);
    } else if (typeId.second == 18) {
      block.dateGranularity = parseVarUInt(c);
    } else {
      skipType(typeId.first, c);
    }
  }

  return block;
}

// _____________________________________________________________________________
std::vector<std::string> PBFSource::parseStringTable(unsigned char *& c) {
  auto start = c;

  size_t len = parseVarUInt(c);

  std::vector<std::string> table;
  table.reserve(len / 10);

  while ((size_t)(c - start) < len) {
    auto typeId = nextTypeAndId(c);

    if (typeId.second == 1) {
      auto str = parseString(c);
      table.push_back(str);
    } else {
      skipType(typeId.first, c);
    }
  }

  return table;
}

// _____________________________________________________________________________
PBFSource::OSMHeader PBFSource::parseOSMHeader(const Blob& blob) {
  auto c = reinterpret_cast<unsigned char*>(const_cast<char*>(blob.content));
  auto start = c;

  PBFSource::OSMHeader header;

  while ((size_t)(c - start) < blob.datasize) {
    auto typeId = nextTypeAndId(c);

    std::cout << "TYPE " << (int)typeId.first << "  " << "ID " << (int)typeId.second << std::endl;

    if (typeId.second == 1) {
      auto a = parseHeaderBBox(c);
    } else if (typeId.second == 4) {
      auto feature = parseString(c);
      header.requiredFeatures.push_back(feature);
    } else if (typeId.second == 5) {
      auto feature = parseString(c);
      header.optionalFeatures.push_back(feature);
    } else if (typeId.second == 16) {
      header.writingProgram = parseString(c);
    } else {
      skipType(typeId.first, c);
    }
  }

  return header;
}

// _____________________________________________________________________________
PBFSource::Blob PBFSource::parseBlob(size_t len) {
  auto start = _c;

  PBFSource::Blob ret;

  while ((size_t)(_c - start) < len) {
    auto typeId = nextTypeAndId();

    if (typeId.second == 1) {
      // raw, no compression
      auto typeId = nextTypeAndId();
      ret.datasize = parseVarUInt();
      ret.content = reinterpret_cast<const char*>(_c);
      _c += ret.datasize;
    } else if (typeId.second == 4) {
      throw std::runtime_error("LZMA compression not supported");
    } else if (typeId.second == 5) {
      throw std::runtime_error("BZIP2 compression not supported");
    } else if (typeId.second == 6) {
      throw std::runtime_error("LZ4 compression not supported");
    } else if (typeId.second == 7) {
      throw std::runtime_error("ZSTD compression not supported");
    } else if (typeId.second == 3) {
      // ZLIB compression
      if (typeId.first != PBFSource::VarType::S) {
        throw std::runtime_error("expected byte array value");
      }
      size_t size = parseVarUInt();
      size_t uncompressedSize = BUFFER_S;
      int a = uncompress(_blockbuf, &uncompressedSize, _c, size);

      ret.content = reinterpret_cast<const char*>(_blockbuf);
      ret.datasize = uncompressedSize;

      _c += size;
    } else if (typeId.second == 2) {
      ret.datasize = parseUInt(typeId);
    } else {
      skipType(typeId.first);
    }
  }

  return ret;
}

// _____________________________________________________________________________
PBFSource::BlobHeader PBFSource::parseBlobHeader(size_t len) {
  auto start = _c;

  BlobHeader ret;

  while ((size_t)(_c - start) < len) {
    auto typeId = nextTypeAndId();

    if (typeId.second == 1) {
      ret.type = parseString();
    } else if (typeId.second == 3) {
      ret.datasize = parseUInt(typeId);
    } else {
      skipType(typeId.first);
    }
  }

  return ret;
}

// _____________________________________________________________________________
uint32_t PBFSource::parseFixedUInt32() { return parseFixedUInt32(_c); }

// _____________________________________________________________________________
uint32_t PBFSource::parseFixedUInt32(unsigned char*& c) {
  uint32_t ret = *(reinterpret_cast<uint32_t*>(c));
  c += 4;
  return ret;
}

// _____________________________________________________________________________
uint64_t PBFSource::parseFixedUInt64() { return parseFixedUInt64(_c); }

// _____________________________________________________________________________
uint64_t PBFSource::parseFixedUInt64(unsigned char*& c) {
  uint64_t ret = *(reinterpret_cast<uint64_t*>(c));
  c += 8;
  return ret;
}

// _____________________________________________________________________________
std::string PBFSource::parseString() { return parseString(_c); }

// _____________________________________________________________________________
std::string PBFSource::parseString(unsigned char*& c) {
  auto strlen = parseVarUInt(c);
  std::string ret = {reinterpret_cast<const char*>(c), strlen};
  c += strlen;

  return ret;
}

// _____________________________________________________________________________
uint64_t PBFSource::parseUInt(std::pair<VarType, uint8_t> typeId) {
  if (typeId.first == PBFSource::VarType::I) {
    return parseFixedUInt32();
  }

  if (typeId.first == PBFSource::VarType::D) {
    return parseFixedUInt64();
  }

  if (typeId.first == PBFSource::VarType::V) {
    return parseVarUInt();
  }

  throw std::runtime_error(std::string("expected integer value"));
}

// _____________________________________________________________________________
int64_t PBFSource::parseVarInt(unsigned char*& c) {
  int64_t i = parseVarUInt(c);
  return (i >> 1) ^ (-(i & 1));
}

// _____________________________________________________________________________
int64_t PBFSource::parseVarInt() { return parseVarInt(_c); }

// _____________________________________________________________________________
uint64_t PBFSource::parseVarUInt(unsigned char*& c) {
  int i = 0;

  uint64_t ret = 0;
  uint64_t cur;

  do {
    cur = *c;
    ret |= (uint64_t)((uint8_t)(cur << 1) >> 1) << (i * 7);
    i++;
    c++;
  } while (cur & (1 << 7));

  return ret;
}

// _____________________________________________________________________________
uint64_t PBFSource::parseVarUInt() { return parseVarUInt(_c); }

// _____________________________________________________________________________
std::pair<PBFSource::VarType, uint8_t> PBFSource::nextTypeAndId() {
  return nextTypeAndId(_c);
}

// _____________________________________________________________________________
std::pair<PBFSource::VarType, uint8_t> PBFSource::nextTypeAndId(
    unsigned char*& c) {

  PBFSource::VarType type = PBFSource::VarType(((unsigned char)((*c) << 5) >> 5));

  uint64_t id = (uint8_t)((uint8_t)((*c) << 1) >> 4);

  int i = 0;

  if ((*c) & (1 << 7)) {
    c++;
    uint64_t cur;

    do {
      cur = *c;
      id |= (uint64_t)((uint8_t)(cur << 1) >> 1) << (4 + i * 7);
      i++;
      c++;
    } while (cur & (1 << 7));
  } else {
    c++;
  }

  return {type, id};
}

// _____________________________________________________________________________
void PBFSource::skipType(PBFSource::VarType type) { skipType(type, _c); }

// _____________________________________________________________________________
void PBFSource::skipType(PBFSource::VarType type, unsigned char*& c) {
  if (type == PBFSource::VarType::V) {
    parseVarUInt(c);
    return;
  }
  if (type == PBFSource::VarType::D) {
    parseFixedUInt64(c);
    return;
  }
  if (type == PBFSource::VarType::S) {
    auto a = parseString(c);
    return;
  }
  if (type == PBFSource::VarType::I) {
    parseFixedUInt32(c);
    return;
  }

  throw std::runtime_error("parse error");
}

// _____________________________________________________________________________
PBFSource::~PBFSource() {
  delete[] _buf[0];
  delete[] _buf[1];
  delete[] _buf;
  delete[] _blockbuf;
}

// _____________________________________________________________________________
const OsmSourceNode* PBFSource::nextNode() {
  do {
    if (_curBlock.denseNodePtr < _curBlock.curDenseNodes.size()) {
      _curNode.id = _curBlock.curDenseNodes[_curBlock.denseNodePtr++].id;
      return &_curNode;
    }

    auto typeId = nextTypeAndId(_curBlock.c);
    if (typeId.second == 1) {
      size_t len = parseVarUInt(_curBlock.c);
      _curBlock.c += len;
    } else if (typeId.second == 2) {
      _curBlock.curDenseNodes = parseDenseNodes(_curBlock.c);
      _curBlock.denseNodePtr = 0;

      if (_curBlock.denseNodePtr < _curBlock.curDenseNodes.size()) {
        _curNode.id = _curBlock.curDenseNodes[_curBlock.denseNodePtr++].id;
        return &_curNode;
      }
    } else {
      // ignore non-nodes here
      size_t len = parseVarUInt(_curBlock.c);
      _curBlock.c += len;

    }
  } while (checkGroup());

  return 0;
}

// _____________________________________________________________________________
void PBFSource::seekNodes() {}

// _____________________________________________________________________________
void PBFSource::seekWays() {}

// _____________________________________________________________________________
void PBFSource::seekRels() {}

// _____________________________________________________________________________
bool PBFSource::checkGroup() {
  while (true) {
    // skip to first non-empty block
    while (_curBlock.primitiveGroups.size() == 0 && getNextBlock()) {}

    if (_curBlock.primitiveGroups.size() == 0) return false;

    if (_curBlock.c == 0) {
      _curBlock.c = _curBlock.primitiveGroups.front();
      _curBlock.curGroupLen = parseVarUInt(_curBlock.c);
      _curBlock.primitiveGroups.front() = _curBlock.c;

      std::cout << "CUR GROUP LEN: " <<  _curBlock.curGroupLen << std::endl;
    }

    if ((size_t)(_curBlock.c - _curBlock.primitiveGroups.front()) >= _curBlock.curGroupLen) {
      _curBlock.primitiveGroups.pop();
      _curBlock.c = 0;
      continue;
    }

    return true;
  }
}

// _____________________________________________________________________________
std::vector<PBFSource::Node> PBFSource::parseDenseNodes(unsigned char*& c) {
  std::vector<PBFSource::Node> ret;

  size_t len = parseVarUInt(c);
  auto start = c;

  while ((size_t)(c - start) < len) {
    auto typeId = nextTypeAndId(c);

    if (typeId.second == 1) {
      // IDs
      size_t len = parseVarUInt(c);
      auto start = c;
      size_t i = 0;
      if (ret.size()) {
        size_t i =0;
        while ((size_t)(c - start) < len) {
          auto nid = parseVarInt(c);
          if (i > 0) nid = ret[i-1].id + nid;
          ret[i++].id = nid;
        }
      } else {
        while ((size_t)(c - start) < len) {
          auto nid = parseVarInt(c);
          if (ret.size() > 0) nid = ret.back().id + nid;
          ret.push_back({0, 0, nid, {}});
        }
      }
    } else if (typeId.second == 5) {
      // denseinfo
      size_t len = parseVarUInt(c);

      c += len;
    } else if (typeId.second == 8) {
      // lat
      size_t len = parseVarUInt(c);

      c += len;
    } else if (typeId.second == 9) {
      // lon
      size_t len = parseVarUInt(c);

      c += len;
    } else if (typeId.second == 10) {
      // tags
      size_t len = parseVarUInt(c);

      c += len;
    }
  }

  return ret;
}

// _____________________________________________________________________________
const OsmSourceWay* PBFSource::nextWay() { return 0; }

// _____________________________________________________________________________
const OsmSourceRelationMember* PBFSource::nextMember() { return 0; }

// _____________________________________________________________________________
uint64_t PBFSource::nextMemberNode() { return 0; }

// _____________________________________________________________________________
const OsmSourceRelation* PBFSource::nextRel() { return 0; }

// _____________________________________________________________________________
const OsmSourceAttr PBFSource::nextAttr() {
  return {0, 0};
}

// _____________________________________________________________________________
util::geo::Box<double> PBFSource::getBounds() {}

// _____________________________________________________________________________
std::string PBFSource::decode(const char* str) const {
  return str;  // TODO
}

// _____________________________________________________________________________
std::string PBFSource::decode(const std::string& str) const {
  return str;  // TODO
}

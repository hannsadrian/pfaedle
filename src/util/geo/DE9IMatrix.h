// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_GEO_DE9IMATRIX_H_
#define UTIL_GEO_DE9IMATRIX_H_

// hacky helper function for writing 16 bit binary constants prior to C++ 14
#define bit16_(x)                                                              \
  (((x & 0x0000000000000001) ? 1 : 0) + ((x & 0x0000000000000010) ? 2 : 0) +   \
   ((x & 0x0000000000000100) ? 4 : 0) + ((x & 0x0000000000001000) ? 8 : 0) +   \
   ((x & 0x0000000000010000) ? 16 : 0) + ((x & 0x0000000000100000) ? 32 : 0) + \
   ((x & 0x0000000001000000) ? 64 : 0) +                                       \
   ((x & 0x0000000010000000) ? 128 : 0) +                                      \
   ((x & 0x0000000100000000) ? 256 : 0) +                                      \
   ((x & 0x0000001000000000) ? 512 : 0) +                                      \
   ((x & 0x0000010000000000) ? 1024 : 0) +                                     \
   ((x & 0x0000100000000000) ? 2048 : 0) +                                     \
   ((x & 0x0001000000000000) ? 4096 : 0) +                                     \
   ((x & 0x0010000000000000) ? 8192 : 0) +                                     \
   ((x & 0x0100000000000000) ? 16384 : 0) +                                    \
   ((x & 0x1000000000000000) ? 32768 : 0))

#if __cplusplus >= 201402L
#define bit16(d) (0b##d)
#else
#define bit16(d) ((uint16_t)bit16_(0x##d))
#endif

#if __cplusplus >= 201402L
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

#include <iostream>

namespace util {
namespace geo {

const static uint16_t F = bit16(00);
const static uint16_t D0 = bit16(01);
const static uint16_t D1 = bit16(10);
const static uint16_t D2 = bit16(11);

class DE9IMatrix {
 public:
  DE9IMatrix() {}

  CONSTEXPR DE9IMatrix(const char* m) {
    if (!m) return;
    for (size_t i = 0; i < 8; i++) {
      if (!m[i]) return;
      if (m[i] == '0') _m |= (D0 << (i * 2));
      if (m[i] == '1') _m |= (D1 << (i * 2));
      if (m[i] == '2') _m |= (D2 << (i * 2));
    }
  }

  DE9IMatrix(const std::string& m) : DE9IMatrix(m.c_str()) {}

  DE9IMatrix(const uint16_t m) : _m(m) {}

  std::string toString() const {
    std::string ret = "FFFFFFFF2";
    for (size_t i = 0; i < 8; i++) {
      uint8_t v = (_m & (3 << (i * 2))) >> (i * 2);
      if (v == F) ret[i] = 'F';
      if (v == D0) ret[i] = '0';
      if (v == D1) ret[i] = '1';
      if (v == D2) ret[i] = '2';
    }
    return ret;
  }

  void setTo(size_t i, uint16_t v) {
    _m &= ~(D2 << (i * 2));
    _m |= (v << (i * 2));
  }

  uint16_t get(size_t i) const {
    uint16_t ret = _m & (D2 << (i * 2));
    return ret >> (i * 2);
  }

  void II(uint16_t v) { setTo(0, v); }
  void IB(uint16_t v) { setTo(1, v); }
  void IE(uint16_t v) { setTo(2, v); }
  void BI(uint16_t v) { setTo(3, v); }
  void BB(uint16_t v) { setTo(4, v); }
  void BE(uint16_t v) { setTo(5, v); }
  void EI(uint16_t v) { setTo(6, v); }
  void EB(uint16_t v) { setTo(7, v); }

  uint16_t II() const { return get(0); }
  uint16_t IB() const { return get(1); }
  uint16_t IE() const { return get(2); }
  uint16_t BI() const { return get(3); }
  uint16_t BB() const { return get(4); }
  uint16_t BE() const { return get(5); }
  uint16_t EI() const { return get(6); }
  uint16_t EB() const { return get(7); }

  DE9IMatrix transpose() const {
    DE9IMatrix ret;
    ret.II(II());
    ret.IB(BI());
    ret.IE(EI());
    ret.BI(IB());
    ret.BB(BB());
    ret.BE(EB());
    ret.EI(IE());
    ret.EB(BE());
    return ret;
  }

  DE9IMatrix& operator+=(const DE9IMatrix& other) {
    for (size_t i = 0; i < 8; i++) {
      if (other.get(i) >= get(i))
        setTo(i, other.get(i));
      else
        setTo(i, get(i));
    }
    return *this;
  }

  // see https://en.wikipedia.org/wiki/DE-9IM
  bool disjoint() const { return !II() && !IB() && !BI() && !BB(); }
  bool contains() const { return II() && !EI() && !EB(); }
  bool covers() const {
    return (II() || IB() || BI() || BB()) && !EI() && !EB();
  }
  bool equals() const { return II() && !IE() && !BE() && !EI() && !EB(); }
  bool touches() const { return !II() && (IB() || BI() || BB()); }
  bool intersects() const { return !disjoint(); }
  bool within() const { return II() && !IE() && !BE(); }
  bool covered() const {
    return !IE() && !BE() && (II() || IB() || BI() || BB());
  }
  bool coveredBy() const { return covered(); }
  bool overlaps02() const { return II() && IE() && EI(); }

  bool overlaps1() const { return II() == D1 && IE() && EI(); }

  CONSTEXPR uint16_t getMatrix() const { return _m; }

 private:
  uint16_t _m = 0;
};

class DE9IMFilter {
 public:
  DE9IMFilter() {}

  CONSTEXPR DE9IMFilter(const char* m) {
    if (!m) return;
    for (size_t i = 0; i < 8; i++) {
      if (!m[i]) return;
      if (m[i] >= '0' && m[i] <= '2') {
        _equalityMask |= (D2 << (i * 2));
        _equalityValues |= ((m[i] - '0' + 1) << (i * 2));
      }
      if (m[i] == 'F') {
        _equalityMask |= (D2 << (i * 2));
      }
      if (m[i] == 'T') {
        _trueMask |= (D2 << (i * 2));
      }
    }
    if (!m[8]) return;

    if (m[8] == 'F') _eeVal = F;
    if (m[8] == '0') _eeVal = D0;
    if (m[8] == '1') _eeVal = D1;
    if (m[8] == '2') _eeVal = D2;
    if (m[8] == 'T') _eeVal = D2 + 1;
  }

  bool matches(const DE9IMatrix a) const {
    if (_eeVal == F || _eeVal == D0 || _eeVal == D1) return false;
    if ((a.getMatrix() & _equalityMask) != _equalityValues) return false;

    // the _trueMask has a 11 pair where matrix pair values have to be 01, 10
    // or 11. by shifting the trueMask'ed matrix to the right by 1, and or'ing
    // this shifted matrix with the original trueMask'ed matrix, we get a matrix
    // where the lower bit of each pair is 1 if the original pair had at least 1
    // bit set. To avoid an overflow of a set bit into an adjacent previously
    // unset pair, we then mask to only the lower bits (the overflow can only
    // happen into the upper bit). This gives as something we can directly
    // compare to the masked _trueMask

    if (_trueMask &&
        ((((a.getMatrix() & _trueMask) >> 1) | (a.getMatrix() & _trueMask)) &
         bit16(0101010101010101)) != (_trueMask & bit16(0101010101010101)))
      return false;

    return true;
  }

  std::string toString() const {
    std::string ret = "*********";

    for (size_t i = 0; i < 8; i++) {
      uint8_t eqMask = (_equalityMask & (3 << (i * 2))) >> (i * 2);
      if (!eqMask) continue;
      uint8_t v = (_equalityValues & (3 << (i * 2))) >> (i * 2);

      if (v == F) ret[i] = 'F';
      if (v == D0) ret[i] = '0';
      if (v == D1) ret[i] = '1';
      if (v == D2) ret[i] = '2';
    }

    for (size_t i = 0; i < 8; i++) {
      uint8_t trueMask = (_trueMask & (3 << (i * 2))) >> (i * 2);
      if (!trueMask) continue;
      ret[i] = 'T';
    }

    if (_eeVal == F) ret[8] = 'F';
    if (_eeVal == D0) ret[8] = '0';
    if (_eeVal == D1) ret[8] = '1';
    if (_eeVal == D2) ret[8] = '2';
    if (_eeVal == D2 + 1) ret[8] = 'T';
    if (_eeVal == D2 + 2) ret[8] = '*';

    return ret;
  }

 private:
  uint16_t _equalityMask = 0;
  uint16_t _trueMask = 0;
  uint16_t _equalityValues = 0;
  uint8_t _eeVal = D2 + 2;
};

// often used matrices
static CONSTEXPR DE9IMatrix M0FFFFF102("0FFFFF102");
static CONSTEXPR DE9IMatrix MF0FFFF102("F0FFFF102");
static CONSTEXPR DE9IMatrix MFF0FFF102("FF0FFF102");
static CONSTEXPR DE9IMatrix M0F1FF0FF2("0F1FF0FF2");
static CONSTEXPR DE9IMatrix MFF1F00FF2("FF1F00FF2");
static CONSTEXPR DE9IMatrix MFF1FF00F2("FF1FF00F2");
static CONSTEXPR DE9IMatrix M0FFFFF212("0FFFFF212");
static CONSTEXPR DE9IMatrix MF0FFFF212("F0FFFF212");
static CONSTEXPR DE9IMatrix MFF0FFF212("FF0FFF212");
static CONSTEXPR DE9IMatrix M0F2FF1FF2("0F2FF1FF2");
static CONSTEXPR DE9IMatrix MFF20F1FF2("FF20F1FF2");
static CONSTEXPR DE9IMatrix MFF2FF10F2("FF2FF10F2");
static CONSTEXPR DE9IMatrix MFF1FF0102("FF1FF0102");
static CONSTEXPR DE9IMatrix MFF2FF1212("FF2FF1212");
static CONSTEXPR DE9IMatrix M0FFFFFFF2("0FFFFFFF2");
static CONSTEXPR DE9IMatrix MFF0FFF0F2("FF0FFF0F2");

inline DE9IMatrix operator+(const DE9IMatrix a, const DE9IMatrix b) {
  DE9IMatrix ret;

  for (size_t i = 0; i < 8; i++) {
    if (a.get(i) >= b.get(i))
      ret.setTo(i, a.get(i));
    else
      ret.setTo(i, b.get(i));
  }

  return ret;
}

inline bool operator&(const DE9IMFilter a, const DE9IMatrix b) {
  return a.matches(b);
}

inline bool operator&(const DE9IMatrix a, const DE9IMFilter b) {
  return b.matches(a);
}

inline std::ostream& operator<<(std::ostream& os, const DE9IMatrix a) {
  os << a.toString();
  return os;
}

inline bool operator==(const DE9IMatrix a, const DE9IMatrix b) {
  return a.getMatrix() == b.getMatrix();
}

inline bool operator!=(const std::string& a, const DE9IMatrix b) {
  return a != b.toString();
}

inline bool operator!=(const DE9IMatrix a, const std::string& b) {
  return b != a;
}

inline bool operator!=(const DE9IMatrix a, const DE9IMatrix b) {
  return a.getMatrix() != b.getMatrix();
}

}  // namespace geo
}  // namespace util

#endif  // UTIL_GEO_DE9IMATRIX_H_

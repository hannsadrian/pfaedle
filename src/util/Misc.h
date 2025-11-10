// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_MISC_H_
#define UTIL_MISC_H_

#define FMT_HEADER_ONLY

#include <map>

#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>
#ifdef PBUTIL_ZLIB_FOUND
#include <zlib.h>
#endif
#ifdef PBUTIL_BZIP2_FOUND
#include <bzlib.h>
#endif
#include <sys/types.h>
#include "JobQueue.h"

static const size_t SORT_BUFFER_S = 64 * 128 * 1024;

#define UNUSED(expr) do { (void)(expr); } while (0)
#define TIME() std::chrono::high_resolution_clock::now()
#define TOOK_UNTIL(t1, t2) (std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count())
#define TOOK(t1) (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - t1).count())
#define T_START(n)  auto _tstart_##n = std::chrono::high_resolution_clock::now()
#define T_STOP(n) (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _tstart_##n).count())

#define TODO(msg) std::cerr << "\n" __FILE__ << ":" << __LINE__ << ": TODO: " << #msg << std::endl;

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>
#endif
#else
#error "Cannot define getPeakRSS( ) or getCurrentRSS( ) for an unknown OS."
#endif

namespace util {

struct SortJob {
  SortJob() : part(0), partbuf(0) {}
  SortJob(size_t part, unsigned char* partbuf) : part(part), partbuf(partbuf) {}
  size_t part = 0;
  unsigned char* partbuf = 0;
};

inline bool operator==(const SortJob& a, const SortJob& b) {
  return a.part == b.part && a.partbuf == b.partbuf;
}

inline bool operator!=(const SortJob& a, const SortJob& b) { return !(a == b); }

const static std::map<std::string, std::string> HTML_COLOR_NAMES = {
    {"aliceblue", "F0F8FF"},
    {"antiquewhite", "FAEBD7"},
    {"aqua", "00FFFF"},
    {"aquamarine", "7FFFD4"},
    {"azure", "F0FFFF"},
    {"beige", "F5F5DC"},
    {"bisque", "FFE4C4"},
    {"black", "000000"},
    {"blanchedalmond", "FFEBCD"},
    {"blue", "0000FF"},
    {"blueviolet", "8A2BE2"},
    {"brown", "A52A2A"},
    {"burlywood", "DEB887"},
    {"cadetblue", "5F9EA0"},
    {"chartreuse", "7FFF00"},
    {"chocolate", "D2691E"},
    {"coral", "FF7F50"},
    {"cornflowerblue", "6495ED"},
    {"cornsilk", "FFF8DC"},
    {"crimson", "DC143C"},
    {"cyan", "00FFFF"},
    {"darkblue", "00008B"},
    {"darkcyan", "008B8B"},
    {"darkgoldenrod", "B8860B"},
    {"darkgray", "A9A9A9"},
    {"darkgreen", "006400"},
    {"darkgrey", "A9A9A9"},
    {"darkkhaki", "BDB76B"},
    {"darkmagenta", "8B008B"},
    {"darkolivegreen", "556B2F"},
    {"darkorange", "FF8C00"},
    {"darkorchid", "9932CC"},
    {"darkred", "8B0000"},
    {"darksalmon", "E9967A"},
    {"darkseagreen", "8FBC8F"},
    {"darkslateblue", "483D8B"},
    {"darkslategray", "2F4F4F"},
    {"darkslategrey", "2F4F4F"},
    {"darkturquoise", "00CED1"},
    {"darkviolet", "9400D3"},
    {"deeppink", "FF1493"},
    {"deepskyblue", "00BFFF"},
    {"dimgray", "696969"},
    {"dimgrey", "696969"},
    {"dodgerblue", "1E90FF"},
    {"firebrick", "B22222"},
    {"floralwhite", "FFFAF0"},
    {"forestgreen", "228B22"},
    {"fuchsia", "FF00FF"},
    {"gainsboro", "DCDCDC"},
    {"ghostwhite", "F8F8FF"},
    {"gold", "FFD700"},
    {"goldenrod", "DAA520"},
    {"gray", "808080"},
    {"green", "008000"},
    {"greenyellow", "ADFF2F"},
    {"grey", "808080"},
    {"honeydew", "F0FFF0"},
    {"hotpink", "FF69B4"},
    {"indianred", "CD5C5C"},
    {"indigo", "4B0082"},
    {"ivory", "FFFFF0"},
    {"khaki", "F0E68C"},
    {"lavender", "E6E6FA"},
    {"lavenderblush", "FFF0F5"},
    {"lawngreen", "7CFC00"},
    {"lemonchiffon", "FFFACD"},
    {"lightblue", "ADD8E6"},
    {"lightcoral", "F08080"},
    {"lightcyan", "E0FFFF"},
    {"lightgoldenrodyellow", "FAFAD2"},
    {"lightgray", "D3D3D3"},
    {"lightgreen", "90EE90"},
    {"lightgrey", "D3D3D3"},
    {"lightpink", "FFB6C1"},
    {"lightsalmon", "FFA07A"},
    {"lightseagreen", "20B2AA"},
    {"lightskyblue", "87CEFA"},
    {"lightslategray", "778899"},
    {"lightslategrey", "778899"},
    {"lightsteelblue", "B0C4DE"},
    {"lightyellow", "FFFFE0"},
    {"lime", "00FF00"},
    {"limegreen", "32CD32"},
    {"linen", "FAF0E6"},
    {"magenta", "FF00FF"},
    {"maroon", "800000"},
    {"mediumaquamarine", "66CDAA"},
    {"mediumblue", "0000CD"},
    {"mediumorchid", "BA55D3"},
    {"mediumpurple", "9370DB"},
    {"mediumseagreen", "3CB371"},
    {"mediumslateblue", "7B68EE"},
    {"mediumspringgreen", "00FA9A"},
    {"mediumturquoise", "48D1CC"},
    {"mediumvioletred", "C71585"},
    {"midnightblue", "191970"},
    {"mintcream", "F5FFFA"},
    {"mistyrose", "FFE4E1"},
    {"moccasin", "FFE4B5"},
    {"navajowhite", "FFDEAD"},
    {"navy", "000080"},
    {"oldlace", "FDF5E6"},
    {"olive", "808000"},
    {"olivedrab", "6B8E23"},
    {"orange", "FFA500"},
    {"orangered", "FF4500"},
    {"orchid", "DA70D6"},
    {"palegoldenrod", "EEE8AA"},
    {"palegreen", "98FB98"},
    {"paleturquoise", "AFEEEE"},
    {"palevioletred", "DB7093"},
    {"papayawhip", "FFEFD5"},
    {"peachpuff", "FFDAB9"},
    {"peru", "CD853F"},
    {"pink", "FFC0CB"},
    {"plum", "DDA0DD"},
    {"powderblue", "B0E0E6"},
    {"purple", "800080"},
    {"red", "FF0000"},
    {"rosybrown", "BC8F8F"},
    {"royalblue", "4169E1"},
    {"saddlebrown", "8B4513"},
    {"salmon", "FA8072"},
    {"sandybrown", "F4A460"},
    {"seagreen", "2E8B57"},
    {"seashell", "FFF5EE"},
    {"sienna", "A0522D"},
    {"silver", "C0C0C0"},
    {"skyblue", "87CEEB"},
    {"slateblue", "6A5ACD"},
    {"slategray", "708090"},
    {"slategrey", "708090"},
    {"snow", "FFFAFA"},
    {"springgreen", "00FF7F"},
    {"steelblue", "4682B4"},
    {"tan", "D2B48C"},
    {"teal", "008080"},
    {"thistle", "D8BFD8"},
    {"tomato", "FF6347"},
    {"turquoise", "40E0D0"},
    {"violet", "EE82EE"},
    {"wheat", "F5DEB3"},
    {"white", "FFFFFF"},
    {"whitesmoke", "F5F5F5"},
    {"yellow", "FFFF00"},
    {"yellowgreen", "9ACD32"}};

struct hashPair {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2>& p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);
    return h1 ^ h2;
  }
};

template <typename Key, typename Val, Val Def>
class SparseMatrix {
 public:
  Val get(const Key& x, const Key& y) const {
    auto a = _m.find(std::pair<Key, Key>(x, y));
    if (a == _m.end()) return Def;
    return a->second;
  }

  void set(Key x, Key y, Val v) { _m[std::pair<Key, Key>(x, y)] = v; }

  const std::map<std::pair<Key, Key>, Val>& vals() const { return _m; }

 private:
  std::map<std::pair<Key, Key>, Val> _m;
};

uint64_t factorial(uint64_t n);

uint64_t atoul(const char* p);

bool isFloatingPoint(const std::string& str);

std::string formatFloat(double f, int DIGITS);

double atof(const char* p, uint8_t mn);

ssize_t preadAll(int file, unsigned char* buf, size_t count, size_t offset);

#ifdef PBUTIL_ZLIB_FOUND
ssize_t zreadAll(gzFile file, unsigned char* buf, size_t count);
#endif

#ifdef PBUTIL_BZIP2_FOUND
ssize_t bz2readAll(BZFILE* file, unsigned char* buf, size_t count);
#endif

ssize_t readAll(int file, unsigned char* buf, size_t count);

ssize_t pwriteAll(int file, const unsigned char* buf, size_t count,
                  size_t offset);

ssize_t writeAll(int file, const unsigned char* buf, size_t count);

double atof(const char* p);

// _____________________________________________________________________________
template <typename V>
int merge(V* lst, V* tmpLst, size_t l, size_t m, size_t r) {
  size_t ret = 0;

  size_t lp = l;
  size_t rp = m;
  size_t outp = l;

  while (lp < m && rp < r + 1) {
    if (lst[lp] <= lst[rp]) {
      // if left element is smaller or equal, add it to return list,
      // increase left pointer
      tmpLst[outp] = lst[lp];
      lp++;
    } else {
      // if left element is bigger, add the right element, add it to ret,
      // increase right pointer
      tmpLst[outp] = lst[rp];
      rp++;

      // if the left element was bigger, everything to the right in the
      // left list is also bigger, and all these m - i elements were
      // initially in the wrong order! Count these inversions.
      ret += m - lp;
    }

    outp++;
  }

  // fill in remaining values
  if (lp < m) std::memcpy(tmpLst + outp, lst + lp, (m - lp) * sizeof(V));
  if (rp <= r) std::memcpy(tmpLst + outp, lst + rp, ((r + 1) - rp) * sizeof(V));

  // copy to output
  std::memcpy(lst + l, tmpLst + l, ((r + 1) - l) * sizeof(V));

  return ret;
}

// _____________________________________________________________________________
template <typename V>
size_t mergeInvCount(V* lst, V* tmpLst, size_t l, size_t r) {
  size_t ret = 0;
  if (l < r) {
    size_t m = (r + l) / 2;

    ret += mergeInvCount(lst, tmpLst, l, m);
    ret += mergeInvCount(lst, tmpLst, m + 1, r);

    ret += merge(lst, tmpLst, l, m + 1, r);
  }
  return ret;
}

// _____________________________________________________________________________
template <typename V>
size_t inversions(const std::vector<V>& v) {
  if (v.size() < 2) return 0;  // no inversions possible

  // unroll some simple cases
  if (v.size() == 2) return v[1] < v[0];
  if (v.size() == 3) return (v[0] > v[1]) + (v[0] > v[2]) + (v[1] > v[2]);

  auto tmpLst = new V[v.size()];
  auto lst = new V[v.size()];

  for (size_t i = 0; i < v.size(); i++) lst[i] = v[i];

  size_t ret = mergeInvCount<V>(lst, tmpLst, 0, v.size() - 1);
  delete[] tmpLst;
  delete[] lst;
  return ret;
}

std::string getHomeDir();

std::string getTmpDir();

std::string getTmpFName(std::string dir, std::string name, std::string postf);

std::string rgbToHex(int r, int g, int b);

void hsvToRgb(float* r, float* g, float* b, float h, float s, float v);

std::string normHtmlColor(const std::string& col);

std::string randomHtmlColor();

char* readableSize(double size, size_t n, char* buf);

// _____________________________________________________________________________
inline std::string readableSize(double size) {
  char buffer[30];
  return readableSize(size, 30, buffer);
}

void sortPart(int file, size_t objSize, size_t part, unsigned char* buf,
              unsigned char* partbuf, size_t bufferSize, size_t partsBufSize,
              size_t* partsize, int (*cmpf)(const void*, const void*));

void processSortQueue(util::JobQueue<SortJob>* jobs, int file, size_t objSize,
                      unsigned char* buf, size_t bufferSize,
                      size_t partsBufSize, size_t* partsize,
                      int (*cmpf)(const void*, const void*));

ssize_t externalSort(int file, int newFile, size_t size, size_t numobjs,
                     size_t numThreads, int (*cmpf)(const void*, const void*));

// _____________________________________________________________________________
class approx {
 public:
  explicit approx(double magnitude)
      : _epsilon{std::numeric_limits<float>::epsilon() * 100},
        _magnitude{magnitude} {}

  friend bool operator==(double lhs, approx const& rhs) {
    return std::abs(lhs - rhs._magnitude) < rhs._epsilon;
  }

  friend bool operator==(approx const& lhs, double rhs) {
    return operator==(rhs, lhs);
  }

  friend bool operator!=(double lhs, approx const& rhs) {
    return !operator==(lhs, rhs);
  }

  friend bool operator!=(approx const& lhs, double rhs) {
    return !operator==(rhs, lhs);
  }

  friend bool operator<=(double lhs, approx const& rhs) {
    return lhs < rhs._magnitude || lhs == rhs;
  }

  friend bool operator<=(approx const& lhs, double rhs) {
    return lhs._magnitude < rhs || lhs == rhs;
  }

  friend bool operator>=(double lhs, approx const& rhs) {
    return lhs > rhs._magnitude || lhs == rhs;
  }

  friend bool operator>=(approx const& lhs, double rhs) {
    return lhs._magnitude > rhs || lhs == rhs;
  }

  friend std::ostream& operator<<(std::ostream& out, const approx& a) {
    out << "~" << a._magnitude;
    return out;
  }

 private:
  double _epsilon;
  double _magnitude;
};

size_t getPeakRSS();
size_t getCurrentRSS();

}  // namespace util

#endif  // UTIL_MISC_H_

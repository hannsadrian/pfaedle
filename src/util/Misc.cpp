// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include "./Misc.h"

#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <thread>

#include "3rdparty/fmt/core.h"
#include "String.h"

// cached first 10 powers of 10
static int pow10[10] = {1,      10,      100,      1000,      10000,
                        100000, 1000000, 10000000, 100000000, 1000000000};

// _____________________________________________________________________________
uint64_t util::factorial(uint64_t n) {
  if (n < 2) return 1;
  return n * factorial(n - 1);
}

// _____________________________________________________________________________
uint64_t util::atoul(const char* p) {
  uint64_t ret = 0;

  while (*p) {
    ret = ret * 10 + (*p++ - '0');
  }

  return ret;
}

// _____________________________________________________________________________
bool util::isFloatingPoint(const std::string& str) {
  std::stringstream ss(str);
  double f;
  ss >> std::noskipws >> f;
  return ss.eof() && !ss.fail();
}

// _____________________________________________________________________________
std::string util::formatFloat(double f, int DIGITS) {
  std::string fStr = "{:." + std::to_string(DIGITS) + "f}";

  fmt::memory_buffer buf;
  fmt::vformat_to(std::back_inserter(buf), fStr, fmt::make_format_args(f));

  auto ret = fmt::to_string(buf);

  if (ret.back() == '0') {
    if (ret.find('.') != std::string::npos) {
      auto p = ret.find_last_not_of('0');
      if (ret[p] == '.') return ret.substr(0, p);
      return ret.substr(0, p + 1);
    }
  }

  return ret;
}

// _____________________________________________________________________________
double util::atof(const char* p, uint8_t mn) {
  // this atof implementation works only on "normal" float strings like
  // 56.445 or -345.00, but should be faster than std::atof
  while (*p && (*p == ' ' || *p == '\n' || *p == '\t' || *p == '\r')) p++;

  double ret = 0.0;
  bool neg = false;
  if (*p == '-') {
    neg = true;
    p++;
  }

  while (*p >= '0' && *p <= '9') {
    ret = ret * 10.0 + (*p - '0');
    p++;
  }

  if (*p == '.') {
    p++;
    double f = 0;
    uint8_t n = 0;

    for (; n < mn && *p >= '0' && *p <= '9'; n++, p++) {
      f = f * 10.0 + (*p - '0');
    }

    if (n < 10) {
      ret += f / pow10[n];
    } else {
      double res = 1;
      double base = 10;
      while (n > 0) {
        if (n & 1) res *= base;
        base *= base;
        n >>= 1;
      }
      ret += f / res;
    }
  }

  if (neg) return -ret;
  return ret;
}

// _____________________________________________________________________________
ssize_t util::preadAll(int file, unsigned char* buf, size_t count,
                       size_t offset) {
  ssize_t r;
  ssize_t rem = count;

  while ((r = pread(file, buf + (count - rem), rem, offset))) {
    if (r < 0) return -1;
    rem -= r;
    offset += r;
  }

  return count - rem;
}

// _____________________________________________________________________________
#ifdef PBUTIL_ZLIB_FOUND
ssize_t util::zreadAll(gzFile file, unsigned char* buf, size_t count) {
  ssize_t r;
  ssize_t rem = count;

  while ((r = gzread(file, buf + (count - rem), rem))) {
    if (r < 0) return -1;
    rem -= r;
  }

  return count - rem;
}
#endif

// _____________________________________________________________________________
#ifdef PBUTIL_BZIP2_FOUND
ssize_t util::bz2readAll(BZFILE* file, unsigned char* buf, size_t count) {
  ssize_t r;
  ssize_t rem = count;

  int err;

  while ((r = BZ2_bzRead(&err, file, buf + (count - rem), rem))) {
    if (r < 0) return -1;
    rem -= r;
  }

  return count - rem;
}
#endif

// _____________________________________________________________________________
ssize_t util::readAll(int file, unsigned char* buf, size_t count) {
  ssize_t r;
  ssize_t rem = count;

  while ((r = read(file, buf + (count - rem), rem))) {
    if (r < 0) return -1;
    rem -= r;
  }

  return count - rem;
}

// _____________________________________________________________________________
ssize_t util::pwriteAll(int file, const unsigned char* buf, size_t count,
                        size_t offset) {
  ssize_t r;
  ssize_t rem = count;

  while ((r = pwrite(file, buf + (count - rem), rem, offset))) {
    if (r < 0) return -1;
    rem -= r;
    offset += r;
  }

  return count - rem;
}

// _____________________________________________________________________________
ssize_t util::writeAll(int file, const unsigned char* buf, size_t count) {
  ssize_t r;
  ssize_t rem = count;
  while ((r = write(file, buf + (count - rem), rem))) {
    if (r < 0) return -1;
    rem -= r;
  }

  return count - rem;
}

// _____________________________________________________________________________
double util::atof(const char* p) { return atof(p, 38); }

// _____________________________________________________________________________
std::string util::getHomeDir() {
  // parse implicit paths
  const char* homedir = 0;
  char* buf = 0;

  if ((homedir = getenv("HOME")) == 0) {
    homedir = "";
    struct passwd pwd;
    struct passwd* result;
    size_t bufsize;
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == static_cast<size_t>(-1)) bufsize = 0x4000;
    buf = static_cast<char*>(malloc(bufsize));
    if (buf != 0) {
      getpwuid_r(getuid(), &pwd, buf, bufsize, &result);
      if (result != NULL) homedir = result->pw_dir;
    }
  }

  std::string ret(homedir);
  if (buf) free(buf);

  return ret;
}

// _____________________________________________________________________________
std::string util::getTmpDir() {
  // first, check if an env variable is set
  const char* tmpdir = getenv("TMPDIR");
  if (tmpdir && std::strlen(tmpdir)) return std::string(tmpdir);

  // second, check if /tmp is writable
  if (access("/tmp/", W_OK) == 0) return "/tmp";

  // third, check if the cwd is writable
  if (access(".", W_OK) == 0) return ".";

  // lastly, return the users home directory as a fallback
  return getHomeDir();
}

// ___________________________________________________________________________
void util::hsvToRgb(float* r, float* g, float* b, float h, float s, float v) {
  int i;
  float f, p, q, t;

  if (s == 0) {
    *r = *g = *b = v;
    return;
  }

  h /= 60;
  i = floor(h);
  f = h - i;
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));

  switch (i) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    default:
      *r = v;
      *g = p;
      *b = q;
      break;
  }
}

// ___________________________________________________________________________
std::string util::normHtmlColor(const std::string& col) {
  auto i = HTML_COLOR_NAMES.find(toLower(col));
  if (i != HTML_COLOR_NAMES.end()) return i->second;
  return col;
}

// ___________________________________________________________________________
std::string util::randomHtmlColor() {
  double goldenRatio = 0.618033988749895;
  double h = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  h += goldenRatio;
  h = fmod(h, 1.0);
  float r, g, b;
  hsvToRgb(&r, &g, &b, h * 360, 0.95, 0.95);
  return rgbToHex(r * 256, g * 256, b * 256);
}

// _____________________________________________________________________________
std::string util::rgbToHex(int r, int g, int b) {
  char hexcol[16];
  snprintf(hexcol, sizeof hexcol, "%02x%02x%02x", r, g, b);
  return hexcol;
}

// _____________________________________________________________________________
void util::sortPart(int file, size_t objSize, size_t part, unsigned char* buf,
                    unsigned char* partbuf, size_t bufferSize,
                    size_t partsBufSize, size_t* partsize,
                    int (*cmpf)(const void*, const void*)) {
  // read entire part to buf
  ssize_t n = preadAll(file, buf, bufferSize, bufferSize * part);
  if (n < 0) {
    std::cerr << strerror(errno) << std::endl;
    return;
  }

  // sort entire part in memory
  qsort(buf, n / objSize, objSize, cmpf);

  // write entire part, now sorted, back to file, to the same position
  ssize_t r = pwriteAll(file, buf, n, bufferSize * part);
  if (r < 0) {
    std::cerr << strerror(errno) << std::endl;
    return;
  }

  // already copy the beginning of the read part to the parts buffer
  memcpy(partbuf, buf, std::min<size_t>(n, partsBufSize));

  // save size of this part (which might be different than bufferSize for the
  // last part (might be even 0)
  partsize[part] = n;
}

// _____________________________________________________________________________
void util::processSortQueue(util::JobQueue<SortJob>* jobs, int file,
                            size_t objSize, unsigned char* buf,
                            size_t bufferSize, size_t partsBufSize,
                            size_t* partsize,
                            int (*cmpf)(const void*, const void*)) {
  SortJob job;
  while ((job = jobs->get()).partbuf != 0) {
    sortPart(file, objSize, job.part, buf, job.partbuf, bufferSize,
             partsBufSize, partsize, cmpf);
  }
}

// _____________________________________________________________________________
ssize_t util::externalSort(int file, int newFile, size_t size, size_t numobjs,
                           size_t numThreads,
                           int (*cmpf)(const void*, const void*)) {
  // sort a file via an external sort. Sorting is parallel (with numThreads
  // parallel threads), merging of the sorted parts is sequential

  size_t fsize = size * numobjs;

  size_t bufferSize = SORT_BUFFER_S * size;

  size_t parts = fsize / bufferSize + 1;
  size_t partsBufSize = ((bufferSize / size) / parts + 1) * size;
  unsigned char** bufs = new unsigned char*[numThreads];
  unsigned char** partbufs = new unsigned char*[parts];
  size_t* partpos = new size_t[parts];
  size_t* partsize = new size_t[parts];

  // fire up worker threads for geometry checking
  std::vector<std::thread> thrds(numThreads);

  auto pqComp = [cmpf](const std::pair<const void*, size_t>& a,
                       const std::pair<const void*, size_t>& b) {
    return cmpf(a.first, b.first) != -1;
  };
  std::priority_queue<std::pair<const void*, size_t>,
                      std::vector<std::pair<const void*, size_t>>,
                      decltype(pqComp)>
      pq(pqComp);

  util::JobQueue<SortJob> jobQ;

  // sort the 'parts' number of file parts independently
  for (size_t i = 0; i < parts; i++) {
    partbufs[i] = new unsigned char[partsBufSize];
    partpos[i] = 0;
    partsize[i] = 0;

    jobQ.add({i, partbufs[i]});
  }

  // the DONE element on the job queue to signal all threads to shut down
  jobQ.add({});

  for (size_t t = 0; t < numThreads; t++) {
    bufs[t] = new unsigned char[bufferSize];
    thrds[t] = std::thread(&processSortQueue, &jobQ, file, size, bufs[t],
                           bufferSize, partsBufSize, partsize, cmpf);
  }

  for (auto& th : thrds) th.join();

  // init priority queue, push all parts to it
  for (size_t j = 0; j < parts; j++) {
    if (partpos[j] == partsize[j]) continue;  // bucket already empty
    pq.push({&partbufs[j][partpos[j] % partsBufSize], j});
  }

  for (size_t i = 0; i < fsize; i += size) {
    auto top = pq.top();
    pq.pop();

    const void* smallest = top.first;
    ssize_t smallestP = top.second;

    // write the smallest element to the current buffer position
    memcpy(bufs[0] + (i % bufferSize), smallest, size);

    // if buffer is full (or if we are at the end of the file), flush
    if ((i % bufferSize) + size == bufferSize || i + size == fsize) {
      // write to output file
      ssize_t r = writeAll(newFile, bufs[0], i % bufferSize + size);
      if (r < 0) return -1;
    }

    // increment the position in the current smallest part by 1
    partpos[smallestP] += size;

    // we have reached the end of this part, do not re-add again
    if (partpos[smallestP] == partsize[smallestP]) continue;

    // we have reached the end of the parts buffer, re-fill
    if (partpos[smallestP] % partsBufSize == 0) {
      lseek(file, bufferSize * smallestP + partpos[smallestP], SEEK_SET);
      ssize_t r = readAll(file, partbufs[smallestP], partsBufSize);
      if (r < 0) return -1;
    }

    // re-add part with new smallest element to PQ
    pq.push(
        {&partbufs[smallestP][partpos[smallestP] % partsBufSize], smallestP});
  }

  // cleanup
  for (size_t j = 0; j < parts; j++) {
    delete[] partbufs[j];
  }
  for (size_t j = 0; j < numThreads; j++) {
    delete[] bufs[j];
  }
  delete[] partbufs;
  delete[] bufs;
  delete[] partpos;
  delete[] partsize;

  return 1;
}

// _____________________________________________________________________________
char* util::readableSize(double size, size_t n, char* buf) {
  int i = 0;
  const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB"};
  while (size > 1024 && i < 5) {
    size /= 1024;
    i++;
  }
  snprintf(buf, n, "%.*f %s", i, size, units[i]);
  return buf;
}

// _____________________________________________________________________________
std::string util::getTmpFName(std::string dir, std::string name,
                              std::string postf) {
  if (postf.size()) postf = "." + postf;
  if (dir == "<tmp>") dir = util::getTmpDir();
  if (dir.size() && dir.back() != '/') dir = dir + "/";

  std::string f = dir + name + postf + "." + std::to_string(getpid()) + "." +
                  randomString(15);

  size_t c = 0;

  while (access(f.c_str(), F_OK) != -1) {
    c++;
    if (c > 10000) {
      // giving up...
      std::cerr << "Could not create temporary file!" << std::endl;
      exit(1);
    }
    f = dir + name + postf + "." + std::to_string(getpid()) + "." +
        randomString(15);
  }

  return f;
}

/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */

/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */

// _____________________________________________________________________________
size_t util::getPeakRSS() {
#if defined(_WIN32)
  /* Windows -------------------------------------------------- */
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
  return (size_t)info.PeakWorkingSetSize;

#elif (defined(_AIX) || defined(__TOS__AIX__)) || \
    (defined(__sun__) || defined(__sun) ||        \
     defined(sun) && (defined(__SVR4) || defined(__svr4__)))
  /* AIX and Solaris ------------------------------------------ */
  struct psinfo psinfo;
  int fd = -1;
  if ((fd = open("/proc/self/psinfo", O_RDONLY)) == -1)
    return (size_t)0L; /* Can't open? */
  if (read(fd, &psinfo, sizeof(psinfo)) != sizeof(psinfo)) {
    close(fd);
    return (size_t)0L; /* Can't read? */
  }
  close(fd);
  return (size_t)(psinfo.pr_rssize * 1024L);

#elif defined(__unix__) || defined(__unix) || defined(unix) || \
    (defined(__APPLE__) && defined(__MACH__))
  /* BSD, Linux, and OSX -------------------------------------- */
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
#if defined(__APPLE__) && defined(__MACH__)
  return (size_t)rusage.ru_maxrss;
#else
  return (size_t)(rusage.ru_maxrss * 1024L);
#endif

#else
  /* Unknown OS ----------------------------------------------- */
  return (size_t)0L; /* Unsupported. */
#endif
}

/*
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */

// _____________________________________________________________________________
size_t util::getCurrentRSS() {
#if defined(_WIN32)
  /* Windows -------------------------------------------------- */
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
  return (size_t)info.WorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
  /* OSX ------------------------------------------------------ */
  struct mach_task_basic_info info;
  mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
  if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info,
                &infoCount) != KERN_SUCCESS)
    return (size_t)0L; /* Can't access? */
  return (size_t)info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || \
    defined(__gnu_linux__)
  /* Linux ---------------------------------------------------- */
  long rss = 0L;
  FILE* fp = NULL;
  if ((fp = fopen("/proc/self/statm", "r")) == NULL)
    return (size_t)0L; /* Can't open? */
  if (fscanf(fp, "%*s%ld", &rss) != 1) {
    fclose(fp);
    return (size_t)0L; /* Can't read? */
  }
  fclose(fp);
  return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);

#else
  /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
  return (size_t)0L; /* Unsupported. */
#endif
}

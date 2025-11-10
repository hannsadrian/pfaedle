// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef UTIL_JOBQUEUE_H_
#define UTIL_JOBQUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace util {

template <typename T>
class JobQueue {
 public:
  JobQueue() : _maxSize(std::numeric_limits<size_t>::max()) {}
  JobQueue(size_t maxSize) : _maxSize(maxSize) {}

  size_t size() {
    std::unique_lock<std::mutex> lock(_mut);
    return _jobs.size();
  }

  void reset() {
    _jobs = {};
  }

  void add(const T &job) {
    std::unique_lock<std::mutex> lockWait(_mut);
    _notFull.wait(lockWait, [this] { return _jobs.size() < _maxSize; });
      _jobs.push(job);
    _hasNew.notify_one();
  }

  void add(T &&job) {
    std::unique_lock<std::mutex> lockWait(_mut);
    _notFull.wait(lockWait, [this] { return _jobs.size() < _maxSize; });

      _jobs.push(std::move(job));
    _hasNew.notify_one();
  }

  T get() {
    T next;
    {
      std::unique_lock<std::mutex> lockWaitHave(_mut);
      // wait until a job arrives, but only block iff we do not have job
      _hasNew.wait(lockWaitHave, [this] { return !_jobs.empty(); });

      next = _jobs.front();
      if (next != T()) _jobs.pop();
    }

    // notify that we are not full anymore, but only if this is not the
    // DONE element
    if (next != T())
      _notFull.notify_one();
    else
      _hasNew.notify_one();
    return next;
  }

  JobQueue(const JobQueue &) = delete;
  JobQueue(JobQueue &&) = delete;
  JobQueue &operator=(const JobQueue &) = delete;
  JobQueue &operator=(JobQueue &&) = delete;

 private:
  std::mutex _mut;
  std::queue<T> _jobs;
  std::condition_variable _hasNew;
  std::condition_variable _notFull;

  size_t _maxSize;
};
}  // namespace util
#endif

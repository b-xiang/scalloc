// Copyright (c) 2014, the scalloc Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef SCALLOC_PROFILER_H_
#define SCALLOC_PROFILER_H_

#ifndef PROFILER

#define PROFILER_DECL 
#define PROFILER_SPANPOOL_PUT(sc)
#define PROFILER_SPANPOOL_GET(sc)

#else  // PROFILER

#include "log.h"

namespace scalloc {
  class Profiler;
  extern Profiler GlobalProfiler;
}

#define PROFILER_DECL scalloc::Profiler profiler_
#define PROFILER_SPANPOOL_PUT(sc) profiler_.SpanPoolPut()
#define PROFILER_SPANPOOL_GET(sc) profiler_.SpanPoolGet()

namespace scalloc {

class Profiler {
 public:
  static const uint64_t kMaxChildren = 32000;

  inline void Init(Profiler* parent);
  inline void Report();
  inline void SpanPoolPut();
  inline void SpanPoolGet();
  inline void Print();

 private:
  inline void Reset();
  inline void Update(Profiler* other);

  Profiler* parent_;
  uint64_t updates_;
  uint64_t span_pool_put_;
  uint64_t span_pool_get_;
};


void Profiler::Init(scalloc::Profiler* parent) {
  if (parent != NULL) {
    LOG_CAT("profiler", kTrace, "setting parent profiler %p", parent);
    parent_ = parent;
  }
  parent_ = parent; 
  Reset();
}


void Profiler::Reset() {
  span_pool_put_ = 0;
  span_pool_get_ = 0;
  updates_ = 0;
}


void Profiler::Update(Profiler* other) {
  LOG_CAT("profiler", kTrace, "updating profiler data");
  updates_++;
  span_pool_put_ += other->span_pool_put_;
  span_pool_get_ += other->span_pool_get_;
}


void Profiler::Report() {
  LOG_CAT("profiler", kTrace, "reporting profiler data");
  if (parent_ != NULL) {
    parent_->Update(this);
  }
}


void Profiler::SpanPoolPut() {
  span_pool_put_++;
}


void Profiler::SpanPoolGet() {
  span_pool_get_++;
}


void Profiler::Print() {
  printf(
      "{\n"
      "  'updates': %lu,\n"
      "  'span_pool_put': %lu,\n"
      "  'span_pool_get': %lu\n"
      "}\n",
      updates_,
      span_pool_put_,
      span_pool_get_
      );
}

}  // namespace scalloc

#endif  // PROFILER

#endif  // SCALLOC_PROFILER_H_


// Copyright (c) 2012-2013, the scalloc Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef SCALLOC_FREELIST_H_
#define SCALLOC_FREELIST_H_

#ifdef HAVE_CONFIG_H_
#include "config.h"
#endif  // HAVE_CONFIG_H_

#include <cstdint>

#include "common.h"
#include "log.h"

// An unlocked freelist.
class Freelist {
 public:
  void FromBlock(const void* start, const size_t size, size_t len);
  void AddRange(const void* start, const size_t size, size_t len);
  void Push(void* p);
  void* Pop();
  bool Empty();
  size_t Size();

 private:
  size_t len_;
  void* list_;
#ifdef FREELIST_CHECK_BOUNDS
  uintptr_t lower_;
  uintptr_t upper_;
#endif  // FREELIST_CHECK_BOUNDS
};

inline void Freelist::FromBlock(const void* start, const size_t size, size_t len) {
  len_ = 0;
  list_ = NULL;
  uintptr_t start_ptr = reinterpret_cast<uintptr_t>(start);
#ifdef FREELIST_CHECK_BOUNDS
  lower_ = start_ptr;
  upper_ = start_ptr + size *len;
#endif  // FREELIST_CHECK_BOUNDS
  for (; len > 0; len--) {
    Push(reinterpret_cast<void*>(start_ptr));
    start_ptr += size;
  }
}

inline void Freelist::AddRange(const void* start, const size_t size, size_t len) {
  uintptr_t start_ptr = reinterpret_cast<uintptr_t>(start);
#ifdef FREELIST_CHECK_BOUNDS
  if (start_ptr < lower_) {
    lower_ = start_ptr;
  }
  if ((start_ptr + size * len) > upper_) {
    upper_ = start_ptr + size * len;
  }
#endif  // FREELIST_CHECK_BOUNDS
  for (; len > 0; len--) {
    Push(reinterpret_cast<void*>(start_ptr));
    start_ptr += size;
  }
}

always_inline bool Freelist::Empty() {
  return list_ == NULL;
}

always_inline size_t Freelist::Size() {
  return len_;
}

always_inline void Freelist::Push(void* p) {
#ifdef FREELIST_CHECK_BOUNDS
  if (!((reinterpret_cast<uintptr_t>(p) >= lower_) &&
        (reinterpret_cast<uintptr_t>(p) < upper_))) {
    ErrorOut("Freelist::Push() out of bounds");
  }
#endif  // FREELIST_CHECK_BOUNDS
  *(reinterpret_cast<void**>(p)) = list_;
  list_ = p;
  len_++;
}

always_inline void* Freelist::Pop() {
  void* result = list_;
  if (result != NULL) {
    list_ = *(reinterpret_cast<void**>(list_));
    len_--;
  }
#ifdef FREELIST_CHECK_BOUNDS
  if (result) {
    if (!((reinterpret_cast<uintptr_t>(result) >= lower_) &&
          (reinterpret_cast<uintptr_t>(result) < upper_))) {
      ErrorOut("Freelist::Push() out of bounds");
    }
  }
#endif  // FREELIST_CHECK_BOUNDS
  return result;
}

#endif  // SCALLOC_FREELIST_H_

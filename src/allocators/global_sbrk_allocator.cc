// Copyright (c) 2012-2013, the scalloc Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "allocators/global_sbrk_allocator.h"

void GlobalSbrkAllocator::Init(size_t size) {
  size_ = size;
  size = size * 2 - RuntimeVars::SystemPageSize();
  uintptr_t p = reinterpret_cast<uintptr_t>(mmap(
      0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
      -1, 0));
  if (reinterpret_cast<void*>(p) == MAP_FAILED) {
    ErrorOut("[GlobalSbrkAllocator] mmap failed. errno: %lu", errno);
  }
  p += p % size_;
  current_ = p;
}

#pragma once

#include <cursed-tea/core.h>
#include <stddef.h>

struct CursedViewport {
  struct CtModel base;
  struct CtModel *child;
  size_t _offset;
  size_t _max_offset;
};

void ctu_viewport_setup(struct CursedViewport *model,
                           struct CtModel *child);

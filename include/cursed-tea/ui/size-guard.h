#pragma once

#include <cursed-tea/core.h>
#include <stdbool.h>
#include <stddef.h>

struct CursedSizeGuard {
  struct CtModel base;
  struct CtModel *child;
  size_t min_x, min_y;
  bool _ok;
};

void ctu_size_guard_setup(struct CursedSizeGuard *model,
                             const size_t min_x, const size_t min_y,
                             struct CtModel *child);

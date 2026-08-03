#pragma once

#include <cursed-tea/core.h>
#include <stddef.h>
#include <wchar.h>

struct CursedScrollableArea {
  struct CtModel base;
  wchar_t *_text;
  size_t _offset, _max_offset;
};

void ctu_scrollable_area_setup(struct CursedScrollableArea *model,
                                  const wchar_t *text);

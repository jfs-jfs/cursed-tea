#pragma once

#include <cursed-tea/core.h>
#include <cursed-tea/ui/label.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

struct CursedToggle {
  struct CtModel base;
  struct CursedLabel label;
  size_t label_size;
  bool focus;
  bool selected;
  int signal_on_change;
};

void ctu_toggle_setup(struct CursedToggle *model, const wchar_t *text,
                         bool selected, int signal_on_change,
                         const size_t label_size);

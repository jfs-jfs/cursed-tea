#pragma once

#include <cursed-tea/core.h>
#include <stdbool.h>
#include <stddef.h>

enum Throbbers {
  T_LINE,
  T_DOT,
  T_MINIDOT,
  T_JUMP,
  T_PULSE,
};

struct CursedThrobber {
  struct CtModel base;
  enum Throbbers style;
  size_t _current_step;
  bool _started;
  enum Throbbers _previous_style;
};

void ctu_throbber_setup(struct CursedThrobber *model,
                           enum Throbbers style);

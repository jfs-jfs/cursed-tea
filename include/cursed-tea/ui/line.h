#pragma once

#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>

struct CursedLine {
  struct CtModel base;
  enum Orientation orientation;
  enum CtBorderStyles type;
};

void ctu_line_setup(struct CursedLine *model, enum CtBorderStyles style,
                       enum Orientation orientation);

#pragma once

#include <cursed-tea/uvector.h>
#include <stddef.h>

struct CtStyledZone {
  CtUVector start;
  CtUVector end;
  struct {
    size_t r, g, b;
  } background;
  struct {
    size_t r, g, b;
  } foreground;
};

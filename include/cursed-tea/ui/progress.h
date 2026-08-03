#pragma once

#include <cursed-tea/core.h>
#include <stdbool.h>
#include <stdint.h>

enum ProgressStyle {
  P_CLASSIC,
  P_BLOCK,
  P_PARTIAL,
  P_SOLID,
  P_GRADIENT,
};

enum ProgressPercent {
  PP_NONE,
  PP_OVERLAY,
  PP_TRAIL,
};

struct CursedProgress {
  struct CtModel base;
  uint8_t value;
  enum ProgressStyle style;
  enum ProgressPercent percent;
  bool border;
  struct CtRGB fill_color;
  struct CtRGB gradient_from;
  struct CtRGB gradient_to;
};

void ctu_progress_setup_classic(struct CursedProgress *model,
                                   uint8_t value, enum ProgressPercent percent);

void ctu_progress_setup_block(struct CursedProgress *model,
                                 uint8_t value, enum ProgressPercent percent);

void ctu_progress_setup_partial(struct CursedProgress *model,
                                   uint8_t value, enum ProgressPercent percent);

void ctu_progress_setup_solid(struct CursedProgress *model,
                                 uint8_t value, enum ProgressPercent percent,
                                 struct CtRGB fill_color);

void ctu_progress_setup_gradient(struct CursedProgress *model,
                                    uint8_t value, enum ProgressPercent percent,
                                    struct CtRGB gradient_from,
                                    struct CtRGB gradient_to);

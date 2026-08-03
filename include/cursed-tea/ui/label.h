#pragma once

#include <cursed-tea/core.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#define CTU_LABEL_MAX_SIZE 256

struct CursedLabel {
  struct CtModel base;
  bool border;
  bool center;
  size_t padding_x;
  size_t padding_y;
  wchar_t text[CTU_LABEL_MAX_SIZE + 1];
};

void ctu_label_setup(struct CursedLabel *model, const wchar_t *text,
                        bool border, bool center, size_t padding_x,
                        size_t padding_y);

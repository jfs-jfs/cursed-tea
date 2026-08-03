#pragma once

#include <cursed-tea/core.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#define CTU_INPUT_MAX 512

struct CursedInput {
  struct CtModel base;
  attr_t highlight;
  bool focus;
  bool secret;
  bool border;
  bool centered;
  bool centered_text;
  size_t max_size;
  int signal_on_submit;
  wchar_t buffer[CTU_INPUT_MAX + 1];
  bool _draw_caret;
};

void ctu_input_setup(struct CursedInput *out, attr_t highlight,
                        bool secret, bool border, bool centered,
                        bool centered_text, size_t max_size,
                        int signal_on_submit);

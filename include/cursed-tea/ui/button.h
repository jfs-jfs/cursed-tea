#pragma once

#include <cursed-tea/core.h>
#include <ncurses.h>
#include <stdbool.h>
#include <wchar.h>

#define MAX_TEXT_LENGTH 128

struct CursedButton {
  struct CtModel base;
  int signal_on_pressed;
  attr_t highlight;
  bool has_focus;
  bool centered;
  bool border;
  wchar_t text[MAX_TEXT_LENGTH + 1];
};

void ctu_button_static_init(const wchar_t *text, bool center_text,
                               bool border, attr_t highlight,
                               int signal_on_pressed,
                               struct CursedButton *button_model);

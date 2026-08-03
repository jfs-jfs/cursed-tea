#pragma once

#include <cursed-tea/core.h>
#include <stddef.h>
#include <wchar.h>

struct CursedMenu {
  struct CtModel base;
  wchar_t **options;
  wchar_t **options_shortcurt;
  size_t options_count;
  int signal_on_select;
};

void ctu_menu_setup(struct CursedMenu *model, const wchar_t **options,
                       const wchar_t **options_shortcurt, size_t options_count);

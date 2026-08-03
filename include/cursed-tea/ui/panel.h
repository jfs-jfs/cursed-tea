#pragma once

#include <cursed-tea/core.h>
#include <cursed-tea/ui/label.h>
#include <wchar.h>

struct CursedPanel {
  struct CtModel base;
  struct CtModel *child;
  struct CursedLabel title;
  enum CtBorderStyles border;
  bool draw_title;
};

void ctu_panel_setup(struct CursedPanel *model,
                        const enum CtBorderStyles border, const wchar_t *title,
                        struct CtModel *child);

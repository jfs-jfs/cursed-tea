#pragma once

#include <cursed-tea/canvas.h>

#include <wchar.h>

#define NUMBER_BORDER_STYLES 1
enum CtBorderStyles {
  B_NONE = -1,
  B_NORMAL,
  B_DOUBLE,
  B_CURVED,
  B_BLOCK,
  B_HALFBLOCK_OUTER,
  B_HALFBLOCK_INNER,
  B_THICK,
  B_ASCII,
  B_MODERN,
};

struct CtBorderStyle {
  wchar_t top;
  wchar_t bottom;
  wchar_t left;
  wchar_t right;
  wchar_t top_left;
  wchar_t top_right;
  wchar_t bottom_left;
  wchar_t bottom_right;
};

// after_border set to null if not wanted
void ct_cborder(struct CtCanvas *canvas, struct CtCanvas *after_border);
void ct_cborder_custom(struct CtCanvas *canvas, struct CtCanvas *after_border,
                       const wchar_t top, const wchar_t bottom,
                       const wchar_t left, const wchar_t right,
                       const wchar_t top_left, const wchar_t top_right,
                       const wchar_t bottom_left, const wchar_t bottom_right);

struct CtBorderStyle ct_border_from(const enum CtBorderStyles style);

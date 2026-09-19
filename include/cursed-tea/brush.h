#pragma once

#include <cursed-tea/border.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define DEFAULT_BACKGROUND (struct CtRGB){0, 0, 0}
#define DEFAULT_FOREGROUND (struct CtRGB){255, 255, 255}

struct CtRGB {
  uint8_t r, g, b;
};

struct CtBrush {
  struct CtRGB foreground;
  struct CtRGB background;
  attr_t attribute;
  enum CtBorderStyles border_style;
};

void ct_brush_setup();

void ct_brush_fg(const uint8_t r, const uint8_t g, const uint8_t b);
void ct_brush_bg(const uint8_t r, const uint8_t g, const uint8_t b);
void ct_brush_fg_hex(const char *hex);
void ct_brush_bg_hex(const char *hex);
void ct_brush_swap_colors();
void ct_brush_attr(const attr_t attribute);
void ct_brush_add_attr(const attr_t attribute);
void ct_brush_border(const enum CtBorderStyles border_style);
void ct_brush_clean();
void ct_brush_from(const struct CtBrush other);

struct CtBrush ct_brush();

bool ct_brush_has_color(void);
bool ct_brush_has_truecolor(void);
int ct_brush_colors(void);
int ct_brush_color_pairs(void);

short ct_brush_current_color_pair();

void ct_brush_cell(cchar_t *cell, const wchar_t character);

cchar_t ct_brush_default_cell();

// TODO: move helper to a common file?
bool parse_hex_color(const char *hex, struct CtRGB *result);

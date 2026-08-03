#pragma once

#include <ncurses.h>
#include <stddef.h>
#include <wchar.h>

#define MAX_Y_SCREEN 1024

struct CtCanvas {
  size_t max_x, max_y;
  size_t size;
  cchar_t *screen_cells;
  cchar_t *cell_matrix[MAX_Y_SCREEN];
};

void ct_main_canvas_init(const size_t x, const size_t y);
struct CtCanvas *ct_main_canvas();
void ct_main_canvas_clear();
void ct_main_canvas_resize(const size_t x, const size_t y);

// Parallel canvas will never be brushed into screen, useful when you need to
// render the child of a component first, extract information and modify it's
// output. It always has max screen size
struct CtCanvas *ct_parallel_canvas();
void ct_parallel_canvas_clear();
void ct_parallel_of_width(struct CtCanvas *out, const size_t width);

// Assumes content start at 0,0 and first row with all default cells is height
size_t ct_canvas_content_height(const struct CtCanvas *canvas);

void ct_canvas_copy(const struct CtCanvas *from, struct CtCanvas *to);

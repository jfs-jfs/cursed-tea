#include <cursed-tea/canvas.h>
#include <cursed-tea/layout.h>
#include <cursed-tea/logger.h>
#include <cursed-tea/style/brush.h>
#include <assert.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

static struct CtCanvas MAIN_CANVAS = {0, 0, 0, NULL, {NULL}};
static struct CtCanvas PARALLEL_CANVAS = {0, 0, 0, NULL, {NULL}};

void _canvases_cleanup();
void _canvas_init(struct CtCanvas *canvas, const size_t x, const size_t y);

static void _fill_blank(struct CtCanvas *c) {
  cchar_t default_cell = ct_brush_default_cell();
  for (size_t i = 0; i < c->size; i++)
    c->screen_cells[i] = default_cell;
}

void ct_main_canvas_init(const size_t x, const size_t y) {
  _canvas_init(&MAIN_CANVAS, x, y);
  _canvas_init(&PARALLEL_CANVAS, x, MAX_Y_SCREEN);
  atexit(_canvases_cleanup);
}

struct CtCanvas *ct_main_canvas() { return &MAIN_CANVAS; }

struct CtCanvas *ct_parallel_canvas() { return &PARALLEL_CANVAS; }

void ct_main_canvas_clear() { _fill_blank(&MAIN_CANVAS); }

void ct_parallel_canvas_clear() { _fill_blank(&PARALLEL_CANVAS); }

void ct_main_canvas_resize(const size_t x, const size_t y) {
  _canvases_cleanup();
  _canvas_init(&MAIN_CANVAS, x, y);
  _canvas_init(&PARALLEL_CANVAS, x, MAX_Y_SCREEN);
}

void _canvases_cleanup() {
  free(MAIN_CANVAS.screen_cells);
  free(PARALLEL_CANVAS.screen_cells);
  log_debug(L"Canvas freed");
}

void _canvas_init(struct CtCanvas *canvas, const size_t x, const size_t y) {
  canvas->max_x = x;
  canvas->max_y = y;
  canvas->size = x * y;
  canvas->screen_cells = malloc((canvas->size + 1) * sizeof(cchar_t));

  log_fmt(LOG_DEBUG, L"max_x=%zu", canvas->max_x);
  log_fmt(LOG_DEBUG, L"max_y=%zu", canvas->max_y);
  if (NULL == canvas->screen_cells) {
    log_fmt(LOG_DEBUG, L"max_x=%zu", canvas->max_x);
    log_fmt(LOG_DEBUG, L"max_y=%zu", canvas->max_y);
    log_crit(L"Unable to allocate space for screen_cells");
  }

  log_debug(L"Canvas allocated");

  _fill_blank(canvas);

  for (size_t i = 0; i < y; i++)
    canvas->cell_matrix[i] = canvas->screen_cells + (i * x);
}

void ct_canvas_copy(const struct CtCanvas *from, struct CtCanvas *to) {
  to->max_x = from->max_x;
  to->max_y = from->max_y;
  to->size = from->size;
  to->screen_cells = from->screen_cells;

  for (size_t i = 0; i < from->max_y; i++) {
    to->cell_matrix[i] = from->cell_matrix[i];
  }
}

void ct_parallel_of_width(struct CtCanvas *out, const size_t width) {
  assert(width <= PARALLEL_CANVAS.max_x);
  ct_ccutout(&PARALLEL_CANVAS, out, 0, 0, width, MAX_Y_SCREEN);
}

size_t ct_canvas_content_height(const struct CtCanvas *canvas) {

  size_t height;
  cchar_t empty_cell = ct_brush_default_cell();

  for (height = 0; height < canvas->max_y; height++) {
    bool empty_line = true;
    size_t j = 0;
    while (empty_line && j < canvas->max_x) {
      empty_line =
          wcscmp(canvas->cell_matrix[height][j++].chars, empty_cell.chars) == 0;
    }

    if (empty_line)
      break;
  }
  return height;
}

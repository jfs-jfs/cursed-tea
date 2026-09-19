#include <cursed-tea/brush.h>
#include <cursed-tea/canvas-write.h>
#include <cursed-tea/logger.h>
#include <cursed-tea/uvector.h>
#include <stddef.h>

void ct_cwrite_char(struct CtCanvas *canvas, const size_t x, const size_t y,
                    const wchar_t ch) {
  CtUVector point = {x, y};
  CtUVector canvas_area = {canvas->max_x, canvas->max_y};
  if (!ct_uvec_is_inside(canvas_area, point)) {
    log_error(L"Trying to write character outside canvas");
    return;
  }
  ct_brush_cell(&canvas->cell_matrix[y][x], ch);
}

void _ct_cwrite_char_rec(struct CtCanvas *canvas, const CtUVector from,
                         const CtUVector to, const wchar_t character) {
  ct_brush_cell(&canvas->cell_matrix[from.y][from.x], character);
  if (ct_uvec_same_as(from, to)) {
    return;
  }

  size_t smallest_idx = 0;
  size_t smallest_distance = -1;
  float distances[] = {.0, .0, .0, .0};
  CtUVector directions[] = {
      {from.x + 1, from.y},
      {from.x, from.y + 1},
      {from.x - 1, from.y},
      {from.x, from.y - 1},
  };

  for (size_t i = 0; i < 4; i++)
    distances[i] = ct_uvec_distance(directions[i], to);

  for (size_t i = 0; i < 4; i++)
    if (distances[i] < smallest_distance) {
      smallest_distance = distances[i];
      smallest_idx = i;
    }

  _ct_cwrite_char_rec(canvas, directions[smallest_idx], to, character);
}

void ct_cwrite_char_line(struct CtCanvas *canvas, const size_t from_x,
                         const size_t from_y, const size_t to_x,
                         const size_t to_y, const wchar_t ch) {

  CtUVector start = {from_x, from_y};
  CtUVector end = {to_x, to_y};
  CtUVector canvas_area = {canvas->max_x, canvas->max_y};

  if (!ct_uvec_is_inside(canvas_area, start) ||
      !ct_uvec_is_inside(canvas_area, end)) {
    log_error(L"Trying to write outside of canvas!");
    return;
  }

  _ct_cwrite_char_rec(canvas, start, end, ch);
}

void ct_cfill(struct CtCanvas *canvas, const wchar_t character) {
  for (size_t i = 0; i < canvas->max_y; i++)
    ct_cwrite_char_line(canvas, 0, i, canvas->max_x - 1, i, character);
}

void ct_cwrite(struct CtCanvas *canvas, const size_t x, const size_t y,
               const wchar_t *text) {
  size_t text_length = wcslen(text);
  CtUVector start = {x, y};
  CtUVector end = {x + text_length, y};
  CtUVector canvas_area = {canvas->max_x, canvas->max_y};

  // start is a cell; end.x is exclusive (end.y is the same cell row).
  if (!ct_uvec_is_inside(canvas_area, start) ||
      !ct_uvec_fits(canvas_area, end)) {
    log_error(L"Trying to write outside of canvas!");
    return;
  }

  for (size_t i = 0; i < text_length; i++)
    ct_brush_cell(&canvas->cell_matrix[y][x + i], text[i]);
}

void ct_cwritev(struct CtCanvas *canvas, const size_t x, const size_t y,
                const wchar_t *text) {
  size_t text_length = wcslen(text);
  CtUVector start = {x, y};
  CtUVector end = {x, y + text_length};
  CtUVector canvas_area = {canvas->max_x, canvas->max_y};

  // start is a cell; end.y is exclusive (end.x is the same cell column).
  if (!ct_uvec_is_inside(canvas_area, start) ||
      !ct_uvec_fits(canvas_area, end)) {
    log_error(L"Trying to write outside of canvas!");
    return;
  }

  for (size_t i = 0; i < text_length; i++)
    ct_brush_cell(&canvas->cell_matrix[y + i][x], text[i]);
}

// Horizontal center
//
void ct_cwrite_c(struct CtCanvas *canvas, const size_t y, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = (canvas->max_x - text_length) / 2;
  ct_cwrite(canvas, x, y, text);
}

void ct_cwrite_cs(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = (canvas->max_x - text_length) / 2;
  ct_cwrite(canvas, x, 0, text);
}

void ct_cwrite_cc(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = (canvas->max_x - text_length) / 2;
  ct_cwrite(canvas, x, canvas->max_y / 2, text);
}

void ct_cwrite_ce(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = (canvas->max_x - text_length) / 2;
  ct_cwrite(canvas, x, canvas->max_y - 1, text);
}

// Horizontal start
//
void ct_cwrite_s(struct CtCanvas *canvas, const size_t y, const wchar_t *text) {
  ct_cwrite(canvas, 0, y, text);
}

void ct_cwrite_ss(struct CtCanvas *canvas, const wchar_t *text) {
  ct_cwrite(canvas, 0, 0, text);
}

void ct_cwrite_sc(struct CtCanvas *canvas, const wchar_t *text) {
  ct_cwrite(canvas, 0, canvas->max_y / 2, text);
}

void ct_cwrite_se(struct CtCanvas *canvas, const wchar_t *text) {
  ct_cwrite(canvas, 0, canvas->max_y - 1, text);
}

// Horizontal end
//
void ct_cwrite_e(struct CtCanvas *canvas, const size_t y, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = canvas->max_x - text_length;
  ct_cwrite(canvas, x, y, text);
}

void ct_cwrite_es(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = canvas->max_x - text_length;
  ct_cwrite(canvas, x, 0, text);
}

void ct_cwrite_ec(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = canvas->max_x - text_length;
  ct_cwrite(canvas, x, canvas->max_y / 2, text);
}

void ct_cwrite_ee(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t x = canvas->max_x - text_length;
  ct_cwrite(canvas, x, canvas->max_y - 1, text);
}

// Vertical center
//
void ct_cwritev_c(struct CtCanvas *canvas, const size_t x,
                  const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = (canvas->max_y - text_length) / 2;
  ct_cwritev(canvas, x, y, text);
}

void ct_cwritev_cs(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = (canvas->max_y - text_length) / 2;
  ct_cwritev(canvas, 0, y, text);
}

void ct_cwritev_cc(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = (canvas->max_y - text_length) / 2;
  ct_cwritev(canvas, canvas->max_x / 2, y, text);
}

void ct_cwritev_ce(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = (canvas->max_y - text_length) / 2;
  ct_cwritev(canvas, canvas->max_x - 1, y, text);
}

// Vertical start
//
void ct_cwritev_s(struct CtCanvas *canvas, const size_t x,
                  const wchar_t *text) {
  ct_cwritev(canvas, x, 0, text);
}

void ct_cwritev_ss(struct CtCanvas *canvas, const wchar_t *text) {
  ct_cwritev(canvas, 0, 0, text);
}

void ct_cwritev_sc(struct CtCanvas *canvas, const wchar_t *text) {
  ct_cwritev(canvas, canvas->max_x / 2, 0, text);
}

void ct_cwritev_se(struct CtCanvas *canvas, const wchar_t *text) {
  ct_cwritev(canvas, canvas->max_x - 1, 0, text);
}

// Vertical end
//
void ct_cwritev_e(struct CtCanvas *canvas, const size_t x,
                  const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = canvas->max_y - text_length;
  ct_cwritev(canvas, x, y, text);
}

void ct_cwritev_es(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = canvas->max_y - text_length;
  ct_cwritev(canvas, 0, y, text);
}

void ct_cwritev_ec(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = canvas->max_y - text_length;
  ct_cwritev(canvas, canvas->max_x / 2, y, text);
}

void ct_cwritev_ee(struct CtCanvas *canvas, const wchar_t *text) {
  size_t text_length = wcslen(text);
  size_t y = canvas->max_y - text_length;
  ct_cwritev(canvas, canvas->max_x - 1, y, text);
}

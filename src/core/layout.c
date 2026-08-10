#include <assert.h>
#include <cursed-tea/canvas-write.h>
#include <cursed-tea/canvas.h>
#include <cursed-tea/layout.h>
#include <cursed-tea/logger.h>
#include <cursed-tea/uvector.h>
#include <math.h>
#include <stddef.h>
#include <wchar.h>

void ct_cvsplit(const struct CtCanvas *canvas, struct CtCanvas *left,
                struct CtCanvas *right) {
  assert(left != NULL);
  assert(right != NULL);

  size_t left_size = canvas->max_x / 2;
  size_t right_size = canvas->max_x - left_size;

  left->max_x = left_size;
  left->max_y = canvas->max_y;
  left->size = left->max_y * left->max_x;
  left->screen_cells = NULL;

  for (size_t i = 0; i < left->max_y; i++)
    left->cell_matrix[i] = canvas->cell_matrix[i];

  right->max_x = right_size;
  right->max_y = canvas->max_y;
  right->size = right->max_y * right->max_x;
  right->screen_cells = NULL;

  for (size_t i = 0; i < right->max_y; i++)
    right->cell_matrix[i] = canvas->cell_matrix[i] + left_size;
}

void ct_chsplit(const struct CtCanvas *canvas, struct CtCanvas *top,
                struct CtCanvas *bottom) {

  assert(top != NULL);
  assert(bottom != NULL);

  size_t top_size = canvas->max_y / 2;
  size_t bottom_size = canvas->max_y - top_size;

  top->max_x = canvas->max_x;
  top->max_y = top_size;
  top->size = top->max_y * top->max_x;
  top->screen_cells = NULL;

  for (size_t i = 0; i < top->max_y; i++)
    top->cell_matrix[i] = canvas->cell_matrix[i];

  bottom->max_x = canvas->max_x;
  bottom->max_y = bottom_size;
  bottom->size = bottom->max_y * bottom->max_x;
  bottom->screen_cells = NULL;

  for (size_t i = 0; i < bottom->max_y; i++)
    bottom->cell_matrix[i] = canvas->cell_matrix[i + top_size];
}

void ct_ccutout(const struct CtCanvas *canvas, struct CtCanvas *cutout,
                const size_t from_x, const size_t from_y, const size_t to_x,
                const size_t to_y) {
  assert(NULL != cutout);

  CtUVector start = {from_x, from_y};
  CtUVector end = {to_x, to_y};
  const CtUVector canvas_area = {canvas->max_x, canvas->max_y};

  // start is inclusive cell; end is exclusive far corner.
  if (!ct_uvec_is_inside(canvas_area, start) ||
      !ct_uvec_fits(canvas_area, end)) {
    log_error(L"Cutout out of bounds");
    return;
  }

  if (ct_uvec_same_as(start, end)) {
    log_error(L"Can not make a cutout out of point");
    return;
  }

  if (start.x == end.x || end.y == start.y) {
    log_error(L"Can not make a cutout out of a line");
    return;
  }

  if (start.x > end.x || start.y > end.y) {
    log_error(L"Can not make a cutout out of given coordinates");
    return;
  }

  cutout->max_x = end.x - start.x;
  cutout->max_y = end.y - start.y;
  cutout->screen_cells = NULL;
  cutout->size = cutout->max_x * cutout->max_y;

  for (size_t i = 0; i < cutout->max_y; i++)
    cutout->cell_matrix[i] = canvas->cell_matrix[i + start.y] + start.x;
}

void ct_ccutout_c(const struct CtCanvas *canvas, struct CtCanvas *cutout,
                  const size_t x, const size_t y) {
  assert(NULL != cutout);

  size_t from_x, from_y, to_x, to_y;
  from_x = (canvas->max_x - x) / 2;
  to_x = from_x + x;
  from_y = (canvas->max_y - y) / 2;
  to_y = from_y + y;
  ct_ccutout(canvas, cutout, from_x, from_y, to_x, to_y);
}

void ct_cvwsplit(const struct CtCanvas *canvas, struct CtCanvas *left,
                 struct CtCanvas *right, const size_t weight_left,
                 const size_t weight_right) {
  assert(NULL != left);
  assert(NULL != right);
  assert(0 != weight_left && 0 != weight_right);

  const size_t total = weight_left + weight_right;
  size_t left_size = floorf((canvas->max_x / (float)total) * weight_left);
  size_t right_size = canvas->max_x - left_size;

  left->max_x = left_size;
  left->max_y = canvas->max_y;
  left->size = left->max_y * left->max_x;
  left->screen_cells = NULL;

  for (size_t i = 0; i < left->max_y; i++)
    left->cell_matrix[i] = canvas->cell_matrix[i];

  right->max_x = right_size;
  right->max_y = canvas->max_y;
  right->size = right->max_y * right->max_x;
  right->screen_cells = NULL;

  for (size_t i = 0; i < right->max_y; i++)
    right->cell_matrix[i] = canvas->cell_matrix[i] + left_size;
}

void ct_chwsplit(const struct CtCanvas *canvas, struct CtCanvas *top,
                 struct CtCanvas *bottom, const size_t weight_top,
                 const size_t weight_bottom) {
  assert(NULL != top);
  assert(NULL != bottom);
  assert(0 != weight_top && 0 != weight_bottom);

  const size_t total = weight_bottom + weight_top;
  size_t top_size = floorf((canvas->max_y / (float)total) * weight_top);
  size_t bottom_size = canvas->max_y - top_size;

  top->max_x = canvas->max_x;
  top->max_y = top_size;
  top->size = top->max_y * top->max_x;
  top->screen_cells = NULL;

  for (size_t i = 0; i < top->max_y; i++)
    top->cell_matrix[i] = canvas->cell_matrix[i];

  bottom->max_x = canvas->max_x;
  bottom->max_y = bottom_size;
  bottom->size = bottom->max_y * bottom->max_x;
  bottom->screen_cells = NULL;

  for (size_t i = 0; i < bottom->max_y; i++)
    bottom->cell_matrix[i] = canvas->cell_matrix[i + top_size];
}

void ct_cmargin(const struct CtCanvas *canvas, struct CtCanvas *out,
                const size_t top, const size_t right, const size_t bottom,
                const size_t left) {

  if (canvas->max_x <= right + left || canvas->max_y <= top + bottom) {
    log_error(
        L"there is not enough space to cut out margin, returning original");
    ct_canvas_copy(canvas, out);
    return;
  }

  out->max_x = canvas->max_x - right - left;
  out->max_y = canvas->max_y - top - bottom;
  out->size = out->max_x * out->max_y;
  out->screen_cells = NULL;

  for (size_t i = 0; i < out->max_y; i++)
    out->cell_matrix[i] = canvas->cell_matrix[i + top] + left;
}

void ct_cmargin_x(const struct CtCanvas *canvas, struct CtCanvas *out,
                  const size_t right, const size_t left) {
  ct_cmargin(canvas, out, 0, right, 0, left);
}

void ct_cmargin_y(const struct CtCanvas *canvas, struct CtCanvas *out,
                  const size_t top, const size_t bottom) {
  ct_cmargin(canvas, out, top, 0, bottom, 0);
}

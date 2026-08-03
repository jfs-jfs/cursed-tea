#include <cursed-tea/ui/progress.h>
#include <cursed-tea/ui/common.h>
#include <stdio.h>
#include <wchar.h>

static uint8_t ctu_progress_clamp(uint8_t value) {
  return value > 100 ? 100 : value;
}

static float ctu_progress_ratio(uint8_t value) {
  return (float)ctu_progress_clamp(value) / 100.0f;
}

static uint8_t ctu_progress_lerp_channel(uint8_t start, uint8_t end,
                                            float blend) {
  return (uint8_t)((float)start + ((float)end - (float)start) * blend);
}

static struct CtRGB ctu_progress_lerp_rgb(struct CtRGB start, struct CtRGB end,
                                           float blend) {
  if (blend < 0.0f)
    blend = 0.0f;
  if (blend > 1.0f)
    blend = 1.0f;
  return (struct CtRGB){
      ctu_progress_lerp_channel(start.r, end.r, blend),
      ctu_progress_lerp_channel(start.g, end.g, blend),
      ctu_progress_lerp_channel(start.b, end.b, blend),
  };
}

static void ctu_progress_contrast_foreground(struct CtRGB background) {
  unsigned luminance = (unsigned)background.r * 3u +
                       (unsigned)background.g * 6u +
                       (unsigned)background.b * 1u;
  if (luminance > 127u * 10u)
    ct_brush_fg(0, 0, 0);
  else
    ct_brush_fg(255, 255, 255);
}

static wchar_t ctu_progress_partial_glyph(float fraction) {
  static const wchar_t partial_glyphs[] = L" ▏▎▍▌▋▊▉█";
  if (fraction <= 0.0f)
    return partial_glyphs[0];
  if (fraction >= 1.0f)
    return partial_glyphs[8];
  size_t glyph_index = (size_t)(fraction * 8.0f);
  if (glyph_index > 8)
    glyph_index = 8;
  return partial_glyphs[glyph_index];
}

static void ctu_progress_format_percent(wchar_t *buffer,
                                           size_t buffer_length,
                                           uint8_t value) {
  swprintf(buffer, buffer_length, L"%u%%",
           (unsigned)ctu_progress_clamp(value));
}

static void ctu_progress_draw_classic(struct CtCanvas *bar_canvas,
                                         uint8_t value) {
  size_t width = bar_canvas->max_x;
  size_t center_row = bar_canvas->max_y / 2;
  if (width < 2)
    return;

  ct_cwrite_char(bar_canvas, 0, center_row, L'[');
  ct_cwrite_char(bar_canvas, width - 1, center_row, L']');

  size_t inner_width = width - 2;
  size_t filled_columns = ((size_t)value * inner_width + 50) / 100;
  if (filled_columns > inner_width)
    filled_columns = inner_width;

  for (size_t column = 0; column < inner_width; column++) {
    wchar_t character = (column < filled_columns) ? L'#' : L'.';
    ct_cwrite_char(bar_canvas, column + 1, center_row, character);
  }
}

static void ctu_progress_draw_block(struct CtCanvas *bar_canvas, uint8_t value,
                                       bool use_partial) {
  size_t width = bar_canvas->max_x;
  size_t center_row = bar_canvas->max_y / 2;
  if (width == 0)
    return;

  float filled_cells = ctu_progress_ratio(value) * (float)width;
  size_t full_columns = (size_t)filled_cells;
  if (full_columns > width)
    full_columns = width;
  float partial_fraction = filled_cells - (float)full_columns;

  for (size_t column = 0; column < width; column++) {
    wchar_t character;
    if (column < full_columns)
      character = L'█';
    else if (use_partial && column == full_columns && partial_fraction > 0.0f)
      character = ctu_progress_partial_glyph(partial_fraction);
    else
      character = L'░';
    ct_cwrite_char(bar_canvas, column, center_row, character);
  }
}

static void ctu_progress_draw_solid(struct CtCanvas *bar_canvas, uint8_t value,
                                       struct CtRGB fill_color) {
  size_t width = bar_canvas->max_x;
  size_t center_row = bar_canvas->max_y / 2;
  if (width == 0 || bar_canvas->max_y == 0)
    return;

  size_t filled_columns = ((size_t)value * width + 50) / 100;
  if (filled_columns > width)
    filled_columns = width;

  for (size_t column = 0; column < width; column++) {
    if (column < filled_columns) {
      ct_brush_fg(fill_color.r, fill_color.g, fill_color.b);
      ct_brush_bg(fill_color.r, fill_color.g, fill_color.b);
    } else {
      ct_brush_fg(60, 60, 70);
      ct_brush_bg(30, 30, 36);
    }
    ct_cwrite_char(bar_canvas, column, center_row, L' ');
  }
}

static void ctu_progress_draw_gradient(struct CtCanvas *bar_canvas,
                                          uint8_t value,
                                          struct CtRGB gradient_start,
                                          struct CtRGB gradient_end) {
  size_t width = bar_canvas->max_x;
  size_t center_row = bar_canvas->max_y / 2;
  if (width == 0 || bar_canvas->max_y == 0)
    return;

  size_t filled_columns = ((size_t)value * width + 50) / 100;
  if (filled_columns > width)
    filled_columns = width;

  for (size_t column = 0; column < width; column++) {
    if (column < filled_columns) {
      float blend = (filled_columns <= 1)
                        ? 0.0f
                        : (float)column / (float)(filled_columns - 1);
      struct CtRGB color =
          ctu_progress_lerp_rgb(gradient_start, gradient_end, blend);
      ct_brush_fg(color.r, color.g, color.b);
      ct_brush_bg(color.r, color.g, color.b);
    } else {
      ct_brush_fg(60, 60, 70);
      ct_brush_bg(30, 30, 36);
    }
    ct_cwrite_char(bar_canvas, column, center_row, L' ');
  }
}

static void ctu_progress_draw_overlay(
    struct CtCanvas *bar_canvas, uint8_t value, enum ProgressStyle style,
    struct CtRGB fill_color, struct CtRGB gradient_start, struct CtRGB gradient_end) {
  wchar_t label[8];
  ctu_progress_format_percent(label, 8, value);
  size_t label_length = wcslen(label);
  if (label_length == 0 || label_length > bar_canvas->max_x ||
      bar_canvas->max_y == 0)
    return;

  size_t label_start_column = (bar_canvas->max_x - label_length) / 2;
  size_t center_row = bar_canvas->max_y / 2;
  float filled_cells = ctu_progress_ratio(value) * (float)bar_canvas->max_x;
  size_t filled_columns = (size_t)(filled_cells + 0.5f);
  if (filled_columns > bar_canvas->max_x)
    filled_columns = bar_canvas->max_x;

  for (size_t label_index = 0; label_index < label_length; label_index++) {
    size_t column = label_start_column + label_index;
    if (style == P_SOLID || style == P_GRADIENT) {
      if ((float)column < filled_cells) {
        struct CtRGB background = fill_color;
        if (style == P_GRADIENT) {
          size_t gradient_span = filled_columns <= 1 ? 1 : filled_columns - 1;
          float blend = (float)column / (float)gradient_span;
          background =
              ctu_progress_lerp_rgb(gradient_start, gradient_end, blend);
        }
        ctu_progress_contrast_foreground(background);
        ct_brush_bg(background.r, background.g, background.b);
      } else {
        ct_brush_fg(210, 210, 220);
        ct_brush_bg(30, 30, 36);
      }
    } else {
      ct_brush_fg(230, 230, 240);
    }
    ct_cwrite_char(bar_canvas, column, center_row, label[label_index]);
  }
}

static void ctu_progress_render(const void *uncasted_model,
                                   struct CtCanvas *canvas) {
  const struct CursedProgress *model = uncasted_model;
  if (canvas->max_x == 0 || canvas->max_y == 0)
    return;

  struct CtBrush brush_backup = ct_brush();
  uint8_t value = ctu_progress_clamp(model->value);

  struct CtCanvas outer_canvas = *canvas;
  struct CtCanvas body_canvas;
  if (model->border)
    ct_cborder(&outer_canvas, &body_canvas);
  else
    body_canvas = outer_canvas;

  if (body_canvas.max_x == 0 || body_canvas.max_y == 0) {
    ct_brush_from(brush_backup);
    return;
  }

  struct CtCanvas bar_canvas = body_canvas;
  struct CtCanvas trail_canvas;
  bool has_trail = model->percent == PP_TRAIL && body_canvas.max_x > 6;

  ct_brush_clean();

  if (has_trail) {
    size_t trail_width = 5;
    if (trail_width >= body_canvas.max_x)
      trail_width = 4;
    if (trail_width >= body_canvas.max_x)
      has_trail = false;
    else {
      ct_ccutout(&body_canvas, &bar_canvas, 0, 0, body_canvas.max_x - trail_width,
              body_canvas.max_y);
      ct_ccutout(&body_canvas, &trail_canvas, body_canvas.max_x - trail_width, 0,
              body_canvas.max_x, body_canvas.max_y);
    }
  }

  switch (model->style) {
  case P_CLASSIC:
    ctu_progress_draw_classic(&bar_canvas, value);
    break;
  case P_BLOCK:
    ctu_progress_draw_block(&bar_canvas, value, false);
    break;
  case P_PARTIAL:
    ctu_progress_draw_block(&bar_canvas, value, true);
    break;
  case P_SOLID:
    ctu_progress_draw_solid(&bar_canvas, value, model->fill_color);
    break;
  case P_GRADIENT:
    ctu_progress_draw_gradient(&bar_canvas, value, model->gradient_from,
                                  model->gradient_to);
    break;
  }

  if (model->percent == PP_OVERLAY)
    ctu_progress_draw_overlay(&bar_canvas, value, model->style,
                                 model->fill_color, model->gradient_from,
                                 model->gradient_to);
  else if (has_trail) {
    ct_brush_clean();
    ct_brush_fg(200, 200, 210);
    wchar_t label[8];
    ctu_progress_format_percent(label, 8, value);
    ct_cwrite_cc(&trail_canvas, label);
  }

  ct_brush_from(brush_backup);
}

static void ctu_progress_base_setup(struct CursedProgress *model,
                                       uint8_t value, enum ProgressStyle style,
                                       enum ProgressPercent percent) {
  model->base.cleanup = _empty_cleanup;
  model->base.handler = _empty_handler;
  model->base.render = ctu_progress_render;

  model->value = ctu_progress_clamp(value);
  model->style = style;
  model->percent = percent;
  model->border = false;
  model->fill_color = (struct CtRGB){80, 200, 120};
  model->gradient_from = (struct CtRGB){50, 100, 255};
  model->gradient_to = (struct CtRGB){50, 255, 120};
}

void ctu_progress_setup_classic(struct CursedProgress *model,
                                   uint8_t value,
                                   enum ProgressPercent percent) {
  ctu_progress_base_setup(model, value, P_CLASSIC, percent);
}

void ctu_progress_setup_block(struct CursedProgress *model,
                                 uint8_t value, enum ProgressPercent percent) {
  ctu_progress_base_setup(model, value, P_BLOCK, percent);
}

void ctu_progress_setup_partial(struct CursedProgress *model,
                                   uint8_t value,
                                   enum ProgressPercent percent) {
  ctu_progress_base_setup(model, value, P_PARTIAL, percent);
}

void ctu_progress_setup_solid(struct CursedProgress *model,
                                 uint8_t value, enum ProgressPercent percent,
                                 struct CtRGB fill_color) {
  ctu_progress_base_setup(model, value, P_SOLID, percent);
  model->fill_color = fill_color;
}

void ctu_progress_setup_gradient(struct CursedProgress *model,
                                    uint8_t value, enum ProgressPercent percent,
                                    struct CtRGB gradient_from,
                                    struct CtRGB gradient_to) {
  ctu_progress_base_setup(model, value, P_GRADIENT, percent);
  model->gradient_from = gradient_from;
  model->gradient_to = gradient_to;
}

/* oracle made */
#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define RAMP_ROWS 5
#define RAMP_STEPS_MAX 64

struct Root {
  struct CtModel base;
  bool swapped;
  int page;
};

/* Leave headroom for chrome colors; 5 ramps share the palette. */
static unsigned ramp_steps(void) {
  int colors = ct_brush_colors();
  if (colors <= 16)
    return 16;
  unsigned usable = (unsigned)colors;
  if (usable > 16)
    usable -= 16; /* avoid default 0-15 when possible */
  if (usable > 40)
    usable -= 40; /* chrome / labels */
  else
    usable = usable / 2;
  unsigned per_ramp = usable / RAMP_ROWS;
  if (per_ramp < 8)
    per_ramp = 8;
  if (per_ramp > RAMP_STEPS_MAX)
    per_ramp = RAMP_STEPS_MAX;
  return per_ramp;
}

static uint8_t quantize(uint8_t v, unsigned steps) {
  if (steps <= 1)
    return v;
  unsigned q = (unsigned)v * (steps - 1) / 255;
  return (uint8_t)(q * 255 / (steps - 1));
}

static void ramp_rgb(int channel, uint8_t t, uint8_t *r, uint8_t *g,
                     uint8_t *b) {
  *r = *g = *b = 0;
  if (channel == 0)
    *r = *g = *b = t;
  else if (channel == 1)
    *r = t;
  else if (channel == 2)
    *g = t;
  else if (channel == 3)
    *b = t;
  else {
    float tf = (float)t / 255.0f;
    float h = tf * 6.0f;
    int i = (int)h;
    if (i > 5)
      i = 5;
    float f = h - (float)i;
    uint8_t q = (uint8_t)((1.0f - f) * 255.0f);
    uint8_t t2 = (uint8_t)(f * 255.0f);
    switch (i) {
    case 0:
      *r = 255;
      *g = t2;
      *b = 0;
      break;
    case 1:
      *r = q;
      *g = 255;
      *b = 0;
      break;
    case 2:
      *r = 0;
      *g = 255;
      *b = t2;
      break;
    case 3:
      *r = 0;
      *g = q;
      *b = 255;
      break;
    case 4:
      *r = t2;
      *g = 0;
      *b = 255;
      break;
    default:
      *r = 255;
      *g = 0;
      *b = q;
      break;
    }
  }
}

static void fill_ramp(struct CtCanvas *canvas, int channel) {
  size_t w = canvas->max_x;
  size_t h = canvas->max_y;
  if (w == 0 || h == 0)
    return;

  unsigned steps = ramp_steps();
  ct_brush_fg(0, 0, 0);
  for (size_t x = 0; x < w; x++) {
    uint8_t raw = (uint8_t)((x * 255) / (w > 1 ? w - 1 : 1));
    uint8_t t = quantize(raw, steps);
    uint8_t r, g, b;
    ramp_rgb(channel, t, &r, &g, &b);
    ct_brush_bg(r, g, b);
    for (size_t y = 0; y < h; y++)
      ct_cwrite_char(canvas, x, y, L' ');
  }
  ct_brush_clean();
}

static void draw_swatch(struct CtCanvas *cell, uint8_t r, uint8_t g, uint8_t b,
                        const wchar_t *label) {
  ct_brush_bg(r, g, b);
  ct_cfill(cell, L' ');
  if (label && cell->max_x > 0 && cell->max_y > 0) {
    unsigned lum = (unsigned)r * 3u + (unsigned)g * 6u + (unsigned)b;
    if (lum > 127u * 10u)
      ct_brush_fg(0, 0, 0);
    else
      ct_brush_fg(255, 255, 255);
    ct_brush_bg(r, g, b);
    ct_cwrite_cc(cell, label);
  }
  ct_brush_clean();
}

static void render_ramps(struct CtCanvas *body) {
  static const wchar_t *names[] = {L"grey", L"red", L"green", L"blue", L"rain"};
  if (body->max_y < RAMP_ROWS * 2 || body->max_x < 12)
    return;

  size_t row_h = body->max_y / RAMP_ROWS;
  if (row_h < 2)
    row_h = 2;

  for (size_t i = 0; i < RAMP_ROWS; i++) {
    size_t y0 = i * row_h;
    size_t y1 = y0 + row_h;
    if (i == RAMP_ROWS - 1)
      y1 = body->max_y;
    if (y1 <= y0 + 1)
      continue;

    /* leave 1-row gap between strips when possible */
    size_t bar_y0 = y0;
    size_t bar_y1 = (y1 > y0 + 2) ? y1 - 1 : y1;

    struct CtCanvas strip, label, bar;
    ct_brush_clean();
    ct_ccutout(body, &strip, 0, bar_y0, body->max_x, bar_y1);
    ct_cvwsplit(&strip, &label, &bar, 1, 12);

    ct_brush_fg(180, 180, 190);
    ct_brush_bg(12, 12, 16);
    ct_cfill(&label, L' ');
    if (label.max_x > 0 && label.max_y > 0)
      ct_cwrite_sc(&label, names[i]);
    ct_brush_clean();

    fill_ramp(&bar, (int)i);
  }
}

static void render_named(struct CtCanvas *body) {
  struct {
    uint8_t r, g, b;
    const wchar_t *name;
  } colors[] = {
      {255, 0, 0, L"red"},       {0, 255, 0, L"green"},
      {0, 0, 255, L"blue"},      {255, 255, 0, L"yellow"},
      {0, 255, 255, L"cyan"},    {255, 0, 255, L"magenta"},
      {255, 128, 0, L"orange"},  {128, 0, 255, L"purple"},
      {255, 255, 255, L"white"}, {40, 40, 40, L"dark"},
      {255, 105, 180, L"pink"},  {0, 128, 128, L"teal"},
  };
  size_t n = sizeof(colors) / sizeof(colors[0]);
  size_t cols = 4;
  size_t grid_rows = (n + cols - 1) / cols;
  if (body->max_x < cols || body->max_y < grid_rows)
    return;

  for (size_t i = 0; i < n; i++) {
    size_t col = i % cols;
    size_t row = i / cols;
    size_t x0 = (col * body->max_x) / cols;
    size_t x1 = ((col + 1) * body->max_x) / cols;
    size_t y0 = (row * body->max_y) / grid_rows;
    size_t y1 = ((row + 1) * body->max_y) / grid_rows;
    if (x1 <= x0 + 1 || y1 <= y0 + 1)
      continue;

    struct CtCanvas cell, inner;
    ct_brush_clean();
    ct_ccutout(body, &cell, x0, y0, x1, y1);
    ct_cmargin(&cell, &inner, 1, 1, 1, 1);
    draw_swatch(&inner, colors[i].r, colors[i].g, colors[i].b, colors[i].name);
  }
}

static void render_hex(struct CtCanvas *body) {
  const char *hexes[] = {"#f00",    "#0f0",    "#00f",    "#ff8800",
                         "#88ff00", "#0088ff", "#ff0088", "#abcdef",
                         "#123456", "#ffffff", "#000000", "#7f7f7f"};
  const wchar_t *labels[] = {L"#f00",    L"#0f0",    L"#00f",    L"#ff8800",
                             L"#88ff00", L"#0088ff", L"#ff0088", L"#abcdef",
                             L"#123456", L"#ffffff", L"#000000", L"#7f7f7f"};
  size_t n = sizeof(hexes) / sizeof(hexes[0]);
  size_t cols = 4;
  size_t grid_rows = (n + cols - 1) / cols;
  if (body->max_x < cols || body->max_y < grid_rows)
    return;

  for (size_t i = 0; i < n; i++) {
    size_t col = i % cols;
    size_t row = i / cols;
    size_t x0 = (col * body->max_x) / cols;
    size_t x1 = ((col + 1) * body->max_x) / cols;
    size_t y0 = (row * body->max_y) / grid_rows;
    size_t y1 = ((row + 1) * body->max_y) / grid_rows;
    if (x1 <= x0 + 1 || y1 <= y0 + 1)
      continue;

    struct CtCanvas cell, inner;
    ct_brush_clean();
    ct_ccutout(body, &cell, x0, y0, x1, y1);
    ct_cmargin(&cell, &inner, 1, 1, 1, 1);

    ct_brush_bg_hex(hexes[i]);
    ct_cfill(&inner, L' ');
    if (hexes[i][1] == 'f' || hexes[i][1] == 'F' || hexes[i][1] == 'a' ||
        hexes[i][1] == 'A' || hexes[i][1] == '8' || hexes[i][1] == '7')
      ct_brush_fg_hex("#000000");
    else
      ct_brush_fg_hex("#ffffff");
    ct_brush_bg_hex(hexes[i]);
    if (inner.max_x > 0 && inner.max_y > 0)
      ct_cwrite_cc(&inner, labels[i]);
    ct_brush_clean();
  }
}

static void render_attrs(struct CtCanvas *body, bool swapped) {
  struct CtCanvas top, bot, a, b, c, d;

  if (body->max_x < 4 || body->max_y < 4)
    return;

  ct_chsplit(body, &top, &bot);
  ct_cvsplit(&top, &a, &b);
  ct_cvsplit(&bot, &c, &d);

  ct_brush_fg(255, 220, 100);
  ct_brush_bg(30, 30, 50);
  if (swapped)
    ct_brush_swap_colors();
  ct_brush_attr(A_NORMAL);
  ct_cborder(&a, NULL);
  ct_cwrite_cc(&a, L"A_NORMAL (+ swap .)");

  ct_brush_fg(255, 220, 100);
  ct_brush_bg(30, 30, 50);
  if (swapped)
    ct_brush_swap_colors();
  ct_brush_attr(A_BOLD);
  ct_cborder(&b, NULL);
  ct_cwrite_cc(&b, L"A_BOLD");

  ct_brush_fg(255, 220, 100);
  ct_brush_bg(30, 30, 50);
  if (swapped)
    ct_brush_swap_colors();
  ct_brush_attr(A_UNDERLINE);
  ct_cborder(&c, NULL);
  ct_cwrite_cc(&c, L"A_UNDERLINE");

  ct_brush_fg(255, 220, 100);
  ct_brush_bg(30, 30, 50);
  if (swapped)
    ct_brush_swap_colors();
  ct_brush_attr(A_REVERSE);
  ct_cborder(&d, NULL);
  ct_cwrite_cc(&d, L"A_REVERSE");

  ct_brush_clean();
}

void cleanup(void *uncasted_model) { (void)uncasted_model; }

void handle(void *uncasted_model, const struct CtEvent *event) {
  struct Root *model = uncasted_model;
  if (event->type != KEY_EVENT)
    return;

  if (event->key == L'q')
    ct_event_send_exit();
  else if (event->key == L'.')
    model->swapped = !model->swapped;
  else if (event->key == L'1')
    model->page = 0;
  else if (event->key == L'2')
    model->page = 1;
  else if (event->key == L'3')
    model->page = 2;
  else if (event->key == L'4')
    model->page = 3;
  else if (event->key == L'n' || event->key == L' ')
    model->page = (model->page + 1) % 4;
  else if (event->key == L'p')
    model->page = (model->page + 3) % 4;
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const struct Root *model = uncasted_model;
  struct CtCanvas header, body, title_row, caps_row, content;

  if (canvas->max_y < 5)
    return;

  ct_chwsplit(canvas, &header, &body, 2,
              canvas->max_y > 2 ? canvas->max_y - 2 : 1);

  ct_brush_fg(180, 180, 200);
  ct_brush_bg(16, 16, 22);
  ct_cfill(&header, L' ');
  ct_chsplit(&header, &title_row, &caps_row);

  const wchar_t *page_name = L"?";
  switch (model->page) {
  case 0:
    page_name = L"ramps";
    break;
  case 1:
    page_name = L"RGB swatches";
    break;
  case 2:
    page_name = L"hex";
    break;
  case 3:
    page_name = L"attributes";
    break;
  }

  wchar_t title_buf[160];
  swprintf(title_buf, 160, L" color — %ls   [1-4/n/p] [.]swap [q]uit ",
           page_name);
  ct_brush_fg(120, 200, 255);
  ct_brush_bg(16, 16, 22);
  if (title_row.max_x > 0 && title_row.max_y > 0)
    ct_cwrite_sc(&title_row, title_buf);

  wchar_t caps_buf[160];
  swprintf(caps_buf, 160,
           L" color=%ls  truecolor=%ls  COLORS=%d  COLOR_PAIRS=%d  steps=%u ",
           ct_brush_has_color() ? L"yes" : L"no",
           ct_brush_has_truecolor() ? L"yes" : L"no", ct_brush_colors(),
           ct_brush_color_pairs(), ramp_steps());
  ct_brush_fg(130, 130, 150);
  ct_brush_bg(16, 16, 22);
  if (caps_row.max_x > 0 && caps_row.max_y > 0)
    ct_cwrite_sc(&caps_row, caps_buf);
  ct_brush_clean();

  ct_brush_fg(200, 200, 200);
  ct_brush_bg(0, 0, 0);
  ct_cfill(&body, L' ');
  ct_brush_clean();

  ct_cmargin(&body, &content, 1, 1, 0, 1);

  switch (model->page) {
  case 0:
    render_ramps(&content);
    break;
  case 1:
    render_named(&content);
    break;
  case 2:
    render_hex(&content);
    break;
  case 3:
    render_attrs(&content, model->swapped);
    break;
  }
}

void init(struct Root *model) {
  model->base.cleanup = cleanup;
  model->base.render = render;
  model->base.handler = handle;
  model->swapped = false;
  model->page = 0;
}

int main(void) {
  struct Root model;
  ct_app_init();
  init(&model);
  ct_app_start(&model.base);
  return EXIT_SUCCESS;
}

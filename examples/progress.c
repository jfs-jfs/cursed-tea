#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/progress.h>
#include <stdint.h>
#include <stdlib.h>

#define PROGRESS_TICK 2001
#define TICK_MS 40

typedef struct {
  struct CtModel base;
  struct CursedProgress progress;
  bool auto_run;
  int direction;
} Root;

static const wchar_t *style_name(enum ProgressStyle style) {
  switch (style) {
  case P_CLASSIC:
    return L"classic";
  case P_BLOCK:
    return L"block";
  case P_PARTIAL:
    return L"partial";
  case P_SOLID:
    return L"solid";
  case P_GRADIENT:
    return L"gradient";
  }
  return L"?";
}

static const wchar_t *percent_name(enum ProgressPercent percent) {
  switch (percent) {
  case PP_NONE:
    return L"none";
  case PP_OVERLAY:
    return L"overlay";
  case PP_TRAIL:
    return L"trail";
  }
  return L"?";
}

static void schedule_tick(Root *model) {
  ct_event_send_custom_delayed(PROGRESS_TICK, TICK_MS, model);
}

void handler(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type == START_EVENT) {
    schedule_tick(model);
    return;
  }

  if (event->type == CUSTOM_EVENT && event->custom_signal == PROGRESS_TICK &&
      event->data == uncasted_model) {
    if (model->auto_run) {
      int next_value = (int)model->progress.value + model->direction;
      if (next_value >= 100) {
        next_value = 100;
        model->direction = -1;
      } else if (next_value <= 0) {
        next_value = 0;
        model->direction = 1;
      }
      model->progress.value = (uint8_t)next_value;
    }
    schedule_tick(model);
    return;
  }

  if (event->type != KEY_EVENT)
    return;

  uint8_t value = model->progress.value;
  enum ProgressPercent percent = model->progress.percent;
  bool border = model->progress.border;

  switch (event->key) {
  case L'q':
    ct_event_send_exit();
    break;
  case L'1':
    ctu_progress_setup_classic(&model->progress, value, percent);
    model->progress.border = border;
    break;
  case L'2':
    ctu_progress_setup_block(&model->progress, value, percent);
    model->progress.border = border;
    break;
  case L'3':
    ctu_progress_setup_partial(&model->progress, value, percent);
    model->progress.border = border;
    break;
  case L'4':
    ctu_progress_setup_solid(&model->progress, value, percent,
                             (struct CtRGB){80, 200, 120});
    model->progress.border = border;
    break;
  case L'5':
    ctu_progress_setup_gradient(&model->progress, value, percent,
                                (struct CtRGB){50, 100, 255},
                                (struct CtRGB){50, 255, 120});
    model->progress.border = border;
    break;
  case L'p':
    model->progress.percent =
        (enum ProgressPercent)((model->progress.percent + 1) % 3);
    break;
  case L'b':
    model->progress.border = !model->progress.border;
    break;
  case L' ':
    model->auto_run = !model->auto_run;
    break;
  case L'+':
  case L'=': {
    int next_value = (int)model->progress.value + 5;
    if (next_value > 100)
      next_value = 100;
    model->progress.value = (uint8_t)next_value;
    break;
  }
  case L'-':
  case L'_': {
    int next_value = (int)model->progress.value - 5;
    if (next_value < 0)
      next_value = 0;
    model->progress.value = (uint8_t)next_value;
    break;
  }
  case L'0':
    model->progress.value = 0;
    break;
  case L'9':
    model->progress.value = 100;
    break;
  }
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas body_canvas, bar_region, info_region;
  wchar_t status_line[128];

  ct_cborder(canvas, NULL);
  ct_cwrite_cs(canvas, L"[ PROGRESS ]");
  ct_cwrite_ce(canvas,
               L"[ 1-5:style p:%  b:border space:auto +/- 0/9 q:quit ]");

  ct_cmargin(canvas, &body_canvas, 2, 2, 2, 2);
  if (body_canvas.max_y < 4 || body_canvas.max_x < 10)
    return;

  ct_chwsplit(&body_canvas, &bar_region, &info_region, 3,
              body_canvas.max_y > 3 ? body_canvas.max_y - 3 : 1);

  struct CtCanvas bar_canvas;
  ct_ccutout_c(&bar_region, &bar_canvas,
               bar_region.max_x > 4 ? bar_region.max_x - 2 : bar_region.max_x,
               bar_region.max_y > 2 ? 3 : bar_region.max_y);
  model->progress.base.render(&model->progress, &bar_canvas);

  swprintf(status_line, 128,
           L" style=%ls  percent=%ls  border=%ls  auto=%ls  value=%u%% ",
           style_name(model->progress.style),
           percent_name(model->progress.percent),
           model->progress.border ? L"on" : L"off",
           model->auto_run ? L"on" : L"off", (unsigned)model->progress.value);

  ct_brush_fg(160, 170, 190);
  if (info_region.max_x > 0 && info_region.max_y > 0)
    ct_cwrite_sc(&info_region, status_line);
  ct_brush_clean();
}

void setup(Root *model) {
  model->base.cleanup = _empty_cleanup;
  model->base.render = render;
  model->base.handler = handler;
  model->auto_run = true;
  model->direction = 1;

  ctu_progress_setup_block(&model->progress, 35, PP_OVERLAY);
}

int main(void) {
  Root root;
  ct_app_init();
  setup(&root);
  ct_app_start(&root.base);
  return EXIT_SUCCESS;
}

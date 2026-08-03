#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <stdbool.h>

struct Root {
  struct CtModel base;
  bool on;
};

void cleanup(void *uncasted_model) {}

void handle(void *uncasted_model, const struct CtEvent *event) {
  struct Root *model = uncasted_model;
  if (event->type == KEY_EVENT) {

    if (event->key == L'q')
      ct_event_send_exit();
    else if (event->key == L'.')
      model->on = !model->on;
  }
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const struct Root *model = uncasted_model;

  struct CtCanvas a1, a2, a3, a4;

  ct_cwrite_ce(canvas, L"[ .:Changes borders q:Quits ]");
  ct_cmargin_y(canvas, &a1, 0, 1);
  ct_cvsplit(&a1, &a1, &a3);

  ct_chsplit(&a1, &a1, &a2);
  ct_chsplit(&a3, &a3, &a4);

  if (model->on) {
    ct_brush_border(B_NORMAL);
    ct_cborder(&a1, NULL);
    ct_cwrite_cc(&a1, L"BORDER_NORMAL");

    ct_brush_border(B_DOUBLE);
    ct_cborder(&a2, NULL);
    ct_cwrite_cc(&a2, L"BORDER_DOUBLE");

    ct_brush_border(B_CURVED);
    ct_cborder(&a3, NULL);
    ct_cwrite_cc(&a3, L"BORDER_CURVED");

    ct_brush_border(B_BLOCK);
    ct_cborder(&a4, NULL);
    ct_cwrite_cc(&a4, L"BORDER_BLOCK");

  } else {
    ct_brush_border(B_HALFBLOCK_OUTER);
    ct_cborder(&a1, NULL);
    ct_cwrite_cc(&a1, L"BORDER_HALFBLOCK_OUTER");

    ct_brush_border(B_HALFBLOCK_INNER);
    ct_cborder(&a2, NULL);
    ct_cwrite_cc(&a2, L"BORDER_HALFBLOCK_INNER");

    ct_brush_border(B_THICK);
    ct_cborder(&a3, NULL);
    ct_cwrite_cc(&a3, L"BORDER_THICK");

    ct_brush_border(B_ASCII);
    ct_cborder(&a4, NULL);
    ct_cwrite_cc(&a4, L"BORDER_ASCII");
  }
}

void init(struct Root *model) {

  model->base.cleanup = cleanup;
  model->base.render = render;
  model->base.handler = handle;
  model->on = true;
}

int main() {
  struct Root model;
  ct_app_init();
  init(&model);
  ct_app_start(&model.base);
  return EXIT_SUCCESS;
}

#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/line.h>

typedef struct Root {
  struct CtModel base;
  struct CursedLine l1, l2, l3;
} Root;

void handle(void *uncasted_model, const struct CtEvent *event) {
  if (event->type == KEY_EVENT && event->key == L'q')
    ct_event_send_exit();
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas aux, inner;

  ct_cborder(canvas, &inner);
  ct_cwrite_ce(canvas, L"[ q: to exit ]");

  ct_canvas_copy(&inner, &aux);
  while (aux.max_x > 2 && aux.max_y > 1) {
    ct_cmargin(&aux, &aux, 1, 0, 0, 2);
    model->l1.base.render(&model->l1, &aux);
  }

  ct_canvas_copy(&inner, &aux);
  while (aux.max_x > 2 && aux.max_y > 1) {
    ct_cmargin(&aux, &aux, 1, 2, 0, 0);
    model->l2.base.render(&model->l2, &aux);
  }

  ct_canvas_copy(&inner, &aux);
  ct_cmargin(&aux, &aux, 0, 2, 0, 2);
  while (aux.max_x > 2 && aux.max_y > 1) {
    model->l3.base.render(&model->l3, &aux);
    ct_cmargin(&aux, &aux, 1, 2, 0, 2);
  }
}

void root_setup(Root *root) {
  root->base.cleanup = _empty_cleanup;
  root->base.render = render;
  root->base.handler = handle;

  ctu_line_setup(&root->l1, B_BLOCK, O_LEFT);
  ctu_line_setup(&root->l2, B_BLOCK, O_RIGHT);
  ctu_line_setup(&root->l3, B_HALFBLOCK_INNER, O_UP);
}

int main() {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);
  return EXIT_SUCCESS;
}

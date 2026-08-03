#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/label.h>

typedef struct Root {
  struct CtModel base;
  struct CursedLabel label;
} Root;

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas aux;

  ct_brush_border(B_BLOCK);
  ct_cborder(canvas, &aux);
  ct_cwrite_ce(canvas, L"[ q: to exit ]");
  model->label.base.render(&model->label, &aux);
  ct_brush_border(B_NORMAL);
}

void handle(void *uncasted_model, const struct CtEvent *event) {
  if (event->type == KEY_EVENT && event->key == L'q')
    ct_event_send_exit();
}

void root_setup(struct Root *root) {
  root->base.cleanup = _empty_cleanup;
  root->base.render = render;
  root->base.handler = handle;
  ctu_label_setup(&root->label,
                  L"Hola món!", // Text
                  false,        // Border
                  true,         // Center
                  1, 0          // Paddings
  );
}

int main() {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);
  return EXIT_SUCCESS;
}

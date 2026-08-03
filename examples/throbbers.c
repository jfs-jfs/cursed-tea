#include "cursed-tea/application.h"
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/throbber.h>

typedef struct {
  struct CtModel base;
  struct CursedThrobber throbber;
} Root;

void handler(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;
  if (event->type == KEY_EVENT) {
    switch (event->key) {
    case L'q':
      ct_event_send_exit();
      return;

    case L'1':
      model->throbber.style = T_LINE;
      break;

    case L'2':
      model->throbber.style = T_DOT;
      break;

    case L'3':
      model->throbber.style = T_MINIDOT;
      break;

    case L'4':
      model->throbber.style = T_JUMP;
      break;

    case L'5':
      model->throbber.style = T_PULSE;
      break;
    }
  }
  model->throbber.base.handler(&model->throbber, event);
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas aux;

  ct_cborder(canvas, NULL);
  ct_cwrite_ce(canvas, L"[ q:quit 1:line 2:dot 3:minidot 4:jump 5:pulse ]");
  ct_cwrite_cs(canvas, L"[ THROBBERS ]");
  ct_ccutout_c(canvas, &aux, 1, 1);

  model->throbber.base.render(&model->throbber, &aux);
}

void setup(Root *model) {
  model->base.cleanup = _empty_cleanup;
  model->base.render = render;
  model->base.handler = handler;

  ctu_throbber_setup(&model->throbber, T_LINE);
}

int main() {
  Root root;
  ct_app_init();
  setup(&root);
  ct_app_start(&root.base);
  return EXIT_SUCCESS;
}

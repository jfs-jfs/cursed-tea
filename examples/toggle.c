#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/toggle.h>
#include <stdlib.h>

struct TestToggleModel {
  struct CtModel base;
  struct CursedToggle toggle;
};

void cleanup(void *uncasted_model) {
  struct TestToggleModel *model = uncasted_model;
  model->toggle.base.cleanup(&model->toggle);
  free(model);
}

void handler(void *uncasted_model, const struct CtEvent *event) {
  struct TestToggleModel *model = uncasted_model;

  if (event->type == KEY_EVENT) {
    if (event->key == L'\t')
      model->toggle.focus = !model->toggle.focus;
    else if (event->key == L'q')
      ct_event_send_exit();
  }

  model->toggle.base.handler(&model->toggle, event);
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const struct TestToggleModel *model = uncasted_model;

  ct_cborder(canvas, NULL);
  ct_cwrite_cs(canvas, L"[ TOGGLE ]");
  ct_cwrite_ce(canvas, L"[ TAB:focus  SPACE:toggle  Q:exit ]");

  struct CtCanvas aux;
  ct_ccutout_c(canvas, &aux, 40, 1);
  model->toggle.base.render(&model->toggle, &aux);
}

struct CtModel *init() {
  struct TestToggleModel *test_model = malloc(sizeof(struct TestToggleModel));
  test_model->base.cleanup = cleanup;
  test_model->base.render = render;
  test_model->base.handler = handler;
  ctu_toggle_setup(&test_model->toggle, L"Enable feature", false, 0x1337, 25);
  return &test_model->base;
}

int main() {
  ct_app_init();
  ct_app_start(init());
  return EXIT_SUCCESS;
}

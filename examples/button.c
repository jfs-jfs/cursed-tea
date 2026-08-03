#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/button.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

struct TestButtonModel {
  struct CtModel base;
  struct CursedButton button;
};

void cleanup(void *uncasted_model) {
  struct TestButtonModel *model = uncasted_model;
  model->button.base.cleanup(&model->button);
  free(model);
}

void handler(void *uncasted_model, const struct CtEvent *event) {
  struct TestButtonModel *model = uncasted_model;

  if (event->type == KEY_EVENT) {
    if (event->key == L'\t')
      model->button.has_focus = !model->button.has_focus;
    else if (event->key == L'q')
      ct_event_send_exit();
  }

  model->button.base.handler(&model->button, event);
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const struct TestButtonModel *model = uncasted_model;

  ct_cborder(canvas, NULL);
  ct_cwrite_ce(canvas, L"[ TAB:selects/deselects button Q:Exits ]");

  model->button.base.render(&model->button, canvas);
}

struct CtModel *init() {
  struct TestButtonModel *test_model = malloc(sizeof(struct TestButtonModel));
  test_model->base.cleanup = cleanup;
  test_model->base.render = render;
  test_model->base.handler = handler;
  ctu_button_static_init(L"TEST BUTTON", true, false,
                         A_REVERSE | A_BOLD | A_ITALIC, 0x00,
                         &test_model->button);
  return &test_model->base;
}

int main() {
  ct_app_init();
  ct_app_start(init());
  return EXIT_SUCCESS;
}

#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <stdbool.h>
#include <stddef.h>

#define TICK_EVENT 0x001

typedef struct Root {
  struct CtModel base;
  size_t _step;
  size_t _max_steps;
} Root;

void handler(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type == RESIZE_EVENT) {
    model->_max_steps = (event->new_size.x / 4) - 1;
    model->_step = 0;
    ct_event_send_custom_delayed(TICK_EVENT, 10, NULL);
  }

  else if (event->type == CUSTOM_EVENT && event->custom_signal == TICK_EVENT &&
           model->_step < model->_max_steps) {
    model->_step++;
    ct_event_send_custom_delayed(TICK_EVENT, 10, NULL);
  }

  else if (event->type == KEY_EVENT && event->key == L'q')
    ct_event_send_exit();
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;

  struct CtCanvas inner;
  ct_ccutout_c(canvas, &inner, 1 + model->_step * 4, canvas->max_y);
  ct_cborder(&inner, NULL);
}

int main() {
  Root root;
  ct_app_init();

  root.base.cleanup = _empty_cleanup;
  root.base.handler = handler;
  root.base.render = render;
  root._step = 0;
  root._max_steps = 0;

  ct_app_start(&root.base);

  return EXIT_SUCCESS;
}

#include "cursed-tea/logger.h"
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/size-guard.h>
#include <stdbool.h>
#include <wchar.h>

void ctu_size_guard_handler(void *uncasted_model, const struct CtEvent *event) {
  struct CursedSizeGuard *model = uncasted_model;

  if (event->type == RESIZE_EVENT)
    model->_ok =
        model->min_x <= event->new_size.x && model->min_y <= event->new_size.y;

  if (!model->_ok) {
    log_debug(L"ctu_size_guard_handler: blocking handler");
    // Give an option to exit
    if (event->type == KEY_EVENT && event->key == L'\033') // ESC
      ct_event_send_exit();
    if (event->type == KEY_EVENT && event->key == L'q')
      ct_event_send_exit();
    return;
  }
  model->child->handler(model->child, event);
}

void ctu_size_guard_render(const void *uncasted_model,
                           struct CtCanvas *canvas) {
  const struct CursedSizeGuard *model = uncasted_model;
  wchar_t buffer[120];
  if (canvas->max_x < model->min_x) {
    log_debug(L"ctu_size_guard_render: blocking render");
    swprintf(buffer, 119, L"[ terminal too thin :: expand %zu columns ]",
             model->min_x - canvas->max_x);
    ct_cwrite_cc(canvas, buffer);
    return;
  }
  if (canvas->max_y < model->min_y) {
    log_debug(L"ctu_size_guard_render: blocking render");
    swprintf(buffer, 119, L"[ terminal too short :: expand %zu rows ]",
             model->min_y - canvas->max_y);
    ct_cwrite_cc(canvas, buffer);
    return;
  }
  model->child->render(model->child, canvas);
}

void ctu_size_guard_setup(struct CursedSizeGuard *model, const size_t min_x,
                          const size_t min_y, struct CtModel *child) {
  model->base.cleanup = _empty_cleanup;
  model->base.handler = ctu_size_guard_handler;
  model->base.render = ctu_size_guard_render;
  model->min_x = min_x;
  model->min_y = min_y;
  model->child = child;
  model->_ok = true;
}

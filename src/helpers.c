#include <cursed-tea/helpers.h>

void ct_subcleanup(struct CtModel *model) { model->cleanup(model); }

void ct_subhandler(struct CtModel *model, const struct CtEvent *event) {
  model->handler(model, event);
}

void ct_subrender(const struct CtModel *model, struct CtCanvas *canvas) {
  model->render(model, canvas);
}

void ct_empty_cleanup(void *uncasted_model) { (void)uncasted_model; }

void ct_empty_handler(void *uncasted_model, const struct CtEvent *event) {
  (void)uncasted_model;
  (void)event;
}

void ct_empty_render(const void *uncasted_model, struct CtCanvas *canvas) {
  (void)uncasted_model;
  (void)canvas;
}

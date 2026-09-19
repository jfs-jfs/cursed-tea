#include <cursed-tea/helpers.h>

void ct_subcleanup(struct CtModel *model) { model->cleanup(model); }

void ct_subhandler(struct CtModel *model, const struct CtEvent *event) {
  model->handler(model, event);
}

void ct_subrender(const struct CtModel *model, struct CtCanvas *canvas) {
  model->render(model, canvas);
}

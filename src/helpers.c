#include <cursed-tea/helpers.h>

inline void ct_subcleanup(struct CtModel *model) { model->cleanup(model); }

inline void ct_subhandler(struct CtModel *model, const struct CtEvent *event) {
  model->handler(model, event);
}

inline void ct_subrender(const struct CtModel *model, struct CtCanvas *canvas) {
  model->render(model, canvas);
}

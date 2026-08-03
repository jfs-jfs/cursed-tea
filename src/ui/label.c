#include <cursed-tea/ui/label.h>
#include <cursed-tea/ui/common.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

static void ctu_label_render(const void *uncasted_model,
                                struct CtCanvas *canvas) {
  const struct CursedLabel *model = uncasted_model;

  size_t text_length = wcslen(model->text);
  struct CtCanvas inner;

  if (model->border) {
    if (model->center)
      ct_ccutout_c(canvas, &inner, text_length + model->padding_x * 2 + 2,
                model->padding_y * 2 + 3);
    else
      ct_ccutout(canvas, &inner, 0, 0, text_length + model->padding_x * 2 + 2,
              model->padding_y * 2 + 3);
    ct_cborder(&inner, &inner);
  } else {
    if (model->center)
      ct_ccutout_c(canvas, &inner, text_length + model->padding_x * 2,
                model->padding_y * 2);
    else
      ct_ccutout(canvas, &inner, 0, 0, text_length + model->padding_x * 2,
              model->padding_y * 2 + 1);
  }

  ct_cwrite_cc(&inner, model->text);
}

void ctu_label_setup(struct CursedLabel *model, const wchar_t *text,
                        bool border, bool center, size_t padding_x,
                        size_t padding_y) {
  model->base.cleanup = _empty_cleanup;
  model->base.handler = _empty_handler;
  model->base.render = ctu_label_render;

  model->center = center;
  model->padding_x = padding_x;
  model->padding_y = padding_y;
  model->border = border;
  clamp_string(model->text, text, CTU_LABEL_MAX_SIZE);
}

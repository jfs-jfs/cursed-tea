#include <cursed-tea/ui/labeled-input.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/label.h>
#include <stddef.h>
#include <wchar.h>

static void ctu_labeled_input_handler(void *uncasted_model,
                                         const struct CtEvent *event) {
  struct CursedLabeledInput *model = uncasted_model;
  model->input.focus = model->focus;
  if (!model->focus)
    return;

  model->input.base.handler(&model->input, event);
}

static void ctu_labeled_input_render(const void *uncasted_model,
                                        struct CtCanvas *canvas) {
  const struct CursedLabeledInput *model = uncasted_model;

  struct CtCanvas aux;
  struct CtBrush backup = ct_brush();

  ct_canvas_copy(canvas, &aux);

  if (model->focus)
    ct_brush_add_attr(A_BOLD | A_REVERSE);

  model->label.base.render(&model->label, &aux);

  ct_brush_from(backup);

  ct_cmargin_x(&aux, &aux, 0,
            wcslen(model->label.text) + model->label.padding_x * 2);
  ct_cwrite_char(&aux, 0, 0, L' ');

  ct_cmargin_x(&aux, &aux, 0, 1);
  model->input.base.render(&model->input, &aux);
}

void ctu_labeled_input_setup(struct CursedLabeledInput *model,
                                const wchar_t *text, bool secret,
                                size_t input_max_size, const size_t label_size,
                                int signal_on_submit) {
  model->base.cleanup = _empty_cleanup;
  model->base.render = ctu_labeled_input_render;
  model->base.handler = ctu_labeled_input_handler;

  size_t width = label_size;
  if (label_size > CTU_LABEL_MAX_SIZE)
    width = CTU_LABEL_MAX_SIZE;

  model->label_size = width;

  ctu_input_setup(&model->input, A_BOLD, secret, false, false, true,
                     input_max_size, signal_on_submit);

  const size_t text_length = wcslen(text);
  if (width <= 0 || text_length > width) {
    ctu_label_setup(&model->label, text, false, false, 0, 0);
    return;
  }

  wchar_t buffer[CTU_LABEL_MAX_SIZE + 1];
  clamp_string(buffer, text, CTU_LABEL_MAX_SIZE);

  for (size_t index = 0; index < width - text_length; index++)
    buffer[text_length + index] = L' ';
  buffer[width] = L'\0';

  ctu_label_setup(&model->label, buffer, false, false, 1, 0);
}

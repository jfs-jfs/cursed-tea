#include <cursed-tea/ui/toggle.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/label.h>
#include <stddef.h>
#include <wchar.h>

static void ctu_toggle_handler(void *uncasted_model,
                                  const struct CtEvent *event) {
  struct CursedToggle *model = uncasted_model;

  if (!model->focus)
    return;

  if (event->type == KEY_EVENT && event->key == L' ') {
    model->selected = !model->selected;
    ct_event_send_custom(model->signal_on_change, (void *)&model->selected);
  }
}

static void ctu_toggle_render(const void *uncasted_model,
                                 struct CtCanvas *canvas) {
  const struct CursedToggle *model = uncasted_model;

  struct CtBrush backup = ct_brush();
  if (model->focus)
    ct_brush_add_attr(A_BOLD | A_REVERSE);

  model->label.base.render(&model->label, canvas);

  ct_brush_from(backup);

  struct CtCanvas aux;
  ct_cmargin_x(canvas, &aux, 0,
            wcslen(model->label.text) + model->label.padding_x * 2);
  if (model->selected)
    ct_cwrite_ss(&aux, L" [X]");
  else
    ct_cwrite_ss(&aux, L" [ ]");
}

void ctu_toggle_setup(struct CursedToggle *model, const wchar_t *text,
                         bool selected, int signal_on_change,
                         const size_t label_size) {
  model->base.cleanup = _empty_cleanup;
  model->base.render = ctu_toggle_render;
  model->base.handler = ctu_toggle_handler;
  model->focus = false;
  model->selected = selected;
  model->signal_on_change = signal_on_change;

  size_t width = label_size;
  if (label_size > CTU_LABEL_MAX_SIZE)
    width = CTU_LABEL_MAX_SIZE;

  model->label_size = width;

  const size_t text_length = wcslen(text);
  if (width <= 0 || text_length > width) {
    ctu_label_setup(&model->label, text, false, false, 0, 0);
    return;
  }

  wchar_t buffer[CTU_LABEL_MAX_SIZE + 1];
  clamp_string(buffer, text, width);

  for (size_t index = 0; index < width - text_length; index++)
    buffer[text_length + index] = L' ';
  buffer[width] = L'\0';

  ctu_label_setup(&model->label, buffer, false, false, 1, 0);
}

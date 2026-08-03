#include <cursed-tea/ui/button.h>
#include <cursed-tea/ui/common.h>
#include <wchar.h>

static void ctu_button_handler(void *uncasted_model,
                                  const struct CtEvent *event) {
  struct CursedButton *model = uncasted_model;
  if (!model->has_focus)
    return;

  if (event->type == KEY_EVENT && (event->key == L'\n' || event->key == L'\r'))
    ct_event_send_custom(model->signal_on_pressed, NULL);
}

static void ctu_button_render(const void *uncasted_model,
                                 struct CtCanvas *canvas) {
  const struct CursedButton *model = uncasted_model;
  size_t text_length = wcslen(model->text);

  struct CtBrush old_brush = ct_brush();

  if (model->has_focus)
    ct_brush_add_attr(model->highlight);

  struct CtCanvas button_canvas;
  if (model->centered)
    if (model->border)
      ct_ccutout_c(canvas, &button_canvas, text_length + 2, 3);
    else
      ct_ccutout_c(canvas, &button_canvas, text_length, 1);
  else if (model->border)
    ct_ccutout(canvas, &button_canvas, 0, 0, text_length + 2, 3);
  else
    ct_ccutout(canvas, &button_canvas, 0, 0, text_length, 1);

  if (model->border)
    ct_cborder(&button_canvas, &button_canvas);

  ct_cwrite_cc(&button_canvas, model->text);

  if (model->has_focus)
    ct_brush_attr(old_brush.attribute);
}

void ctu_button_static_init(const wchar_t *text, bool center_text,
                               bool border, attr_t highlight,
                               int signal_on_pressed,
                               struct CursedButton *button_model) {
  struct CtModel *model = &button_model->base;

  if (border)
    clamp_string(button_model->text, text, MAX_TEXT_LENGTH);
  else {
    clamp_string(button_model->text, text, MAX_TEXT_LENGTH - 3);
    button_model->text[0] = L'<';
    size_t length = wcslen(button_model->text);
    button_model->text[length] = L'>';
  }

  button_model->centered = center_text;
  button_model->has_focus = false;
  button_model->signal_on_pressed = signal_on_pressed;
  button_model->border = border;
  button_model->highlight = highlight;
  model->handler = ctu_button_handler;
  model->render = ctu_button_render;
  model->cleanup = _empty_cleanup;
}

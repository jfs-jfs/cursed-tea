#include <cursed-tea/ui/input.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/events.h>
#include <wchar.h>

static void ctu_input_render(const void *uncasted_model,
                                struct CtCanvas *canvas) {
  const struct CtBrush backup = ct_brush();
  const struct CursedInput *model = uncasted_model;

  if (model->focus)
    ct_brush_add_attr(model->highlight);

  struct CtCanvas input_canvas;
  if (model->centered) {
    if (model->border) {
      ct_ccutout_c(canvas, &input_canvas, model->max_size + 2, 3);
      ct_cborder(&input_canvas, &input_canvas);
    } else {
      ct_ccutout_c(canvas, &input_canvas, model->max_size + 2, 1);
      ct_cwrite_char(&input_canvas, 0, 0, L'>');
      ct_cwrite_char(&input_canvas, model->max_size + 1, 0, L'<');
      ct_cmargin_x(&input_canvas, &input_canvas, 1, 1);
    }
  } else {
    if (model->border) {
      ct_ccutout(canvas, &input_canvas, 0, 0, model->max_size + 2, 3);
      ct_cborder(&input_canvas, &input_canvas);
    } else {
      ct_ccutout(canvas, &input_canvas, 0, 0, model->max_size + 2, 1);
      ct_cwrite_char(&input_canvas, 0, 0, L'>');
      ct_cwrite_char(&input_canvas, model->max_size + 1, 0, L'<');
      ct_cmargin_x(&input_canvas, &input_canvas, 1, 1);
    }
  }

  size_t input_length = wcslen(model->buffer);
  wchar_t text[input_length + 1];
  text[input_length] = L'\0';

  if (model->secret)
    wmemset(text, L'*', input_length);
  else
    wmemcpy(text, model->buffer, input_length);

  wchar_t caret = L'┃';
  if (model->focus && model->_draw_caret && input_length < model->max_size) {
    if (model->centered_text)
      ct_cwrite_char(&input_canvas,
                  (model->max_size - input_length) / 2 + input_length, 0,
                  caret);
    else
      ct_cwrite_char(&input_canvas, input_length, 0, caret);
  }

  if (model->centered_text)
    ct_cwrite_cc(&input_canvas, text);
  else
    ct_cwrite_ss(&input_canvas, text);

  ct_brush_from(backup);
}

static void ctu_input_handler(void *uncasted_model,
                                 const struct CtEvent *event) {
  struct CursedInput *model = uncasted_model;
  if (!model->focus)
    return;

  size_t length = wcslen(model->buffer);

  if (event->type == START_EVENT)
    ct_event_send_custom_delayed(CTU_LABELED_INPUT_CARET_EVENT, 400, model);

  if (event->type == CUSTOM_EVENT &&
      event->custom_signal == CTU_LABELED_INPUT_CARET_EVENT &&
      event->data == model) {
    model->_draw_caret = !model->_draw_caret;
    ct_event_send_custom_delayed(CTU_LABELED_INPUT_CARET_EVENT, 400, model);
  }

  if (event->type == KEY_EVENT) {
    switch (event->key) {
    case L'\n':
    case L'\r':
      if (length != 0)
        ct_event_send_custom(model->signal_on_submit, model->buffer);
      break;

    case L'\x7f':
    case KEY_BACKSPACE:
    case L'\b':
      if (0 < length)
        model->buffer[length - 1] = L'\0';
      break;

    case L'\f':
    case L'\t':
    case L'\v':
    case L'\0':
      break;

    default:
      if (length >= model->max_size)
        break;
      if (event->key >= KEY_MIN || event->key < 32)
        break;
      model->buffer[length] = event->key;
    }
  }
}

void ctu_input_setup(struct CursedInput *out, attr_t highlight,
                        bool secret, bool border, bool centered,
                        bool centered_text, size_t max_size,
                        int signal_on_submit) {
  out->base.cleanup = _empty_cleanup;
  out->base.handler = ctu_input_handler;
  out->base.render = ctu_input_render;

  out->highlight = highlight;
  out->focus = false;
  out->secret = secret;
  out->centered = centered;
  out->centered_text = centered_text;
  out->border = border;
  out->max_size = max_size > CTU_INPUT_MAX ? CTU_INPUT_MAX : max_size;
  out->signal_on_submit = signal_on_submit;
  wmemset(out->buffer, L'\0', (CTU_INPUT_MAX + 1));

  out->_draw_caret = false;
}

#include <cursed-tea/ui/throbber.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/events.h>
#include <stddef.h>
#include <wchar.h>

struct Throbber {
  const wchar_t *steps;
  const size_t number_of_steps;
  const size_t speed_ms;
};

static const struct Throbber THROBBERS[] = {
    {L"|/-\\", 4, 100},
    {L"⣾⣽⣻⢿⡿⣟⣯⣷", 8, 100},
    {L"⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏", 10, 83},
    {L"⢄⢂⢁⡁⡈⡐⡠", 7, 100},
    {L"█▓▒░", 4, 120},
};

static void ctu_throbber_handler(void *uncasted_model,
                                    const struct CtEvent *event) {
  struct CursedThrobber *model = uncasted_model;

  if (!model->_started || event->type == START_EVENT) {
    model->_started = true;
    ct_event_send_custom_delayed(CTU_THROBBER_TICK,
                              THROBBERS[model->style].speed_ms, model);
    return;
  }

  if (!(event->type == CUSTOM_EVENT &&
        event->custom_signal == CTU_THROBBER_TICK &&
        event->data == uncasted_model))
    return;

  if (model->_previous_style != model->style) {
    model->_current_step = (size_t)-1;
    model->_previous_style = model->style;
  }

  model->_current_step =
      (model->_current_step + 1) % THROBBERS[model->style].number_of_steps;
  ct_event_send_custom_delayed(CTU_THROBBER_TICK,
                            THROBBERS[model->style].speed_ms, model);
}

static void ctu_throbber_render(const void *uncasted_model,
                                   struct CtCanvas *canvas) {
  const struct CursedThrobber *model = uncasted_model;
  ct_cwrite_char(canvas, 0, 0,
              THROBBERS[model->style].steps[model->_current_step]);
}

void ctu_throbber_setup(struct CursedThrobber *model,
                           enum Throbbers style) {
  model->style = style;
  model->_current_step = 0;
  model->_started = false;
  model->_previous_style = style;
  model->base.render = ctu_throbber_render;
  model->base.handler = ctu_throbber_handler;
  model->base.cleanup = _empty_cleanup;
}

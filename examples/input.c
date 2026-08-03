#include <cursed-tea/core.h>
#include <cursed-tea/ui/input.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#define CE_SUBMIT 0x1337

typedef struct Root {
  struct CtModel base;
  struct CursedInput input;
} Root;

void cleanup(void *uncasted_model) {}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas aux;
  ct_cborder(canvas, &aux);
  ct_cwrite_ce(canvas, L"[ ESC: to exit ]");
  model->input.base.render(&model->input, &aux);
}

void handle(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type == KEY_EVENT && event->key == L'\033') // ESC key
    ct_event_send_exit();

  if (event->type == CUSTOM_EVENT && event->custom_signal == CE_SUBMIT) {
    log_debug(L"received xixa from input");
    log_fmt(LOG_DEBUG, L"%ls", (wchar_t *)event->data);
    wmemset(event->data, L'\0', wcslen((wchar_t *)event->data));
    model->input.focus = false;
  }

  model->input.base.handler(&model->input, event);
}

void root_setup(Root *root) {
  root->base.cleanup = cleanup;
  root->base.handler = handle;
  root->base.render = render;
  ctu_input_setup(&root->input,
                  A_BOLD,     // HIGHLIGHT
                  false,      // SECRET
                  false,      // Border
                  false,      // Center
                  false,      // Center text
                  21,         // max input
                  CE_SUBMIT); // Signal code
  root->input.focus = true;
}

int main() {
  Root root;

  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);

  return EXIT_SUCCESS;
}

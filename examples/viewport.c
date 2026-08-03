#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/viewport.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

#define TALL_CHILD_LINE_COUNT 80

typedef struct TallChild {
  struct CtModel base;
} TallChild;

typedef struct Root {
  struct CtModel base;
  struct CursedViewport viewport;
  TallChild child;
} Root;

void tall_child_render(const void *uncasted_model, struct CtCanvas *canvas) {
  (void)uncasted_model;

  for (size_t line_index = 0; line_index < TALL_CHILD_LINE_COUNT;
       line_index++) {
    wchar_t line_text[96];
    swprintf(line_text, 96, L"Line %02zu: viewport child content row",
             line_index + 1);
    if (line_index >= canvas->max_y)
      break;
    size_t column_limit = canvas->max_x;
    if (column_limit > 0 && wcslen(line_text) > column_limit)
      line_text[column_limit] = L'\0';

    ct_cwrite(canvas, 0, line_index, line_text);
  }
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas inner;
  wchar_t status_line[128];

  ct_cborder(canvas, NULL);
  ct_ccutout_c(canvas, &inner, 76, 27);
  ct_cborder(&inner, &inner);

  ct_cwrite_cs(canvas, L"[ VIEWPORT ]");
  swprintf(status_line, 128, L"[ j/k scroll  q quit  off=%zu/%zu ]",
           model->viewport._offset, model->viewport._max_offset);
  ct_cwrite_ce(canvas, status_line);

  model->viewport.base.render(&model->viewport, &inner);
}

void handle(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type == KEY_EVENT && (event->key == L'q' || event->key == L'Q')) {
    ct_event_send_exit();
    return;
  }

  model->viewport.base.handler(&model->viewport, event);
}

void cleanup(void *uncasted_model) {
  struct Root *root = uncasted_model;
  root->viewport.base.cleanup(&root->viewport);
}

void root_setup(struct Root *root) {
  root->base.cleanup = cleanup;
  root->base.render = render;
  root->base.handler = handle;

  root->child.base.cleanup = _empty_cleanup;
  root->child.base.render = tall_child_render;
  root->child.base.handler = _empty_handler;

  ctu_viewport_setup(&root->viewport, &root->child.base);
}

int main(void) {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);
  return EXIT_SUCCESS;
}

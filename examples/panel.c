#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/label.h>
#include <cursed-tea/ui/panel.h>
#include <stdlib.h>

typedef struct Root {
  struct CtModel base;
  struct CursedPanel panel;
  struct CursedLabel child;
} Root;

static const wchar_t *border_name(enum CtBorderStyles style) {
  switch (style) {
  case B_NONE:
    return L"none";
  case B_NORMAL:
    return L"normal";
  case B_DOUBLE:
    return L"double";
  case B_CURVED:
    return L"curved";
  case B_BLOCK:
    return L"block";
  case B_HALFBLOCK_OUTER:
    return L"half-outer";
  case B_HALFBLOCK_INNER:
    return L"half-inner";
  case B_THICK:
    return L"thick";
  case B_ASCII:
    return L"ascii";
  }
  return L"?";
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas body, panel_region;
  wchar_t status_line[128];

  ct_cborder(canvas, NULL);
  ct_cwrite_cs(canvas, L"[ PANEL ]");
  ct_cwrite_ce(canvas, L"[ b:border  t:title  q:exit ]");

  ct_cmargin(canvas, &body, 2, 2, 2, 2);
  if (body.max_x < 10 || body.max_y < 5)
    return;

  ct_ccutout_c(&body, &panel_region,
               body.max_x > 4 ? body.max_x - 2 : body.max_x,
               body.max_y > 4 ? body.max_y - 3 : body.max_y);
  model->panel.base.render(&model->panel, &panel_region);

  swprintf(status_line, 128, L" border=%ls  title=%ls ",
           border_name(model->panel.border),
           model->panel.draw_title ? L"on" : L"off");
  ct_brush_fg(160, 170, 190);
  ct_cwrite_ce(&body, status_line);
  ct_brush_clean();
}

void handle(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type != KEY_EVENT)
    return;

  switch (event->key) {
  case L'q':
  case L'Q':
    ct_event_send_exit();
    break;
  case L'b':
  case L'B':
    model->panel.border =
        (enum CtBorderStyles)(((model->panel.border + 2) % 8) - 1);
    break;
  case L't':
  case L'T':
    model->panel.draw_title = !model->panel.draw_title;
    break;
  default:
    model->panel.base.handler(&model->panel, event);
    break;
  }
}

void root_setup(Root *root) {
  root->base.cleanup = _empty_cleanup;
  root->base.render = render;
  root->base.handler = handle;

  ctu_panel_setup(&root->panel, B_NORMAL, L"PANEL", &root->child.base);

  ctu_label_setup(&root->child, L"Hello from panel child", false, true, 1, 1);
}

int main(void) {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);
  return EXIT_SUCCESS;
}

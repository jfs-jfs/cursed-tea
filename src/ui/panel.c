#include <cursed-tea/ui/panel.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/label.h>
#include <stdbool.h>
#include <wchar.h>

void ctu_panel_cleanup(void *uncasted_model) {
  struct CursedPanel *model = uncasted_model;
  model->child->cleanup(model->child);
}

void ctu_panel_render(const void *uncasted_model, struct CtCanvas *canvas) {

  const struct CursedPanel *model = uncasted_model;
  struct CtCanvas inner;
  struct CtBrush backup = ct_brush();

  ct_brush_border(model->border);
  ct_cborder(canvas, &inner);
  ct_brush_from(backup);

  model->child->render(model->child, &inner);

  if (!model->draw_title)
    return;

  ct_cmargin_x(canvas, &inner, 0, 2);
  model->title.base.render(&model->title, &inner);
}

void ctu_panel_handler(void *uncasted_model, const struct CtEvent *event) {
  struct CursedPanel *model = uncasted_model;
  model->child->handler(model->child, event);
}

void ctu_panel_setup(struct CursedPanel *model,
                        const enum CtBorderStyles border, const wchar_t *title,
                        struct CtModel *child) {

  model->base.cleanup = ctu_panel_cleanup;
  model->base.handler = ctu_panel_handler;
  model->base.render = ctu_panel_render;

  model->border = border;
  model->child = child;
  if (NULL == title) {
    model->draw_title = false;
    return;
  }

  model->draw_title = true;
  wchar_t buffer1[CTU_LABEL_MAX_SIZE + 1 - 4];
  clamp_string(buffer1, title, CTU_LABEL_MAX_SIZE - 4);

  wchar_t buffer2[CTU_LABEL_MAX_SIZE + 1];
  swprintf(buffer2, CTU_LABEL_MAX_SIZE, L"[ %ls ]", buffer1);
  ctu_label_setup(&model->title, buffer2, false, false, 0, 0);
}

/*orcale made*/
#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/status.h>

typedef struct Root {
  struct CtModel base;
  struct CursedStatus status;
} Root;

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;

  ct_cborder(canvas, NULL);
  ct_cwrite_cs(canvas, L"[ STATUS ]");
  ct_cwrite_ce(canvas, L"[ V:vertical/horizontal  Q:exit ]");

  model->status.base.render(&model->status, canvas);
}

void handle(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type != KEY_EVENT)
    return;

  if (event->key == L'q')
    ct_event_send_exit();
  else if (event->key == L'v' || event->key == L'V')
    model->status.veritcal = !model->status.veritcal;
}

void root_setup(Root *root) {
  root->base.cleanup = _empty_cleanup;
  root->base.render = render;
  root->base.handler = handle;
  ctu_status_setup(&root->status, false, L"[ TS ]", L"[ TC ]", L"[ TE ]",
                   L"[ BS ]", L"[ BC ]", L"[ BE ]");
}

int main() {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);
  return 0;
}

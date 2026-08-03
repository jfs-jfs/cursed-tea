#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/size-guard.h>
#include <stdlib.h>

typedef struct Child {
  struct CtModel base;
} Child;

typedef struct Root {
  struct CursedSizeGuard guard;
  Child child;
} Root;

void child_render(const void *uncasted_model, struct CtCanvas *canvas) {
  (void)uncasted_model;
  ct_cborder(canvas, NULL);
  ct_cwrite_cs(canvas, L"[ SIZE GUARD ]");
  ct_cwrite_cc(canvas, L"Terminal is large enough (min 100x40).");
  ct_cwrite_ce(canvas, L"[ q: quit ]");
}

void child_handle(void *uncasted_model, const struct CtEvent *event) {
  (void)uncasted_model;
  if (event->type == KEY_EVENT && (event->key == L'q' || event->key == L'Q'))
    ct_event_send_exit();
}

void root_setup(Root *root) {
  root->child.base.cleanup = _empty_cleanup;
  root->child.base.render = child_render;
  root->child.base.handler = child_handle;

  ctu_size_guard_setup(&root->guard, 100, 40, &root->child.base);
}

int main(void) {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.guard.base);
  return EXIT_SUCCESS;
}

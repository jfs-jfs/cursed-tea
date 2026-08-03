#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/scrollable-area.h>
#include <stdlib.h>

typedef struct Root {
  struct CtModel base;
  struct CursedScrollableArea area;
} Root;

static const wchar_t *SAMPLE_TEXT =
    L"Line 01: The quick brown fox jumps over the lazy dog.\n"
    L"Line 02: Pack my box with five dozen liquor jugs.\n"
    L"Line 03: How vexingly quick daft zebras jump!\n"
    L"Line 04: Sphinx of black quartz, judge my vow.\n"
    L"Line 05: The five boxing wizards jump quickly.\n"
    L"Line 06: Jackdaws love my big sphinx of quartz.\n"
    L"Line 07: This line is intentionally very very very very very very very "
    L"very very very very very very very very very very very very very very "
    L"very very very very long "
    L"to demonstrate horizontal clipping past the canvas width.\n"
    L"Line 08: Bright vixens jump; dozy fowl quack.\n"
    L"Line 09: Quick zephyrs blow, vexing daft Jim.\n"
    L"Line 10: Two driven jocks help fax my big quiz.\n"
    L"Line 11: Waltz, bad nymph, for quick jigs vex.\n"
    L"Line 12: Glib jocks quiz nymph to vex dwarf.\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"A lot more lines\n"
    L"The end of the scrollable sample text.";

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas inner;

  ct_cborder(canvas, &inner);
  ct_cwrite_cs(canvas, L"[ SCROLLABLE AREA ]");
  ct_cwrite_ce(canvas, L"[ j/k: scroll  q: quit ]");

  model->area.base.render(&model->area, &inner);
}

void handle(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type == KEY_EVENT && event->key == L'q') {
    ct_event_send_exit();
    return;
  }

  // Forward everything else (j/k scrolling) to the scrollable area.
  model->area.base.handler(&model->area, event);
}

void cleanup(void *uncasted_model) {
  struct Root *root = uncasted_model;
  root->area.base.cleanup(&root->area);
}

void root_setup(struct Root *root) {
  root->base.cleanup = cleanup;
  root->base.render = render;
  root->base.handler = handle;

  ctu_scrollable_area_setup(&root->area, SAMPLE_TEXT);
}

int main(void) {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);
  return EXIT_SUCCESS;
}

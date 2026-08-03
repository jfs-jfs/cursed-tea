#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <stdbool.h>
#include <stdlib.h>

struct RootModel {
  struct CtModel bmodel;
  int counter;
};

void root_cleanup(void *model) {
  log_debug(L"root_cleanup");
  free((struct RootModel *)model);
}

void root_render(const void *bmodel, struct CtCanvas *canvas) {

  int display = ((struct RootModel *)bmodel)->counter;

  log_debug(L"root_render");

  if (display == 0) {
    struct CtCanvas c1, c2, c3;

    ct_cvwsplit(canvas, &c1, &c2, 1, 2);
    ct_cborder(&c1, NULL);
    ct_cwrite_cc(&c1, L"PENE");
    ct_chwsplit(&c2, &c1, &c2, 2, 1);

    ct_cborder(&c1, NULL);
    ct_cwrite_cc(&c1, L"PENE");
    ct_cborder(&c2, NULL);
    ct_cwrite_cc(&c2, L"PENE");

    ct_ccutout_c(canvas, &c3, 50, 10);
    ct_cborder(&c3, NULL);
    ct_brush_fg(255, 80, 80);
    ct_brush_bg(20, 20, 40);
    ct_cwrite_cc(&c3, L"TRUECOLOR");
    ct_brush_clean();
  } else if (display == 1) {
    ct_cwrite(canvas, 1, 1, L"this is a test");
    ct_cwrite_c(canvas, 40, L"CENTER TEST [y=40]");
    ct_cwrite_cs(canvas, L"CENTER START");
    ct_cwrite_cc(canvas, L"CENTER CENTER");
    ct_cwrite_ce(canvas, L"CENTER END");
    ct_cwrite_s(canvas, 20, L"START y=20");
    ct_cwrite_ss(canvas, L"START START");
    ct_cwrite_sc(canvas, L"START CENTER");
    ct_cwrite_se(canvas, L"START END");
    ct_cwrite_e(canvas, 20, L"END y=20");
    ct_cwrite_es(canvas, L"END START");
    ct_cwrite_ec(canvas, L"END CENTER");
    ct_cwrite_ee(canvas, L"END END");
  } else {
    ct_cwritev(canvas, 1, 1, L"this is a test");
    ct_cwritev_c(canvas, 20, L"CENTER TEST [y=20]");
    ct_cwritev_cs(canvas, L"CENTER START");
    ct_cwritev_cc(canvas, L"CENTER CENTER");
    ct_cwritev_ce(canvas, L"CENTER END");
    ct_cwritev_s(canvas, 20, L"START y=20");
    ct_cwritev_ss(canvas, L"START START");
    ct_cwritev_sc(canvas, L"START CENTER");
    ct_cwritev_se(canvas, L"START END");
    ct_cwritev_e(canvas, 20, L"END y=20");
    ct_cwritev_es(canvas, L"END START");
    ct_cwritev_ec(canvas, L"END CENTER");
    ct_cwritev_ee(canvas, L"END END");
  }
}

void root_handler(void *bmodel, const struct CtEvent *event) {
  log_debug(L"root_handler");
  struct RootModel *model = (struct RootModel *)bmodel;
  log_fmt(LOG_DEBUG, L"event type=%d", event->type);
  if (event->type == KEY_EVENT) {
    log_debug(L"EVENT KEY");
    log_fmt(LOG_DEBUG, L"key=%lc", event->key);
    if (event->key == L'q') {
      log_debug(L"EXIT KEY");
      ct_event_send_exit();
    } else if (event->key == L'1') {
      model->counter = 1;
    } else if (event->key == L'2') {
      model->counter = 2;
    } else if (event->key == L'0') {
      model->counter = 0;
    }
  }
}

struct CtModel *root_init() {

  struct RootModel *model = malloc(sizeof(struct RootModel));
  model->counter = 0;
  model->bmodel.render = root_render;
  model->bmodel.cleanup = root_cleanup;
  model->bmodel.handler = root_handler;
  return &model->bmodel;
}

int main(void) {

  ct_app_init();
  ct_app_start(root_init());

  return EXIT_SUCCESS;
}

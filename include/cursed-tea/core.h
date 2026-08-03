#pragma once

#include <cursed-tea/application.h>
#include <cursed-tea/canvas-write.h>
#include <cursed-tea/canvas.h>
#include <cursed-tea/event.h>
#include <cursed-tea/layout.h>
#include <cursed-tea/logger.h>
#include <cursed-tea/style/border.h>
#include <cursed-tea/style/brush.h>
#include <cursed-tea/uvector.h>

#include <ncurses.h>

#include <cursed-tea/helpers.h>

/* ~~[ MODEL */
struct CtModel;

typedef void (*CtModelCleanupFunc)(void *uncasted_model);
typedef void (*CtModelRenderFunc)(const void *uncasted_model,
                                  struct CtCanvas *canvas);
typedef void (*CtModelEventHandlerFunc)(void *uncasted_model,
                                        const struct CtEvent *event);
typedef struct CtModel *(*CtModelSetupFunc)(void);

struct CtModel {
  CtModelEventHandlerFunc handler;
  CtModelRenderFunc render;
  CtModelCleanupFunc cleanup;
};

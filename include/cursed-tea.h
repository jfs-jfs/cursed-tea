#pragma once

// To use cchar_t
#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>

#include <cursed-tea/application.h>
#include <cursed-tea/border.h>
#include <cursed-tea/brush.h>
#include <cursed-tea/canvas-write.h>
#include <cursed-tea/canvas.h>
#include <cursed-tea/event.h>
#include <cursed-tea/layout.h>
#include <cursed-tea/logger.h>
#include <cursed-tea/uvector.h>

struct CtModel;

typedef void (*CtModelCleanupFunc)(void *uncasted_model);
typedef void (*CtModelRenderFunc)(const void *uncasted_model,
                                  struct CtCanvas *canvas);
typedef void (*CtModelEventHandlerFunc)(void *uncasted_model,
                                        const struct CtEvent *event);

struct CtModel {
  CtModelEventHandlerFunc handler;
  CtModelRenderFunc render;
  CtModelCleanupFunc cleanup;
};

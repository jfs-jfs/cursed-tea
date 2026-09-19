#pragma once

#include "cursed-tea/canvas.h"
#include "cursed-tea/event.h"
#include <cursed-tea.h>

inline void ct_subrender(const struct CtModel *model, struct CtCanvas *canvas);
inline void ct_subhandler(struct CtModel *model, const struct CtEvent *event);
inline void ct_subcleanup(struct CtModel *model);

void ct_empty_cleanup(void *uncasted_model);
void ct_empty_handler(void *uncasted_model, const struct CtEvent *event);
void ct_empty_render(const void *uncasted_model, struct CtCanvas *canvas);

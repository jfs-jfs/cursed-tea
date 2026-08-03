#pragma once

#include <cursed-tea/core.h>

void ct_subrender(const struct CtModel *model, struct CtCanvas *canvas);
void ct_subhandler(struct CtModel *model, const struct CtEvent *event);
void ct_subcleanup(struct CtModel *model);

#pragma once

#include <cursed-tea.h>

inline void ct_subrender(const struct CtModel *model, struct CtCanvas *canvas);
inline void ct_subhandler(struct CtModel *model, const struct CtEvent *event);
inline void ct_subcleanup(struct CtModel *model);

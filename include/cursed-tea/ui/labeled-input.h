#pragma once

#include <cursed-tea/core.h>
#include <cursed-tea/ui/input.h>
#include <cursed-tea/ui/label.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

struct CursedLabeledInput {
  struct CtModel base;
  struct CursedInput input;
  struct CursedLabel label;
  bool focus;
  size_t label_size;
};

void ctu_labeled_input_setup(struct CursedLabeledInput *model,
                                const wchar_t *text, bool secret,
                                size_t input_max_size, const size_t label_size,
                                int signal_on_submit);

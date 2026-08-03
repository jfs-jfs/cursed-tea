#pragma once

#include <cursed-tea/core.h>
#include <stddef.h>
#include <wchar.h>

static inline void _empty_cleanup(void *uncasted_model) {
  (void)uncasted_model;
}
static inline void _empty_handler(void *uncasted_model,
                                  const struct CtEvent *event) {
  (void)uncasted_model;
  (void)event;
}

enum Orientation {
  O_UP,
  O_DOWN,
  O_LEFT,
  O_RIGHT,
};

// Assumes destionation has size max_size + 1
static inline void clamp_string(wchar_t *destination, const wchar_t *source,
                                size_t max_size) {
  wmemset(destination, L'\0', max_size + 1);
  if (source == NULL)
    return;

  size_t length = wcslen(source);
  if (length > max_size)
    length = max_size;

  wmemcpy(destination, source, length);
}

#include <cursed-tea/ui/scrollable-area.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

static size_t _count_visual_rows(const wchar_t *text, size_t width) {
  // Consulted Oracle
  if (NULL == text || L'\0' == *text || 0 == width)
    return 0;

  size_t rows = 1; // The first row always exists.
  size_t column = 0;

  for (const wchar_t *cursor = text; L'\0' != *cursor; cursor++) {
    if (L'\n' == *cursor) {
      rows++; // Newline starts a fresh visual row.
      column = 0;
      continue;
    }

    column++;
    if (column == width && L'\0' != *(cursor + 1) && L'\n' != *(cursor + 1)) {
      rows++; // Wrap: the following character spills onto a new row.
      column = 0;
    }
  }

  return rows;
}

void ctu_scrollable_area_cleanup(void *uncasted_model) {
  struct CursedScrollableArea *model = uncasted_model;
  if (NULL != model->_text)
    free(model->_text);
}

void ctu_scrollable_area_render(const void *uncasted_model,
                                   struct CtCanvas *canvas) {

  // Fuck it
  struct CursedScrollableArea *illegal_model = (void *)uncasted_model;
  size_t total_rows = _count_visual_rows(illegal_model->_text, canvas->max_x);
  illegal_model->_max_offset =
      total_rows > canvas->max_y ? total_rows - canvas->max_y : 0;

  const struct CursedScrollableArea *model = uncasted_model;
  if (NULL == model->_text)
    return;

  size_t row = 0;
  size_t column = 0;

  for (const wchar_t *cursor = model->_text; L'\0' != *cursor; cursor++) {
    if (L'\n' == *cursor) {
      row++;
      column = 0;
      continue;
    }

    if (row >= model->_offset) {
      size_t y = row - model->_offset;
      if (y >= canvas->max_y)
        break;
      ct_cwrite_char(canvas, column, y, *cursor);
    }

    column++;
    if (column == canvas->max_x) {
      row++;
      column = 0;
    }
  }
}

void ctu_scrollable_area_handler(void *uncasted_model,
                                    const struct CtEvent *event) {
  struct CursedScrollableArea *model = uncasted_model;

  if (event->type != KEY_EVENT)
    return;

  switch (event->key) {
  case L'j':
    model->_offset = model->_offset >= model->_max_offset ? model->_max_offset
                                                          : model->_offset + 1;
    break;
  case L'k':
    model->_offset = model->_offset <= 0 ? 0 : model->_offset - 1;
    break;
  }
}

void ctu_scrollable_area_setup(struct CursedScrollableArea *model,
                                  const wchar_t *text) {

  model->base.cleanup = ctu_scrollable_area_cleanup;
  model->base.render = ctu_scrollable_area_render;
  model->base.handler = ctu_scrollable_area_handler;

  model->_offset = 0;
  model->_max_offset = 0;

  if (NULL == text) {
    model->_text = NULL;
    return;
  }

  model->_text = malloc((wcslen(text) + 1) * sizeof(wchar_t));
  if (NULL == model->_text)
    log_crit(L"Unable to allocate space for string in scrollabel area");

  wcscpy(model->_text, text);
}

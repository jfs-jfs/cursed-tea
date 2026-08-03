#include <cursed-tea/ui/table.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

// Oracle generated~ then refactored for styles and memory arena

#define CURSED_TABLE_HEADER_ROWS 2 // Header labels + separator line.

struct CursedTableStyle {
  const wchar_t column_separator, header_line, header_join;
  const size_t column_gap;
};

static const struct CursedTableStyle CURSED_TABLE_STYLES[] = {
    // CT_NORMAL
    {L'│', L'─', L'┼', 1},
    // CT_DOUBLE
    {L'║', L'═', L'╬', 1},
    // CT_BLOCK
    {L'█', L'█', L'█', 1},
    // CT_THICK
    {L'┃', L'━', L'╋', 1},
    // CT_ASCII
    {L'|', L'-', L'+', 1},
    // CT_MODERN
    {L' ', L'─', L'─', 0},
    // CT_MODERN_THICK
    {L' ', L'━', L'━', 0},
};

static const wchar_t *_safe_string(const wchar_t *source) {
  return NULL == source ? L"" : source;
}

static void ctu_table_cleanup(void *uncasted_model) {
  struct CursedTable *model = uncasted_model;

  // Everything lives in one arena whose base is the columns array.
  free(model->columns);

  model->columns = NULL;
  model->rows = NULL;
  model->_column_widths = NULL;
}

static void _render_text_row(struct CtCanvas *canvas, const size_t canvas_row,
                             wchar_t **cells, const size_t column_count,
                             const size_t *column_widths,
                             const struct CursedTableStyle style) {
  size_t cursor_x = 0;

  for (size_t column_index = 0; column_index < column_count; column_index++) {
    const wchar_t *cell = cells[column_index];

    for (size_t character_index = 0; L'\0' != cell[character_index];
         character_index++) {
      if (cursor_x + character_index >= canvas->max_x)
        return;
      ct_cwrite_char(canvas, cursor_x + character_index, canvas_row,
                  cell[character_index]);
    }

    cursor_x += column_widths[column_index] + style.column_gap;
    if (column_index + 1 == column_count)
      break;

    if (cursor_x >= canvas->max_x)
      return;
    ct_cwrite_char(canvas, cursor_x, canvas_row, style.column_separator);
    cursor_x += 1 + style.column_gap;
  }
}

static void _render_separator_row(struct CtCanvas *canvas,
                                  const size_t canvas_row,
                                  const size_t column_count,
                                  const size_t *column_widths,
                                  const struct CursedTableStyle style) {
  size_t cursor_x = 0;

  for (size_t column_index = 0; column_index < column_count; column_index++) {
    size_t segment_width = column_widths[column_index] + style.column_gap;

    for (size_t segment_index = 0; segment_index < segment_width;
         segment_index++) {
      if (cursor_x + segment_index >= canvas->max_x)
        return;
      ct_cwrite_char(canvas, cursor_x + segment_index, canvas_row,
                  style.header_line);
    }

    cursor_x += segment_width;
    if (column_index + 1 == column_count)
      break;

    if (cursor_x >= canvas->max_x)
      return;
    ct_cwrite_char(canvas, cursor_x, canvas_row, style.header_join);
    cursor_x++;

    if (cursor_x >= canvas->max_x)
      return;
    ct_cwrite_char(canvas, cursor_x, canvas_row, style.header_line);
    cursor_x++;
  }
}

static void ctu_table_render(const void *uncasted_model,
                                struct CtCanvas *canvas) {
  const struct CursedTable *model = uncasted_model;
  if (0 == model->column_count || NULL == model->columns ||
      canvas->max_y <= CURSED_TABLE_HEADER_ROWS)
    return;

  size_t visible_row_count = canvas->max_y - CURSED_TABLE_HEADER_ROWS;

  // Render receives a const model, but the scroll limit depends on the
  // canvas size which is only known here.
  struct CursedTable *mutable_model = (void *)uncasted_model;
  mutable_model->_max_row_offset = model->row_count > visible_row_count
                                       ? model->row_count - visible_row_count
                                       : 0;
  if (mutable_model->_row_offset > mutable_model->_max_row_offset)
    mutable_model->_row_offset = mutable_model->_max_row_offset;

  struct CursedTableStyle style = CURSED_TABLE_STYLES[model->style];
  _render_text_row(canvas, 0, model->columns, model->column_count,
                   model->_column_widths, style);
  _render_separator_row(canvas, 1, model->column_count, model->_column_widths,
                        style);

  for (size_t visible_index = 0; visible_index < visible_row_count;
       visible_index++) {
    size_t row_index = model->_row_offset + visible_index;
    if (row_index >= model->row_count)
      break;
    _render_text_row(canvas, CURSED_TABLE_HEADER_ROWS + visible_index,
                     model->rows[row_index], model->column_count,
                     model->_column_widths, style);
  }
}

static void ctu_table_handler(void *uncasted_model,
                                 const struct CtEvent *event) {
  struct CursedTable *model = uncasted_model;

  if (event->type != KEY_EVENT)
    return;

  switch (event->key) {
  case L'j':
  case KEY_DOWN:
    model->_row_offset = model->_row_offset >= model->_max_row_offset
                             ? model->_max_row_offset
                             : model->_row_offset + 1;
    break;
  case L'k':
  case KEY_UP:
    model->_row_offset = model->_row_offset == 0 ? 0 : model->_row_offset - 1;
    break;
  }
}

// Arena layout (single malloc, single free):
//   [ columns:        column_count             * wchar_t*  ]
//   [ rows:           row_count                * wchar_t** ]
//   [ row cells:      row_count * column_count * wchar_t*  ]
//   [ column widths:  column_count             * size_t    ]
//   [ string data:    every header and cell, NUL separated ]
// Pointer and size_t segments come first so the wchar_t data that follows
// never breaks their alignment.
void ctu_table_setup(struct CursedTable *model,
                        const enum CursedTableStyles style,
                        const size_t column_count, const size_t row_count,
                        const wchar_t **columns, const wchar_t ***rows) {
  model->base.cleanup = ctu_table_cleanup;
  model->base.render = ctu_table_render;
  model->base.handler = ctu_table_handler;

  model->style = style;
  model->column_count = column_count;
  model->row_count = row_count;
  model->_row_offset = 0;
  model->_max_row_offset = 0;
  model->columns = NULL;
  model->rows = NULL;
  model->_column_widths = NULL;

  if (0 == column_count)
    return;

  // First pass: measure everything.
  size_t character_count = 0;
  for (size_t column_index = 0; column_index < column_count; column_index++) {
    const wchar_t *header =
        NULL == columns ? L"" : _safe_string(columns[column_index]);
    character_count += wcslen(header) + 1;
  }
  for (size_t row_index = 0; row_index < row_count; row_index++)
    for (size_t column_index = 0; column_index < column_count; column_index++) {
      const wchar_t *cell =
          NULL == rows ? L"" : _safe_string(rows[row_index][column_index]);
      character_count += wcslen(cell) + 1;
    }

  size_t columns_bytes = column_count * sizeof(wchar_t *);
  size_t rows_bytes = row_count * sizeof(wchar_t **);
  size_t cells_bytes = row_count * column_count * sizeof(wchar_t *);
  size_t widths_bytes = column_count * sizeof(size_t);
  size_t strings_bytes = character_count * sizeof(wchar_t);

  char *arena = malloc(columns_bytes + rows_bytes + cells_bytes + widths_bytes +
                       strings_bytes);
  if (NULL == arena) {
    log_crit(L"Unable to allocate arena for table");
    return;
  }

  // Second pass: carve the arena and copy everything into it.
  model->columns = (wchar_t **)arena;
  model->rows = (wchar_t ***)(arena + columns_bytes);
  wchar_t **cell_pointers = (wchar_t **)(arena + columns_bytes + rows_bytes);
  model->_column_widths =
      (size_t *)(arena + columns_bytes + rows_bytes + cells_bytes);
  wchar_t *string_cursor = (wchar_t *)(arena + columns_bytes + rows_bytes +
                                       cells_bytes + widths_bytes);

  for (size_t column_index = 0; column_index < column_count; column_index++) {
    const wchar_t *header =
        NULL == columns ? L"" : _safe_string(columns[column_index]);
    size_t header_length = wcslen(header);

    model->columns[column_index] = string_cursor;
    model->_column_widths[column_index] = header_length;
    wcscpy(string_cursor, header);
    string_cursor += header_length + 1;
  }

  for (size_t row_index = 0; row_index < row_count; row_index++) {
    model->rows[row_index] = cell_pointers + row_index * column_count;

    for (size_t column_index = 0; column_index < column_count; column_index++) {
      const wchar_t *cell =
          NULL == rows ? L"" : _safe_string(rows[row_index][column_index]);
      size_t cell_length = wcslen(cell);

      model->rows[row_index][column_index] = string_cursor;
      if (cell_length > model->_column_widths[column_index])
        model->_column_widths[column_index] = cell_length;
      wcscpy(string_cursor, cell);
      string_cursor += cell_length + 1;
    }
  }
}

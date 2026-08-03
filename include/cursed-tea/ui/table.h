#pragma once

#include <cursed-tea/core.h>
#include <stddef.h>
#include <wchar.h>

enum CursedTableStyles {
  CT_NORMAL,
  CT_DOUBLE,
  CT_BLOCK,
  CT_THICK,
  CT_ASCII,
  CT_MODERN,
  CT_MODERN_THICK,
};

struct CursedTable {
  struct CtModel base;
  enum CursedTableStyles style;
  wchar_t **columns;
  wchar_t ***rows;
  size_t column_count;
  size_t row_count;
  size_t *_column_widths;
  size_t _row_offset, _max_row_offset;
};

void ctu_table_setup(struct CursedTable *model,
                        const enum CursedTableStyles style,
                        const size_t column_count, const size_t row_count,
                        const wchar_t **columns, const wchar_t ***rows);

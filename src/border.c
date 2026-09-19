#include <cursed-tea/border.h>
#include <cursed-tea/brush.h>
#include <cursed-tea/canvas-write.h>
#include <stddef.h>
#include <string.h>

static const struct CtBorderStyle DEFAULT_BORDER_STYLES[] = {
    // NORMAL BORDER
    {
        L'─',
        L'─',
        L'│',
        L'│',
        L'┌',
        L'┐',
        L'└',
        L'┘',
    },
    // DOUBLE BORDER
    {
        L'═',
        L'═',
        L'║',
        L'║',
        L'╔',
        L'╗',
        L'╚',
        L'╝',
    },
    // CURVED BORDER
    {
        L'─',
        L'─',
        L'│',
        L'│',
        L'╭',
        L'╮',
        L'╰',
        L'╯',
    },
    // BLOCK BORDER
    {
        L'█',
        L'█',
        L'█',
        L'█',
        L'█',
        L'█',
        L'█',
        L'█',
    },
    // HALFBLOCK OUTER BORDER
    {
        L'▀',
        L'▄',
        L'▌',
        L'▐',
        L'▛',
        L'▜',
        L'▙',
        L'▟',
    },
    // HALFBLOCK INNER BORDER
    {
        L'▄',
        L'▀',
        L'▐',
        L'▌',
        L'▗',
        L'▖',
        L'▝',
        L'▘',
    },
    // THICK BORDER
    {
        L'━',
        L'━',
        L'┃',
        L'┃',
        L'┏',
        L'┓',
        L'┗',
        L'┛',
    },
    // ASCII BORDER
    {
        L'-',
        L'-',
        L'|',
        L'|',
        L'+',
        L'+',
        L'+',
        L'+',
    },
    // MODERN BORDER
    {
        L' ',
        L' ',
        L' ',
        L' ',
        L'╋',
        L'╋',
        L'╋',
        L'╋',
    },
};

void ct_cborder_custom(struct CtCanvas *canvas, struct CtCanvas *after_border,
                       const wchar_t top, const wchar_t bottom,
                       const wchar_t left, const wchar_t right,
                       const wchar_t top_left, const wchar_t top_right,
                       const wchar_t bottom_left, const wchar_t bottom_right) {
  if (canvas->max_x <= 2 || canvas->max_y <= 2) {
    log_error(L"Too small of a canvas for a border");
    ct_canvas_copy(canvas, after_border);
    return;
  }
  ct_cwrite_char(canvas, 0, 0, top_left);
  ct_cwrite_char(canvas, canvas->max_x - 1, 0, top_right);
  ct_cwrite_char(canvas, 0, canvas->max_y - 1, bottom_left);
  ct_cwrite_char(canvas, canvas->max_x - 1, canvas->max_y - 1, bottom_right);

  ct_cwrite_char_line(canvas, 1, 0, canvas->max_x - 2, 0, top);
  ct_cwrite_char_line(canvas, 1, canvas->max_y - 1, canvas->max_x - 2,
                      canvas->max_y - 1, bottom);
  ct_cwrite_char_line(canvas, 0, 1, 0, canvas->max_y - 2, left);
  ct_cwrite_char_line(canvas, canvas->max_x - 1, 1, canvas->max_x - 1,
                      canvas->max_y - 2, right);

  if (NULL == after_border)
    return;

  after_border->max_x = canvas->max_x - 2;
  after_border->max_y = canvas->max_y - 2;
  after_border->size = after_border->max_x * after_border->max_y;
  after_border->screen_cells = NULL;
  for (size_t i = 0; i < after_border->max_y; i++)
    after_border->cell_matrix[i] = canvas->cell_matrix[i + 1] + 1;
}

void ct_cborder(struct CtCanvas *canvas, struct CtCanvas *after_border) {
  enum CtBorderStyles style = ct_brush().border_style;
  if (style == B_NONE) {
    ct_canvas_copy(canvas, after_border);
    return;
  }
  struct CtBorderStyle bstyle = DEFAULT_BORDER_STYLES[style];
  ct_cborder_custom(canvas, after_border, bstyle.top, bstyle.bottom,
                    bstyle.left, bstyle.right, bstyle.top_left,
                    bstyle.top_right, bstyle.bottom_left, bstyle.bottom_right);
}

struct CtBorderStyle ct_border_from(const enum CtBorderStyles style) {
  if (style == B_NONE) {
    log_error(L"B_NONE has no style! Returning empty");
    return (struct CtBorderStyle){
        L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ',
    };
  }
  return DEFAULT_BORDER_STYLES[style];
}

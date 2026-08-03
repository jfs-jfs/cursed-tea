#include <cursed-tea/ui/common.h>
#include <cursed-tea/ui/line.h>

static void ctu_line_render(const void *uncasted_model,
                            struct CtCanvas *canvas) {
  const struct CursedLine *model = uncasted_model;
  wchar_t line_character;
  size_t start_row, start_column, end_column, end_row;

  if (0 == canvas->max_x || 0 == canvas->max_y)
    return;

  switch (model->orientation) {
  case O_UP:
    start_column = 0;
    start_row = 0;
    end_column = canvas->max_x - 1;
    end_row = 0;
    line_character = ct_border_from(model->type).top;
    break;
  case O_DOWN:
    start_column = 0;
    start_row = canvas->max_y - 1;
    end_column = canvas->max_x - 1;
    end_row = canvas->max_y - 1;
    line_character = ct_border_from(model->type).bottom;
    break;
  case O_LEFT:
    start_column = 0;
    start_row = 0;
    end_column = 0;
    end_row = canvas->max_y - 1;
    line_character = ct_border_from(model->type).left;
    break;
  // case O_RIGHT:
  default:
    start_column = canvas->max_x - 1;
    start_row = 0;
    end_column = canvas->max_x - 1;
    end_row = canvas->max_y - 1;
    line_character = ct_border_from(model->type).right;
    break;
  }

  ct_cwrite_char_line(canvas, start_column, start_row, end_column, end_row,
                      line_character);
}

void ctu_line_setup(struct CursedLine *model, enum CtBorderStyles style,
                    enum Orientation orientation) {
  model->base.cleanup = _empty_cleanup;
  model->base.handler = _empty_handler;
  model->base.render = ctu_line_render;
  model->orientation = orientation;
  model->type = style;
}

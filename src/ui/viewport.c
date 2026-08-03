#include <cursed-tea/ui/viewport.h>
#include <stddef.h>

void ctu_viewport_handler(void *uncasted_model, const struct CtEvent *event) {

  struct CursedViewport *model = uncasted_model;
  if (model->_max_offset != 0 && event->type == KEY_EVENT) {
    switch (event->key) {
    case L'j':
      model->_offset = model->_offset >= model->_max_offset
                           ? model->_max_offset
                           : model->_offset + 1;
      break;
    case L'k':
      model->_offset = model->_offset == 0 ? 0 : model->_offset - 1;
      break;
    }
  }

  model->child->handler(model->child, event);
}

void ctu_viewport_render(const void *uncasted_model, struct CtCanvas *canvas) {
  struct CursedViewport *illegal_model = (void *)uncasted_model;
  const struct CursedViewport *model = uncasted_model;

  struct CtCanvas parallel;
  ct_parallel_of_width(&parallel, canvas->max_x);
  model->child->render(model->child, &parallel);

  size_t child_height = ct_canvas_content_height(&parallel);
  if (child_height > canvas->max_y)
    illegal_model->_max_offset = child_height - canvas->max_y;
  else
    illegal_model->_max_offset = 0;

  if (illegal_model->_offset > illegal_model->_max_offset)
    illegal_model->_offset = illegal_model->_max_offset;

  for (size_t row_index = 0; row_index < canvas->max_y; row_index++) {
    size_t source_row = model->_offset + row_index;
    for (size_t column_index = 0; column_index < canvas->max_x;
         column_index++) {
      canvas->cell_matrix[row_index][column_index] =
          parallel.cell_matrix[source_row][column_index];
    }
  }
  ct_parallel_canvas_clear();
}

void ctu_viewport_cleanup(void *uncasted_model) {
  struct CursedViewport *model = uncasted_model;
  model->child->cleanup(model->child);
}

void ctu_viewport_setup(struct CursedViewport *model,
                           struct CtModel *child) {

  model->base.cleanup = ctu_viewport_cleanup;
  model->base.handler = ctu_viewport_handler;
  model->base.render = ctu_viewport_render;

  model->_offset = 0;
  model->_max_offset = 0;
  model->child = child;
}

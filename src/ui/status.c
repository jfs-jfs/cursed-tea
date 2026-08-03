#include <cursed-tea/ui/status.h>
#include <cursed-tea/ui/common.h>

static void ctu_status_render(const void *uncasted_model,
                                 struct CtCanvas *canvas) {
  const struct CursedStatus *model = uncasted_model;

  struct CtCanvas aux;
  if (model->veritcal) {
    ct_cmargin_y(canvas, &aux, 1, 1);
    if (L'\0' != *model->label_ts)
      ct_cwritev_ss(&aux, model->label_ts);
    if (L'\0' != *model->label_tc)
      ct_cwritev_cs(&aux, model->label_tc);
    if (L'\0' != *model->label_te)
      ct_cwritev_es(&aux, model->label_te);
    if (L'\0' != *model->label_bs)
      ct_cwritev_se(&aux, model->label_bs);
    if (L'\0' != *model->label_bc)
      ct_cwritev_ce(&aux, model->label_bc);
    if (L'\0' != *model->label_be)
      ct_cwritev_ee(&aux, model->label_be);
  } else {
    ct_cmargin_x(canvas, &aux, 1, 1);
    if (L'\0' != *model->label_ts)
      ct_cwrite_ss(&aux, model->label_ts);
    if (L'\0' != *model->label_tc)
      ct_cwrite_cs(&aux, model->label_tc);
    if (L'\0' != *model->label_te)
      ct_cwrite_es(&aux, model->label_te);
    if (L'\0' != *model->label_bs)
      ct_cwrite_se(&aux, model->label_bs);
    if (L'\0' != *model->label_bc)
      ct_cwrite_ce(&aux, model->label_bc);
    if (L'\0' != *model->label_be)
      ct_cwrite_ee(&aux, model->label_be);
  }
}

void ctu_status_setup(struct CursedStatus *model, bool vertical,
                         const wchar_t *label_ts, const wchar_t *label_tc,
                         const wchar_t *label_te, const wchar_t *label_bs,
                         const wchar_t *label_bc, const wchar_t *label_be) {
  model->base.cleanup = _empty_cleanup;
  model->base.handler = _empty_handler;
  model->base.render = ctu_status_render;
  model->veritcal = vertical;

  if (label_ts != NULL)
    clamp_string(model->label_ts, label_ts, CTU_STATUS_MAX_SIZE);
  if (label_tc != NULL)
    clamp_string(model->label_tc, label_tc, CTU_STATUS_MAX_SIZE);
  if (label_te != NULL)
    clamp_string(model->label_te, label_te, CTU_STATUS_MAX_SIZE);
  if (label_bs != NULL)
    clamp_string(model->label_bs, label_bs, CTU_STATUS_MAX_SIZE);
  if (label_bc != NULL)
    clamp_string(model->label_bc, label_bc, CTU_STATUS_MAX_SIZE);
  if (label_be != NULL)
    clamp_string(model->label_be, label_be, CTU_STATUS_MAX_SIZE);
}

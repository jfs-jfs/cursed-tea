#pragma once

#include <cursed-tea/canvas.h>
#include <stddef.h>

void ct_cvsplit(const struct CtCanvas *canvas, struct CtCanvas *left,
             struct CtCanvas *right);
void ct_chsplit(const struct CtCanvas *canvas, struct CtCanvas *top,
             struct CtCanvas *bottom);

void ct_cvwsplit(const struct CtCanvas *canvas, struct CtCanvas *left,
              struct CtCanvas *right, const size_t weight_left,
              const size_t weight_right);
void ct_chwsplit(const struct CtCanvas *canvas, struct CtCanvas *top,
              struct CtCanvas *bottom, const size_t weight_top,
              const size_t weight_bottom);

void ct_ccutout(const struct CtCanvas *canvas, struct CtCanvas *cutout,
             const size_t from_x, const size_t from_y, const size_t to_x,
             const size_t to_y);

void ct_ccutout_c(const struct CtCanvas *canvas, struct CtCanvas *cutout,
               const size_t x, const size_t y);

void ct_cmargin(const struct CtCanvas *canvas, struct CtCanvas *out, const size_t top,
             const size_t right, const size_t bottom, const size_t left);

void ct_cmargin_y(const struct CtCanvas *canvas, struct CtCanvas *out,
               const size_t top, const size_t bottom);

void ct_cmargin_x(const struct CtCanvas *canvas, struct CtCanvas *out,
               const size_t right, const size_t left);

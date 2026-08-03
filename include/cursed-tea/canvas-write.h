#pragma once

#include <cursed-tea/canvas.h>
#include <cursed-tea/logger.h>
#include <cursed-tea/uvector.h>
#include <stddef.h>
#include <wchar.h>

// Horizontally
void ct_cwrite(struct CtCanvas *canvas, const size_t x, const size_t y,
            const wchar_t *text);
// Vertically
void ct_cwritev(struct CtCanvas *canvas, const size_t x, const size_t y,
             const wchar_t *text);

void ct_cwrite_char_line(struct CtCanvas *canvas, const size_t from_x,
                      const size_t from_y, const size_t to_x, const size_t to_y,
                      const wchar_t ch);
void ct_cwrite_char(struct CtCanvas *canvas, const size_t x, const size_t y,
                 const wchar_t ch);

void ct_cfill(struct CtCanvas *canvas, const wchar_t character);

// S -> start // C -> center // E -> end
void ct_cwrite_s(struct CtCanvas *canvas, const size_t y, const wchar_t *text);
void ct_cwrite_c(struct CtCanvas *canvas, const size_t y, const wchar_t *text);
void ct_cwrite_e(struct CtCanvas *canvas, const size_t y, const wchar_t *text);
void ct_cwrite_cs(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_cc(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_ce(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_ss(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_sc(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_se(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_es(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_ec(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwrite_ee(struct CtCanvas *canvas, const wchar_t *text);

void ct_cwritev_s(struct CtCanvas *canvas, const size_t x, const wchar_t *text);
void ct_cwritev_c(struct CtCanvas *canvas, const size_t x, const wchar_t *text);
void ct_cwritev_e(struct CtCanvas *canvas, const size_t x, const wchar_t *text);
void ct_cwritev_cs(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_cc(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_ce(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_ss(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_sc(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_se(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_es(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_ec(struct CtCanvas *canvas, const wchar_t *text);
void ct_cwritev_ee(struct CtCanvas *canvas, const wchar_t *text);

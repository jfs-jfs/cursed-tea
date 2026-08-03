#pragma once

#include <cursed-tea/core.h>
#include <stdbool.h>
#include <wchar.h>

#define CTU_STATUS_MAX_SIZE 64

struct CursedStatus {
  struct CtModel base;
  bool veritcal;
  wchar_t label_ts[CTU_STATUS_MAX_SIZE + 1];
  wchar_t label_tc[CTU_STATUS_MAX_SIZE + 1];
  wchar_t label_te[CTU_STATUS_MAX_SIZE + 1];
  wchar_t label_bs[CTU_STATUS_MAX_SIZE + 1];
  wchar_t label_bc[CTU_STATUS_MAX_SIZE + 1];
  wchar_t label_be[CTU_STATUS_MAX_SIZE + 1];
};

void ctu_status_setup(struct CursedStatus *model, bool vertical,
                         const wchar_t *label_ts, const wchar_t *label_tc,
                         const wchar_t *label_te, const wchar_t *label_bs,
                         const wchar_t *label_bc, const wchar_t *label_be);

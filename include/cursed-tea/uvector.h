#pragma once
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
  size_t x;
  size_t y;
} CtUVector;

// Cell coordinate: must be strictly inside [0, area).
static inline bool ct_uvec_is_inside(const CtUVector area, const CtUVector point) {
  return area.x > point.x && area.y > point.y;
}

// Exclusive end bound: may touch the far edge (end == area is valid).
static inline bool ct_uvec_fits(const CtUVector area, const CtUVector end) {
  return area.x >= end.x && area.y >= end.y;
}

static inline bool ct_uvec_same_as(const CtUVector a, const CtUVector b) {
  return a.x == b.x && a.y == b.y;
}

static inline float ct_uvec_distance(const CtUVector from, const CtUVector to) {
  return fabsf(sqrtf(powf(to.x - from.x, 2.0) + powf(to.y - from.y, 2.0)));
}

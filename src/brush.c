#include <cursed-tea/border.h>
#include <cursed-tea/brush.h>
#include <cursed-tea/logger.h>
#include <limits.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

#define COLOR_ID_BASE 16
#define PAIR_ID_BASE 1
#define MAX_COLOR_CACHE 1024
#define MAX_PAIR_CACHE 1024

static bool COLOR = false;
static bool TRUECOLOR = false;

static struct CtBrush DEFAULT_BRUSH;
static struct CtBrush CURRENT_BRUSH;
static cchar_t DEFAULT_CELL;

struct ColorCacheEntry {
  struct CtRGB rgb;
  short id;
  bool used;
};

struct PairCacheEntry {
  struct CtRGB foreground, background;
  short pair;
  bool used;
};

static struct ColorCacheEntry COLOR_CACHE[MAX_COLOR_CACHE];
static size_t COLOR_CACHE_LEN = 0;
static size_t NEXT_COLOR_ID = COLOR_ID_BASE;
static bool LOGGED_COLOR_FALLBACK = false;

static struct PairCacheEntry PAIR_CACHE[MAX_PAIR_CACHE];
static size_t PAIR_SLOT_LIMIT = 0;
static size_t NEXT_PAIR_SLOT = 0;
static bool LOGGED_PAIR_FALLBACK = false;

short _rgb_to_xterm256(const struct CtRGB color);
short _allocate_color(const struct CtRGB color);
short _allocate_color_pair(const struct CtRGB foreground,
                           const struct CtRGB background);
unsigned _rgb_dist2(const struct CtRGB a, const struct CtRGB b);
short _closest_pair(const struct CtRGB foreground,
                    const struct CtRGB background);
short _closest_color(const struct CtRGB color);

static int _hex_nibble(const char character) {
  if (character >= '0' && character <= '9')
    return character - '0';
  if (character >= 'a' && character <= 'f')
    return character - 'a' + 10;
  if (character >= 'A' && character <= 'F')
    return character - 'A' + 10;
  return -1;
}

bool parse_hex_color(const char *hex, struct CtRGB *result) {
  if (NULL == hex)
    return false;
  if (hex[0] == '#')
    hex++;

  size_t length = 0;
  while (hex[length] != '\0')
    length++;

  uint8_t r, g, b;
  if (3 == length) {
    int r_nib = _hex_nibble(hex[0]);
    int g_nib = _hex_nibble(hex[1]);
    int b_nib = _hex_nibble(hex[2]);
    if (r_nib < 0 || g_nib < 0 || b_nib < 0)
      return false;
    r = (uint8_t)(r_nib * 17);
    g = (uint8_t)(g_nib * 17);
    b = (uint8_t)(b_nib * 17);
  } else if (6 == length) {
    int r_nib = _hex_nibble(hex[0]), r_nib1 = _hex_nibble(hex[1]);
    int g_nib = _hex_nibble(hex[2]), g_nib1 = _hex_nibble(hex[3]);
    int b_nib = _hex_nibble(hex[4]), b_nib1 = _hex_nibble(hex[5]);
    if (r_nib < 0 || r_nib1 < 0 || g_nib < 0 || g_nib1 < 0 || b_nib < 0 ||
        b_nib1 < 0)
      return false;
    r = (uint8_t)(r_nib * 16 + r_nib1);
    g = (uint8_t)(g_nib * 16 + g_nib1);
    b = (uint8_t)(b_nib * 16 + b_nib1);
  } else
    return false;

  result->r = r;
  result->g = g;
  result->b = b;
  return true;
}

bool rgb_same_as(const struct CtRGB a, const struct CtRGB b) {
  return a.r == b.r && a.b == b.b && a.g == b.g;
}

void ct_brush_setup() {
  DEFAULT_BRUSH.foreground = DEFAULT_FOREGROUND;
  DEFAULT_BRUSH.background = DEFAULT_BACKGROUND;
  DEFAULT_BRUSH.attribute = A_NORMAL;
  DEFAULT_BRUSH.border_style = B_NORMAL;
  CURRENT_BRUSH = DEFAULT_BRUSH;

  COLOR = has_colors();
  TRUECOLOR = false;
  COLOR_CACHE_LEN = 0;
  NEXT_COLOR_ID = COLOR_ID_BASE;
  LOGGED_COLOR_FALLBACK = false;
  LOGGED_PAIR_FALLBACK = false;
  NEXT_PAIR_SLOT = 0;
  PAIR_SLOT_LIMIT = 0;

  for (size_t i = 0; i < MAX_COLOR_CACHE; i++)
    COLOR_CACHE[i].used = false;
  for (size_t i = 0; i < MAX_PAIR_CACHE; i++)
    PAIR_CACHE[i].used = false;

  if (COLOR) {
    log_info(L"term has color");
    start_color();
    TRUECOLOR = can_change_color();
    if (TRUECOLOR)
      log_info(L"term has truecolor");

    size_t usable = 0;
    if (COLOR_PAIRS > PAIR_ID_BASE)
      usable = (size_t)COLOR_PAIRS - (size_t)PAIR_ID_BASE;
    if (usable > MAX_PAIR_CACHE)
      usable = MAX_PAIR_CACHE;
    PAIR_SLOT_LIMIT = usable;

    log_fmt(LOG_INFO, L"COLORS=%d COLOR_PAIRS=%d pair_slots=%zu", COLORS,
            COLOR_PAIRS, PAIR_SLOT_LIMIT);
  } else {
    log_warn(L"term has no color support");
  }

  ct_brush_cell(&DEFAULT_CELL, L' ');
}

void ct_brush_border(const enum CtBorderStyles border_style) {
  CURRENT_BRUSH.border_style = border_style;
}

void ct_brush_fg(const uint8_t r, const uint8_t g, const uint8_t b) {
  CURRENT_BRUSH.foreground = (struct CtRGB){r, g, b};
}

void ct_brush_bg(const uint8_t r, const uint8_t g, const uint8_t b) {
  CURRENT_BRUSH.background = (struct CtRGB){r, g, b};
}

void ct_brush_fg_hex(const char *hex) {
  struct CtRGB color;
  if (!parse_hex_color(hex, &color)) {
    log_fmt(LOG_ERR, L"invalid fg hex color '%hs'", hex ? hex : "(null)");
    return;
  }
  CURRENT_BRUSH.foreground = color;
}

void ct_brush_bg_hex(const char *hex) {
  struct CtRGB color;
  if (!parse_hex_color(hex, &color)) {
    log_fmt(LOG_ERR, L"invalid bg hex color '%hs'", hex ? hex : "(null)");
    return;
  }
  CURRENT_BRUSH.background = color;
}

void ct_brush_swap_colors() {
  struct CtRGB aux = CURRENT_BRUSH.foreground;
  CURRENT_BRUSH.foreground = CURRENT_BRUSH.background;
  CURRENT_BRUSH.background = aux;
}

void ct_brush_attr(const attr_t attribute) {
  CURRENT_BRUSH.attribute = attribute;
}

void ct_brush_add_attr(const attr_t attribute) {
  CURRENT_BRUSH.attribute |= attribute;
}

void ct_brush_clean() { CURRENT_BRUSH = DEFAULT_BRUSH; }

struct CtBrush ct_brush() { return CURRENT_BRUSH; }

bool ct_brush_has_color(void) { return COLOR; }

bool ct_brush_has_truecolor(void) { return TRUECOLOR; }

int ct_brush_colors(void) { return COLOR ? COLORS : 0; }

int ct_brush_color_pairs(void) { return COLOR ? COLOR_PAIRS : 0; }

short ct_brush_current_color_pair() {
  if (!COLOR)
    return 0;
  return _allocate_color_pair(CURRENT_BRUSH.foreground,
                              CURRENT_BRUSH.background);
}

void ct_brush_cell(cchar_t *cell, const wchar_t character) {
  wchar_t glyph[2] = {character, L'\0'};
  short pair = ct_brush_current_color_pair();
  setcchar(cell, glyph, CURRENT_BRUSH.attribute, pair, NULL);
}

cchar_t ct_brush_default_cell() { return DEFAULT_CELL; }

void ct_brush_from(const struct CtBrush other) {
  CURRENT_BRUSH.attribute = other.attribute;
  CURRENT_BRUSH.background = other.background;
  CURRENT_BRUSH.foreground = other.foreground;
  CURRENT_BRUSH.border_style = other.border_style;
}

// ~~[ INTERNAL

unsigned _rgb_dist2(const struct CtRGB a, const struct CtRGB b) {
  int dr = (int)a.r - (int)b.r;
  int dg = (int)a.g - (int)b.g;
  int db = (int)a.b - (int)b.b;
  return (unsigned)(dr * dr + dg * dg + db * db);
}

short _closest_pair(const struct CtRGB foreground,
                    const struct CtRGB background) {
  short best_pair = 0;
  unsigned best = UINT_MAX;
  bool found = false;

  for (size_t i = 0; i < PAIR_SLOT_LIMIT; i++) {
    if (!PAIR_CACHE[i].used)
      continue;
    unsigned d = _rgb_dist2(PAIR_CACHE[i].foreground, foreground) +
                 _rgb_dist2(PAIR_CACHE[i].background, background);
    if (d < best) {
      best = d;
      best_pair = PAIR_CACHE[i].pair;
      found = true;
    }
  }

  return found ? best_pair : 0;
}

short _closest_color(const struct CtRGB color) {
  short best_id = _rgb_to_xterm256(color);
  unsigned best = UINT_MAX;
  bool found = false;

  for (size_t i = 0; i < COLOR_CACHE_LEN; i++) {
    if (!COLOR_CACHE[i].used)
      continue;
    unsigned d = _rgb_dist2(COLOR_CACHE[i].rgb, color);
    if (d < best) {
      best = d;
      best_id = COLOR_CACHE[i].id;
      found = true;
    }
  }

  (void)found;
  return best_id;
}

short _allocate_color_pair(const struct CtRGB foreground,
                           const struct CtRGB background) {
  for (size_t i = 0; i < PAIR_SLOT_LIMIT; i++) {
    if (PAIR_CACHE[i].used &&
        rgb_same_as(PAIR_CACHE[i].foreground, foreground) &&
        rgb_same_as(PAIR_CACHE[i].background, background)) {
      return PAIR_CACHE[i].pair;
    }
  }

  if (NEXT_PAIR_SLOT >= PAIR_SLOT_LIMIT) {
    if (!LOGGED_PAIR_FALLBACK) {
      log_warn(L"color-pair cache full, snapping to nearest pair");
      LOGGED_PAIR_FALLBACK = true;
    }
    return _closest_pair(foreground, background);
  }

  short foreground_id = _allocate_color(foreground);
  short background_id = _allocate_color(background);
  size_t slot = NEXT_PAIR_SLOT++;
  short pair = (short)(slot + PAIR_ID_BASE);
  init_extended_pair(pair, foreground_id, background_id);

  PAIR_CACHE[slot].foreground = foreground;
  PAIR_CACHE[slot].background = background;
  PAIR_CACHE[slot].pair = pair;
  PAIR_CACHE[slot].used = true;
  return pair;
}

short _allocate_color(const struct CtRGB color) {
  for (size_t i = 0; i < COLOR_CACHE_LEN; i++) {
    if (COLOR_CACHE[i].used && rgb_same_as(COLOR_CACHE[i].rgb, color))
      return COLOR_CACHE[i].id;
  }

  short id;
  if (!TRUECOLOR) {
    id = _rgb_to_xterm256(color);
  } else if ((int)NEXT_COLOR_ID >= COLORS) {
    /* Do not map to stock xterm indices — those slots were redefined. */
    if (!LOGGED_COLOR_FALLBACK) {
      log_warn(L"color slots exhausted, snapping to nearest cached color");
      LOGGED_COLOR_FALLBACK = true;
    }
    return _closest_color(color);
  } else {
    id = (short)NEXT_COLOR_ID++;
    init_extended_color(id, (int)((color.r * 1000) / 255),
                        (int)((color.g * 1000) / 255),
                        (int)((color.b * 1000) / 255));
  }

  if (COLOR_CACHE_LEN < MAX_COLOR_CACHE) {
    COLOR_CACHE[COLOR_CACHE_LEN].rgb = color;
    COLOR_CACHE[COLOR_CACHE_LEN].id = id;
    COLOR_CACHE[COLOR_CACHE_LEN].used = true;
    COLOR_CACHE_LEN++;
  }

  return id;
}

short _rgb_to_xterm256(const struct CtRGB color) {
  if (color.r == color.g && color.g == color.b) {
    if (color.r < 8)
      return 16;
    if (color.r > 248)
      return 231;
    return (short)(232 + ((color.r - 8) * 24) / 247);
  }
  short r = (short)(color.r * 5 / 255);
  short g = (short)(color.g * 5 / 255);
  short b = (short)(color.b * 5 / 255);
  return (short)(16 + 36 * r + 6 * g + b);
}

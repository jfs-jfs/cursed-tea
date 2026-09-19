#include <assert.h>
#include <cursed-tea/application.h>
#include <cursed-tea/canvas.h>
#include <cursed-tea/core.h>
#include <cursed-tea/event.h>
#include <cursed-tea/logger.h>
#include <cursed-tea/style/brush.h>
#include <ncurses.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

#define EVENT_POLLING_DELAY_MS 10

void _system_init();
void _system_cleanup();

// Reads the real terminal size via TIOCGWINSZ on stdout. Unlike ncurses'
// cached LINES/COLS (which only refresh on a resize), this always reflects
// the authentic PTY dimensions, so the UI converges even when SIGWINCH
// delivery to the process cannot be relied upon (e.g. over SSH).
static bool _terminal_size(size_t *out_x, size_t *out_y) {
  struct winsize ws;
  if (ioctl(fileno(stdout), TIOCGWINSZ, &ws) != 0)
    return false;
  *out_x = ws.ws_col;
  *out_y = ws.ws_row;
  return true;
}

static void _apply_resize() {
  resize_term(0, 0);
  int max_x, max_y;
  getmaxyx(stdscr, max_y, max_x);
  ct_main_canvas_resize(max_x, max_y);
  ct_event_send_resize(max_x, max_y);
}

void _sleep_ms(long ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000L;
  nanosleep(&ts, NULL);
}

void ct_app_init() { _system_init(); }

void ct_app_start(struct CtModel *root) {
  assert(NULL != root);

  // _system_init(); // Now called on ct_app_init

  wchar_t pressed_key;

  bool should_exit = false;

  log_info(L"Starting main loop");
  while (!should_exit) {
    // Poll the real terminal size each frame. SIGWINCH/KEY_RESIZE may be
    // unreliable or arrive out of order on some transports (notably SSH),
    // so compare against the authoritative PTY size and resize on change.
    {
      size_t term_x, term_y;
      if (_terminal_size(&term_x, &term_y) && term_x > 0 && term_y > 0) {
        struct CtCanvas *c = ct_main_canvas();
        if (c->max_x != term_x || c->max_y != term_y)
          _apply_resize();
      }
    }

    // Poll Events first always starts with events
    timeout(EVENT_POLLING_DELAY_MS);
    while (0 == ct_event_count()) {
      int rc = get_wch(&pressed_key);
      if (rc == OK || rc == KEY_CODE_YES) {
        if (pressed_key != KEY_RESIZE)
          ct_event_send_key(pressed_key);
        else
          _apply_resize();
      }
    }

    // Consume events
    while (0 != ct_event_count() && !should_exit) {
      struct CtEvent *event = ct_event_next();
      if (event->type == EXIT_EVENT) {
        log_info(L"EXIT EVENT RECEIVED");
        should_exit = true;
        continue;
      }

      root->handler(root, event);
      ct_event_consumed();
    }

    // Drawing pass
    struct CtCanvas *canvas = ct_main_canvas();
    ct_main_canvas_clear();
    root->render(root, canvas);

    for (size_t y = 0; y < canvas->max_y; y++)
      mvadd_wchnstr(y, 0, canvas->cell_matrix[y], (int)canvas->max_x);
    refresh();
  }

  root->cleanup(root);
}

void _system_init() {
  ct_logger_setup();
  ct_logger_file("cursed-tea.log", false);
  ct_logger_level(LOG_TRACE);
  log_info(L"logger setup done");

  // Locale :: UNICODE
  setlocale(LC_ALL, "");
  log_info(L"unicode locale setup");

  // Ncurses
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, true);
  curs_set(0);

  // Color subsystem (truecolor if available)
  ct_brush_setup();

  // Register cleanup
  atexit(_system_cleanup);

  // Events ring
  ct_events_setup();

  // Main canvas setup
  int max_x, max_y;
  getmaxyx(stdscr, max_y, max_x);
  ct_main_canvas_init(max_x, max_y);
  ct_event_send_resize(max_x, max_y);
}

void _system_cleanup() {
  log_trace();

  // Ncurses
  endwin();
}

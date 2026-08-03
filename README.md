# cursed-tea

A small **TEA-style** (The Elm Architecture) TUI framework on **ncursesw**, with optional UI widgets.

License: **MIT**

## Requirements

- C11 compiler
- [ncursesw](https://invisible-island.net/ncurses/) (wide-character ncurses)
- `pkg-config`
- `pthread`, `libm`

## Layout

| Path | Role |
|------|------|
| `include/cursed-tea/` | Public headers |
| `src/core/` | Core library sources (+ private `fifo.h`) |
| `src/ui/` | Optional widget sources |
| `examples/` | Demo programs |
| `dist/` | Generated amalgamations (`make amalgamate`) |

## Build & install (Make)

```sh
make                 # libcursed-tea.a + libcursed-tea-ui.a
make examples        # build/example_*
sudo make install    # core + ui → PREFIX (default /usr/local)
sudo make install-core
sudo make uninstall
```

### Use after install

```c
#include <cursed-tea/core.h>
#include <cursed-tea/ui.h>   /* optional */

int main(void) {
    /* ... build a CtModel ... */
    ct_app_start(root);
}
```

```sh
# core only
cc app.c $(pkg-config --cflags --libs cursed-tea) -o app

# core + widgets
cc app.c $(pkg-config --cflags --libs cursed-tea-ui) -o app
```

You can also include individual widgets:

```c
#include <cursed-tea/core.h>
#include <cursed-tea/ui/button.h>
```

## Amalgamation (drop-in)

```sh
make amalgamate
```

### Single header (stb-style)

`dist/single/cursed-tea.h`

```c
/* exactly one translation unit */
#define CURSED_TEA_IMPLEMENTATION
#define CURSED_TEA_UI            /* omit for core-only */
#include "cursed-tea.h"
```

```sh
cc app.c -lncursesw -lm -pthread -o app
```

### Header + one source (SQLite-style)

| Files | Contents |
|-------|----------|
| `dist/amalg/cursed-tea.h` + `cursed-tea.c` | Core |
| `dist/amalg/cursed-tea-ui.h` + `cursed-tea-ui.c` | Core + UI |

```sh
cc app.c cursed-tea-ui.c -lncursesw -lm -pthread -o app
```

## Minimal example

```c
#include <cursed-tea/core.h>
#include <stdlib.h>

struct App {
    struct CtModel base;
};

static void cleanup(void *m) { free(m); }

static void handler(void *m, const struct CtEvent *ev) {
    (void)m;
    if (ev->type == KEY_EVENT && ev->key == L'q')
        ct_event_send_exit();
}

static void render(const void *m, struct CtCanvas *canvas) {
    (void)m;
    ct_cwrite_cc(canvas, L"press q to quit");
}

int main(void) {
    struct App *app = malloc(sizeof *app);
    app->base.cleanup = cleanup;
    app->base.handler = handler;
    app->base.render = render;
    ct_app_start(&app->base);
    return 0;
}
```

## API naming (current)

Documented debt — may be normalized in a later release:

| Area | Prefixes |
|------|----------|
| Core functions / types | `ct_…`, `Ct…` |
| UI functions | `ctu_…` |
| UI structs | `Cursed…` (e.g. `CursedButton`) |
| UI constants | `CTU_…` |

Internal helpers (not public API): `fifo` ring buffer used by the event queue.

## Incomplete / not shipped in the UI library

- `include/cursed-tea/ui/menu.h` — header only, no implementation yet (not linked).

## Development

```sh
make clean all examples
# clangd: compile_flags.txt uses -Iinclude
```

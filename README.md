# cursed-tea

!! Still in early development, use at your own risk !!

A small **TEA-style** (The Elm Architecture) TUI framework build on top of **ncursesw**.

License: **MIT**

## Requirements

- C11 compiler
- [ncursesw](https://invisible-island.net/ncurses/) (wide-character ncurses)
- `pkg-config`
- `pthread`, `libm`

## Build & install (Make)

```sh
make                 # libcursed-tea.a
sudo make install    # PREFIX (default /usr/local)
sudo make uninstall
```

### Use after install

```c
#include <cursed-tea.h>

int main(void) {
    ct_app_init();
    /* ... build your CtModels ... */
    ct_app_start(root);
}
```

```sh
# Compilation
cc main.c $(pkg-config --cflags --libs cursed-tea) -o program

```

## Minimal example

```c
#include <cursed-tea.h>
#include <cursed-tea/helpers.h>

// Important for the struct CtModel base to be the first element of the struct
// that way a pointer to app->base will be the same as a pointer to app
struct App {
  struct CtModel base;
};

void handler(void *m, const struct CtEvent *ev) {
  (void)m;
  if (ev->type == KEY_EVENT && ev->key == L'q')
    ct_event_send_exit();
}

void render(const void *m, struct CtCanvas *canvas) {
  (void)m;
  ct_cwrite_cc(canvas, L"press q to quit");
}

int main(void) {
  ct_app_init();
  struct App app;
  app.base.cleanup = ct_empty_cleanup;
  app.base.handler = handler;
  app.base.render = render;
  ct_app_start(&app.base);
  return 0;
}
```

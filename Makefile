# cursed-tea — TEA-style TUI library
#   make              build static libs
#   make examples     build example programs
#   make install      install core + ui
#   make install-core install core only
#   make amalgamate   generate drop-in amalgamations
#   make uninstall

VERSION  := $(shell cat VERSION 2>/dev/null || echo 0.0.0)
PREFIX   ?= /usr/local
DESTDIR  ?=
LIBDIR   ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include
PKGCONFIGDIR ?= $(LIBDIR)/pkgconfig

CC       ?= cc
AR       ?= ar
ARFLAGS  ?= rcs
CFLAGS   ?= -O2 -g
CFLAGS   += -std=c11 -Wall -Wextra -Iinclude -Isrc/core
# Feature macros come from ncursesw.pc (_DEFAULT_SOURCE / _XOPEN_SOURCE)
CFLAGS   += $(shell pkg-config --cflags ncursesw 2>/dev/null)
LDFLAGS  ?=
LIBS_CORE := $(shell pkg-config --libs ncursesw 2>/dev/null || echo -lncursesw) -lm -pthread

BUILD    := build
DIST     := dist

CORE_SRCS := \
	src/core/application.c \
	src/core/canvas.c \
	src/core/canvas-write.c \
	src/core/event.c \
	src/core/fifo.c \
	src/core/layout.c \
	src/core/logger.c \
	src/core/helpers.c \
	src/core/style/border.c \
	src/core/style/brush.c

UI_SRCS := \
	src/ui/button.c \
	src/ui/input.c \
	src/ui/label.c \
	src/ui/labeled-input.c \
	src/ui/line.c \
	src/ui/panel.c \
	src/ui/progress.c \
	src/ui/scrollable-area.c \
	src/ui/size-guard.c \
	src/ui/status.c \
	src/ui/table.c \
	src/ui/throbber.c \
	src/ui/toggle.c \
	src/ui/viewport.c

CORE_OBJS := $(CORE_SRCS:%.c=$(BUILD)/%.o)
UI_OBJS   := $(UI_SRCS:%.c=$(BUILD)/%.o)

LIB_CORE := $(BUILD)/libcursed-tea.a
LIB_UI   := $(BUILD)/libcursed-tea-ui.a

EXAMPLE_SRCS := $(wildcard examples/*.c)
EXAMPLE_BINS := $(patsubst examples/%.c,$(BUILD)/example_%,$(EXAMPLE_SRCS))

.PHONY: all clean examples install install-core uninstall amalgamate \
	pc dirs help

all: $(LIB_CORE) $(LIB_UI)

help:
	@echo "Targets: all examples install install-core uninstall amalgamate clean"
	@echo "PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) VERSION=$(VERSION)"

dirs:
	@mkdir -p $(BUILD)/src/core/style $(BUILD)/src/ui $(BUILD)/pkgconfig

$(BUILD)/%.o: %.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_CORE): $(CORE_OBJS) | dirs
	$(AR) $(ARFLAGS) $@ $^

$(LIB_UI): $(UI_OBJS) | dirs
	$(AR) $(ARFLAGS) $@ $^

examples: $(EXAMPLE_BINS)

$(BUILD)/example_%: examples/%.c $(LIB_CORE) $(LIB_UI) | dirs
	$(CC) $(CFLAGS) $< -o $@ $(LIB_UI) $(LIB_CORE) $(LDFLAGS) $(LIBS_CORE)

# --- pkg-config ---
NCURSES_CFLAGS := $(shell pkg-config --cflags ncursesw 2>/dev/null)
NCURSES_LIBS   := $(shell pkg-config --libs ncursesw 2>/dev/null || echo -lncursesw)

pc: dirs
	@sed \
		-e 's|@PREFIX@|$(PREFIX)|g' \
		-e 's|@LIBDIR@|$(LIBDIR)|g' \
		-e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
		-e 's|@VERSION@|$(VERSION)|g' \
		-e 's|@NCURSES_CFLAGS@|$(NCURSES_CFLAGS)|g' \
		-e 's|@NCURSES_LIBS@|$(NCURSES_LIBS)|g' \
		pkgconfig/cursed-tea.pc.in > $(BUILD)/pkgconfig/cursed-tea.pc
	@sed \
		-e 's|@PREFIX@|$(PREFIX)|g' \
		-e 's|@LIBDIR@|$(LIBDIR)|g' \
		-e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
		-e 's|@VERSION@|$(VERSION)|g' \
		pkgconfig/cursed-tea-ui.pc.in > $(BUILD)/pkgconfig/cursed-tea-ui.pc

install: all pc install-core
	install -d $(DESTDIR)$(LIBDIR)
	install -m 644 $(LIB_UI) $(DESTDIR)$(LIBDIR)/
	install -d $(DESTDIR)$(INCLUDEDIR)/cursed-tea/ui
	# menu.h is incomplete (no .c) — not installed
	install -m 644 \
		include/cursed-tea/ui/button.h \
		include/cursed-tea/ui/common.h \
		include/cursed-tea/ui/events.h \
		include/cursed-tea/ui/input.h \
		include/cursed-tea/ui/label.h \
		include/cursed-tea/ui/labeled-input.h \
		include/cursed-tea/ui/line.h \
		include/cursed-tea/ui/panel.h \
		include/cursed-tea/ui/progress.h \
		include/cursed-tea/ui/scrollable-area.h \
		include/cursed-tea/ui/size-guard.h \
		include/cursed-tea/ui/status.h \
		include/cursed-tea/ui/table.h \
		include/cursed-tea/ui/throbber.h \
		include/cursed-tea/ui/toggle.h \
		include/cursed-tea/ui/viewport.h \
		$(DESTDIR)$(INCLUDEDIR)/cursed-tea/ui/
	install -m 644 include/cursed-tea/ui.h $(DESTDIR)$(INCLUDEDIR)/cursed-tea/ui.h
	install -d $(DESTDIR)$(PKGCONFIGDIR)
	install -m 644 $(BUILD)/pkgconfig/cursed-tea-ui.pc $(DESTDIR)$(PKGCONFIGDIR)/

install-core: $(LIB_CORE) pc
	install -d $(DESTDIR)$(LIBDIR)
	install -m 644 $(LIB_CORE) $(DESTDIR)$(LIBDIR)/
	install -d $(DESTDIR)$(INCLUDEDIR)/cursed-tea/style
	install -m 644 include/cursed-tea/core.h \
		include/cursed-tea/cursed-tea.h \
		include/cursed-tea/application.h \
		include/cursed-tea/canvas.h \
		include/cursed-tea/canvas-write.h \
		include/cursed-tea/event.h \
		include/cursed-tea/helpers.h \
		include/cursed-tea/layout.h \
		include/cursed-tea/logger.h \
		include/cursed-tea/uvector.h \
		$(DESTDIR)$(INCLUDEDIR)/cursed-tea/
	install -m 644 include/cursed-tea/style/*.h \
		$(DESTDIR)$(INCLUDEDIR)/cursed-tea/style/
	install -d $(DESTDIR)$(PKGCONFIGDIR)
	install -m 644 $(BUILD)/pkgconfig/cursed-tea.pc $(DESTDIR)$(PKGCONFIGDIR)/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/libcursed-tea.a
	rm -f $(DESTDIR)$(LIBDIR)/libcursed-tea-ui.a
	rm -rf $(DESTDIR)$(INCLUDEDIR)/cursed-tea
	rm -f $(DESTDIR)$(PKGCONFIGDIR)/cursed-tea.pc
	rm -f $(DESTDIR)$(PKGCONFIGDIR)/cursed-tea-ui.pc

amalgamate:
	@mkdir -p $(DIST)/single $(DIST)/amalg
	python3 tools/amalgamate.py

clean:
	rm -rf $(BUILD) $(DIST)
	rm -f example_*

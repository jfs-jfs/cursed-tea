# cursed-tea — TEA-style TUI library
#   make              build static libs
#   make install      install library
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
	src/application.c \
	src/canvas.c \
	src/canvas-write.c \
	src/event.c \
	src/fifo.c \
	src/layout.c \
	src/logger.c \
	src/helpers.c \
	src/border.c \
	src/brush.c

CORE_OBJS := $(CORE_SRCS:%.c=$(BUILD)/%.o)

LIB_CORE := $(BUILD)/libcursed-tea.a

.PHONY: all clean install uninstall pc dirs help

all: $(LIB_CORE)

help:
	@echo "Targets: all install uninstall clean"
	@echo "PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) VERSION=$(VERSION)"

dirs:
	@mkdir -p $(BUILD)/src $(BUILD)/pkgconfig

$(BUILD)/%.o: %.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_CORE): $(CORE_OBJS) | dirs
	$(AR) $(ARFLAGS) $@ $^

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

install: all $(LIB_CORE) pc
	install -d $(DESTDIR)$(LIBDIR)
	install -m 644 $(LIB_CORE) $(DESTDIR)$(LIBDIR)/
	install -d $(DESTDIR)$(INCLUDEDIR)/cursed-tea
	install -m 644 include/cursed-tea.h $(DESTDIR)$(INCLUDEDIR)/
	install -m 644 include/cursed-tea/application.h \
		include/cursed-tea/canvas.h \
		include/cursed-tea/canvas-write.h \
		include/cursed-tea/event.h \
		include/cursed-tea/helpers.h \
		include/cursed-tea/layout.h \
		include/cursed-tea/logger.h \
		include/cursed-tea/uvector.h \
		include/cursed-tea/brush.h \
		include/cursed-tea/border.h \
		$(DESTDIR)$(INCLUDEDIR)/cursed-tea/
	install -d $(DESTDIR)$(PKGCONFIGDIR)
	install -m 644 $(BUILD)/pkgconfig/cursed-tea.pc $(DESTDIR)$(PKGCONFIGDIR)/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/libcursed-tea.a
	rm -f $(DESTDIR)$(INCLUDEDIR)/cursed-tea.h
	rm -rf $(DESTDIR)$(INCLUDEDIR)/cursed-tea
	rm -f $(DESTDIR)$(PKGCONFIGDIR)/cursed-tea.pc

clean:
	rm -rf $(BUILD) $(DIST)

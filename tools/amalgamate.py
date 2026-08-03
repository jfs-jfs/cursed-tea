#!/usr/bin/env python3
"""Generate drop-in amalgamations of cursed-tea.

Outputs:
  dist/single/cursed-tea.h          stb-style (optional CURSED_TEA_UI)
  dist/amalg/cursed-tea.h|.c        core only
  dist/amalg/cursed-tea-ui.h|.c     core + ui
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INCLUDE = ROOT / "include"
SRC = ROOT / "src"
DIST = ROOT / "dist"
VERSION = (ROOT / "VERSION").read_text().strip() if (ROOT / "VERSION").exists() else "0.0.0"

CORE_PUBLIC_HEADERS = [
    "cursed-tea/uvector.h",
    "cursed-tea/logger.h",
    "cursed-tea/canvas.h",
    "cursed-tea/event.h",
    "cursed-tea/application.h",
    "cursed-tea/canvas-write.h",
    "cursed-tea/layout.h",
    "cursed-tea/style/border.h",
    "cursed-tea/style/brush.h",
    # core.h last for CtModel (depends on canvas/event types via decls)
    "cursed-tea/core.h",
]

UI_PUBLIC_HEADERS = [
    "cursed-tea/ui/events.h",
    "cursed-tea/ui/common.h",
    "cursed-tea/ui/label.h",
    "cursed-tea/ui/button.h",
    "cursed-tea/ui/input.h",
    "cursed-tea/ui/labeled-input.h",
    "cursed-tea/ui/line.h",
    "cursed-tea/ui/panel.h",
    "cursed-tea/ui/progress.h",
    "cursed-tea/ui/scrollable-area.h",
    "cursed-tea/ui/size-guard.h",
    "cursed-tea/ui/status.h",
    "cursed-tea/ui/table.h",
    "cursed-tea/ui/throbber.h",
    "cursed-tea/ui/toggle.h",
    "cursed-tea/ui/viewport.h",
    "cursed-tea/ui.h",
]

CORE_SOURCES = [
    "core/fifo.h",  # private header inlined into impl
    "core/fifo.c",
    "core/logger.c",
    "core/canvas.c",
    "core/canvas-write.c",
    "core/event.c",
    "core/application.c",
    "core/layout.c",
    "core/style/border.c",
    "core/style/brush.c",
]

UI_SOURCES = [
    "ui/button.c",
    "ui/input.c",
    "ui/label.c",
    "ui/labeled-input.c",
    "ui/line.c",
    "ui/panel.c",
    "ui/progress.c",
    "ui/scrollable-area.c",
    "ui/size-guard.c",
    "ui/status.c",
    "ui/table.c",
    "ui/throbber.c",
    "ui/toggle.c",
    "ui/viewport.c",
]

INCLUDE_RE = re.compile(r'^\s*#\s*include\s+([<"])([^>"]+)([>"])\s*$', re.M)
PRAGMA_ONCE_RE = re.compile(r'^\s*#\s*pragma\s+once\s*$', re.M)


def read_include(rel: str) -> str:
    path = INCLUDE / rel
    if not path.exists():
        raise FileNotFoundError(path)
    return path.read_text()


def read_src(rel: str) -> str:
    path = SRC / rel
    if not path.exists():
        raise FileNotFoundError(path)
    return path.read_text()


def is_own_include(inc: str) -> bool:
    if inc.startswith("cursed-tea/"):
        return True
    if inc == "fifo.h":
        return True
    return False


def collect_system_includes(texts: list[str]) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []
    for text in texts:
        for m in INCLUDE_RE.finditer(text):
            kind, name, _ = m.group(1), m.group(2), m.group(3)
            if kind == "<" and not is_own_include(name):
                if name not in seen:
                    seen.add(name)
                    ordered.append(name)
    return ordered


def strip_includes_and_pragma(text: str) -> str:
    lines = []
    for line in text.splitlines():
        if PRAGMA_ONCE_RE.match(line):
            continue
        if INCLUDE_RE.match(line):
            m = INCLUDE_RE.match(line)
            assert m
            name = m.group(2)
            # drop own includes; system includes collected separately
            if is_own_include(name) or m.group(1) == "<":
                continue
            # quoted non-own (shouldn't happen) — keep
        lines.append(line)
    # trim leading/trailing blank lines in chunk
    body = "\n".join(lines)
    return body.strip() + "\n" if body.strip() else ""


def amalgamate_headers(header_rels: list[str]) -> tuple[str, list[str]]:
    chunks: list[str] = []
    raws: list[str] = []
    for rel in header_rels:
        raw = read_include(rel)
        raws.append(raw)
        body = strip_includes_and_pragma(raw)
        if body:
            chunks.append(f"/* ==== {rel} ==== */\n{body}")
    sys_incs = collect_system_includes(raws)
    return "\n".join(chunks), sys_incs


def amalgamate_sources(source_rels: list[str]) -> tuple[str, list[str]]:
    chunks: list[str] = []
    raws: list[str] = []
    for rel in source_rels:
        raw = read_src(rel)
        raws.append(raw)
        body = strip_includes_and_pragma(raw)
        if body:
            chunks.append(f"/* ==== src/{rel} ==== */\n{body}")
    sys_incs = collect_system_includes(raws)
    return "\n".join(chunks), sys_incs


def format_sys_includes(names: list[str]) -> str:
    # Prefer a stable, useful order
    priority = [
        "assert.h",
        "locale.h",
        "limits.h",
        "math.h",
        "pthread.h",
        "stdarg.h",
        "stdbool.h",
        "stddef.h",
        "stdint.h",
        "stdio.h",
        "stdlib.h",
        "string.h",
        "time.h",
        "unistd.h",
        "wchar.h",
        "ncurses.h",
    ]
    def key(n: str) -> tuple:
        try:
            return (0, priority.index(n))
        except ValueError:
            return (1, n)

    ordered = sorted(set(names), key=key)
    return "\n".join(f"#include <{n}>" for n in ordered)


BANNER = f"""\
/*
 * cursed-tea amalgamated distribution
 * version {VERSION}
 *
 * Generated by tools/amalgamate.py — do not edit by hand.
 * License: MIT (see LICENSE in the cursed-tea repository)
 */
"""


def write(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content)
    print(f"wrote {path.relative_to(ROOT)} ({len(content)} bytes)")


def build_amalg_pair(name: str, headers: list[str], sources: list[str]) -> None:
    hdr_body, hdr_sys = amalgamate_headers(headers)
    src_body, src_sys = amalgamate_sources(sources)
    all_sys = list(dict.fromkeys(hdr_sys + src_sys))

    header = (
        BANNER
        + f"#ifndef CURSED_TEA_AMALG_{name.upper().replace('-', '_')}_H\n"
        + f"#define CURSED_TEA_AMALG_{name.upper().replace('-', '_')}_H\n\n"
        + format_sys_includes(hdr_sys)
        + "\n\n"
        + hdr_body
        + f"\n#endif /* CURSED_TEA_AMALG_{name.upper().replace('-', '_')}_H */\n"
    )

    source = (
        BANNER
        + f'#include "{name}.h"\n\n'
        + format_sys_includes([s for s in src_sys if s not in set(hdr_sys)])
        + ("\n\n" if any(s not in set(hdr_sys) for s in src_sys) else "\n")
        + src_body
    )

    write(DIST / "amalg" / f"{name}.h", header)
    write(DIST / "amalg" / f"{name}.c", source)


def build_stb() -> None:
    core_hdr, core_hdr_sys = amalgamate_headers(CORE_PUBLIC_HEADERS)
    ui_hdr, ui_hdr_sys = amalgamate_headers(UI_PUBLIC_HEADERS)
    core_src, core_src_sys = amalgamate_sources(CORE_SOURCES)
    ui_src, ui_src_sys = amalgamate_sources(UI_SOURCES)

    pub_sys = list(dict.fromkeys(core_hdr_sys + ui_hdr_sys))
    impl_sys = list(dict.fromkeys(core_src_sys + ui_src_sys))
    # system includes needed only in implementation go inside IMPLEMENTATION
    impl_only = [s for s in impl_sys if s not in set(pub_sys)]

    content = (
        BANNER
        + """\
/*
 * Single-header (stb-style) build:
 *
 *   In exactly one .c file:
 *     #define CURSED_TEA_IMPLEMENTATION
 *     // #define CURSED_TEA_UI          // optional widgets
 *     #include "cursed-tea.h"
 *
 *   Everywhere else:
 *     #include "cursed-tea.h"
 *
 * Link with: -lncursesw -lm -pthread
 * (and define CURSED_TEA_UI in the implementation TU to include widgets)
 */

#ifndef CURSED_TEA_SINGLE_H
#define CURSED_TEA_SINGLE_H

"""
        + format_sys_includes(pub_sys)
        + """

/* ======================== CORE (public) ======================== */

"""
        + core_hdr
        + """
#ifdef CURSED_TEA_UI

/* ======================== UI (public) ======================== */

"""
        + ui_hdr
        + """
#endif /* CURSED_TEA_UI */

/* ======================== IMPLEMENTATION ======================== */

#ifdef CURSED_TEA_IMPLEMENTATION

"""
        + format_sys_includes(impl_only)
        + ("\n" if impl_only else "")
        + """
/* ---- core implementation ---- */

"""
        + core_src
        + """
#ifdef CURSED_TEA_UI

/* ---- ui implementation ---- */

"""
        + ui_src
        + """
#endif /* CURSED_TEA_UI */

#endif /* CURSED_TEA_IMPLEMENTATION */

#endif /* CURSED_TEA_SINGLE_H */
"""
    )
    write(DIST / "single" / "cursed-tea.h", content)


def main() -> int:
    build_stb()
    build_amalg_pair("cursed-tea", CORE_PUBLIC_HEADERS, CORE_SOURCES)
    build_amalg_pair(
        "cursed-tea-ui",
        CORE_PUBLIC_HEADERS + UI_PUBLIC_HEADERS,
        CORE_SOURCES + UI_SOURCES,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())

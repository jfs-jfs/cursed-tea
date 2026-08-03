# TODO: New TUI elements

Prioritized shortlist for cursed-tea (TEA + canvas layout + header-only widgets).

Existing: button, input, labeled-input, label, line, throbber, toggle, status, progress.

## High priority (fills real app gaps)

| Element | Why |
|--------|-----|
| **Radio group** | One-of-N selection; pairs with checkbox |
| **List / Menu** | Core navigation widget; selection + scroll + custom signal on choose |
| **Modal / Dialog** | Confirm/cancel over a dimmed region; needs simple overlay convention |

## Medium priority (power apps)

| Element | Why |
|--------|-----|
| **Tabs** | Multi-view screens without a full router |
| **Textarea / Viewport** | Multi-line view + scroll; unlocks logs, help, editors |
| **Scrollbar** | Shared primitive for list/table/textarea |
| **Select / Dropdown** | Compact list; harder (overlay + focus steal) |
| **Slider / Spinbox** | Numeric input without free text |
| **Toast / Notification** | Timed message via delayed custom events (existing pattern) |

## Lower priority / later

| Element | Notes |
|--------|--------|
| Tree | Nested list; needs collapse state |
| Breadcrumbs | Niche until multi-level nav |
| File picker | Heavy (FS + list + path input) |
| Date/time picker | Specialized |
| Tooltip | Needs hover/mouse or focus-delay |
| Markdown/rich text | Wait until `CtStyledZone` is real |
| Help/keybind bar | Thin status-bar variant |
| Autocomplete input | Input + filtered list composite |

## Framework pieces (not widgets, but unblock many of them)

1. **Focus group** — Tab cycle, `focus_next`/`prev` (everything interactive depends on this)
3. **Overlay / z-stack** — modal, dropdown, toast
5. **Form** — field list + validation + submit (composes labeled-input, checkbox, radio)

## Suggested build order

1. Checkbox + Toggle ✓
2. Focus group (small core helper)
3. List/Menu (+ basic scroll)
4. Progress bar ✓
5. Panel + Status bar (status ✓)
6. Modal/Dialog
7. Tabs
8. Textarea/Viewport
9. Table

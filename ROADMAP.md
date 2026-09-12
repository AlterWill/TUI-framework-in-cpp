# Roadmap

The project is organized into milestones and phases. Each phase builds on the previous one to form a stable, extensible TUI framework.

---

# Milestone 1: MVP Runtime

*Goal: Render text, boxes, layouts, incremental redraw, and terminal resize handling.*

## Phase 1 — Core Engine ✅

The foundation of the framework.

* [x] Widget base class and `WidgetBase` struct
* [x] Widget tree (`WidgetTree`)
* [x] Single-child widgets (`SingleChildWidget` with generic child render)
* [x] Multi-child widgets (`MultiChildWidget` with generic child render)
* [x] Layout pass (`measure()` $\to$ `layout()` / `setRectForChildren()`)
* [x] Render pass (`render(RenderContext&)` delegating to child buffers)
* [x] Framebuffer (`Surface` + `Buffer`)
* [x] UTF-8 rendering & display width calculations
* [x] Text widget with word wrapping and alignment (`Left`, `Center`, `Right`)
* [x] Box widget (multiple border styles, padding, margin, title)
* [x] Row layout (`Row` powered by `LinearLayoutSolver`)
* [x] Column layout (`Column` powered by `LinearLayoutSolver`)
* [x] Grid layout (`Grid<Rows, Cols>` compile-time grid)
* [x] Stack layout (`Stack` with independent child alignment)
* [x] Scroll layout (`Scroll` with offscreen `Buffer`, viewport blitting, and mouse wheel handling)
* [x] Margin support
* [x] Padding support

## Phase 2 — Rendering Engine 🚧

Improve how frames are produced and displayed.

* [x] ANSI style output
* [x] Incremental rendering (cell-diff via `incrementDisplay()`)
* [x] Dirty cell tracking
* [x] Double buffering (`Surface` holds `current` and `previous` buffers)
* [x] Off-screen buffer rendering and blitting (`Buffer::blitTo()`, used in `Scroll`)
* [ ] Clip rectangle stack in `RenderContext`

### Terminal Backend

* [x] 256-color support
* [x] True-color support
* [x] Backend abstraction (`backend` abstract class)
* [x] Linux backend (`linux_backend`: raw mode + SGR mouse)
* [ ] Headless / dummy terminal backend (for CI/CD unit testing)

---

# Milestone 2: Interactive Core

*Goal: Keyboard input, focus handling, flexible layouts, and theming.*

## Phase 3 — Input System 🚧

Everything related to user input and events.

* [x] Keyboard input
* [x] Mouse input (press, release, move, drag, scroll)
* [x] Scroll event handling in `Scroll` container
* [ ] Clipboard integration
* [ ] Terminal resize events (`SIGWINCH`)

### Event System

* [x] Event objects (`keyEvent`, `MouseEvent` as `std::variant<keyEvent, MouseEvent>`)
* [x] Event dispatcher (`EventDispatcher` in `eventHandler.hpp`)
* [x] Event bubbling (dispatched through widget tree)
* [ ] Event capture

### Focus System

* [x] Focus traversal (`EventDispatcher::nextFocus` / `previousFocus`)
* [x] Tab navigation
* [ ] Focus scopes (restricting Tab traversal to a subtree)

## Phase 4 — Layout Engine ✅

* [x] Measure pass (`measure()` on each widget)
* [x] Layout pass (`layout()` / `setRectForChildren()`)
* [x] `RenderContext` passed through render pass
* [x] Shared linear solver (`LinearLayoutSolver`)

### Constraints & Sizing

* [x] Fixed size (`SizeType::Fixed`)
* [x] Min / max size (`SizeConstraints`)
* [x] Percentage sizing (`SizeType::Percentage`)
* [x] Content-based intrinsic sizing (`SizeType::Content` with Clay-style shrink)
* [x] Flex sizing (`SizeType::Flex`)
* [x] Child alignment (via per-child margins & horizontal/vertical alignment enums)

### Containers

* [x] Row container (`Row`)
* [x] Column container (`Column`)
* [x] Grid container (`Grid<Rows, Cols>`)
* [x] Stack container (`Stack`)
* [x] Scroll container (`Scroll`)
* [x] Split pane (`SplitPane` with draggable divider)
* [x] Overlay container (`Overlay`, Stack-based with modal layering)

---

# Milestone 3: Usable Widgets

*Goal: A practical standard library of common TUI controls.*

## Phase 5 — Basic Widgets

* [x] Divider (preset glyphs via `dividerStyle` namespace)
* [x] Spacer
* [x] Spinner
* [x] Progress bar

## Phase 6 — Interactive Widgets

* [x] Button
* [x] Checkbox
* [x] Radio button
* [x] Toggle switch
* [x] Slider

## Phase 7 — Input Widgets

* [ ] Text input
* [ ] Password input
* [ ] Text area
* [ ] Number input

## Phase 8 — Advanced Widgets

* [ ] List view
* [ ] Table
* [ ] Tree view
* [ ] Tabs
* [ ] Menu
* [ ] Status bar
* [ ] Toolbar

## Phase 9 — Dialogs

* [ ] Popup
* [ ] Modal
* [ ] Tooltip
* [ ] Notification
* [ ] Context menu

---

# Milestone 4: Polish

## Phase 10 — Performance & DX

* [x] Dirty widget measurement caching (`LayoutNode::dirty`)
* [x] Diagnostic logger (`tui::Logger`, enabled via `-DENABLE_LOGGING`)
* [ ] API documentation
* [ ] Unit tests
* [ ] Benchmarks
* [ ] CI/CD pipeline

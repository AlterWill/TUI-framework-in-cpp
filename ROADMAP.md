# Roadmap

The project is organized into milestones and phases. Each phase builds on the previous one to form a stable, extensible TUI framework.

---

# Milestone 1: MVP Runtime

*Goal: Render text, boxes, layouts, incremental redraw, and terminal resize handling.*

## Phase 1 — Core Engine ✅

The foundation of the framework.

* [x] Widget base class
* [x] Widget tree
* [x] Single-child widgets
* [x] Multi-child widgets
* [x] Layout pass
* [x] Render pass
* [x] Framebuffer (`Surface` + `Buffer`)
* [x] UTF-8 rendering
* [x] Text widget (left alignment only; center/right are stubs)
* [x] Box widget (multiple border styles, padding, background fill)
* [x] Row layout
* [x] Column layout
* [x] Grid layout
* [x] Margin support
* [x] Padding support
* [ ] Declarative DSL (`react_dsl.hpp` is currently empty)

## Phase 2 — Rendering Engine 🚧

Improve how frames are produced and displayed.

* [x] ANSI style output
* [x] Incremental rendering (cell-diff via `incrementDisplay()`)
* [x] Dirty cell tracking
* [x] Double buffering (`Surface` holds `current` and `previous` buffers)
* [ ] Clip rectangle stack in `RenderContext` (not yet implemented)
* [ ] Off-screen rendering

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
* [ ] Clipboard integration
* [ ] Terminal resize events (`SIGWINCH`) — size is re-queried each frame via `resizeBuffer()`, but no event is dispatched

### Event System

* [x] Event objects (`keyEvent`, `MouseEvent` as `std::variant<keyEvent, MouseEvent>`)
* [x] Event dispatcher (`EventDispatcher` in `eventHandler.hpp`)
* [x] Event bubbling (dispatched up through `parent` chain)
* [ ] Event capture

### Focus System

* [x] Focus traversal (`EventDispatcher::nextFocus` / `previousFocus`)
* [x] Tab navigation (application must wire Tab key to `nextFocus`/`previousFocus`)
* [ ] Focus scopes (restricting Tab traversal to a subtree)

## Phase 4 — Layout Engine 🚧

Make layouts more flexible and expressive.

* [x] Measure pass (`measure()` on each widget)
* [x] Layout pass (`layout()` / `setRectForChildren()`)
* [x] `RenderContext` passed through render pass

### Constraints

* [x] Fixed size
* [x] Min / max size (`SizeConstraints`)
* [x] Flex ratios (`LayoutProperties::flex`)
* [x] Fill / expand (flex < 0 in `Flex` container)
* [ ] Percentage sizing
* [ ] Content based
* [x] Child alignment (via per-child margin)

### Containers

* [ ] Row container (`Row`)
* [ ] Column container (`Column`)
* [ ] Grid container (`Grid`)
* [ ] Stack container (`Stack`)
* [ ] Flex container (`Flex`) 
* [ ] Scroll container (`Scroll` stub exists; logic not implemented)
* [ ] Overlay container
* [ ] Split pane

## Phase 5 — Styling System

Move from per-widget styling to reusable themes.

### Styles

* [x] Per-widget style (`Style` with `ColourPair` and text style flags)
* [x] Text style flags: Bold, Dim, Italic, Underline, Blink, Reverse, Hidden, StrikeThrough

### Themes

* [ ] Theme manager
* [ ] Style inheritance
* [ ] Theme switching
* [ ] Global theme

### Widget States

* [ ] Hover
* [ ] Focused visual rendering
* [ ] Active
* [ ] Disabled
* [ ] Selected

---

# Milestone 3: Usable Widgets

*Goal: A practical standard library of common TUI controls.*

## Phase 6 — Basic Widgets

Simple display widgets.

* [ ] Spacer
* [ ] Divider
* [ ] Progress bar
* [ ] Spinner

## Phase 7 — Interactive Widgets

Widgets requiring events and focus.

* [ ] Button
* [ ] Checkbox
* [ ] Radio button
* [ ] Toggle switch
* [ ] Slider

## Phase 8 — Input Widgets

Editable controls.

* [ ] Text input
* [ ] Password input
* [ ] Text area
* [ ] Number input

## Phase 9 — Advanced Widgets

Larger UI components.

* [ ] List view
* [ ] Table
* [ ] Tree view
* [ ] Tabs
* [ ] Menu
* [ ] Status bar
* [ ] Toolbar

## Phase 10 — Dialogs

Transient UI components.

* [ ] Popup
* [ ] Modal
* [ ] Tooltip
* [ ] Notification
* [ ] Context menu

---

# Milestone 4: Polish

*Goal: Performance, documentation, testing, and developer experience.*

## Phase 11 — Performance

Improve runtime efficiency.

* [ ] Dirty widget rendering (skip layout/render for clean subtrees)
* [ ] Layout caching
* [ ] Render caching
* [ ] Frame timing infrastructure
* [ ] Memory optimizations

## Phase 12 — Developer Experience

Improve usability for library users and contributors.

* [x] Diagnostic logger (`tui::Logger`, enabled via `-DENABLE_LOGGING` compile flag)
* [ ] API documentation
* [ ] Example gallery
* [ ] Unit tests (layout math, UTF-8, event propagation)
* [ ] Headless testing backend (in-memory terminal for CI/CD)
* [ ] Benchmarks
* [ ] CI/CD pipeline (GitHub Actions)
* [ ] Package manager support

---

# Milestone 5: Long-Term Goals

*Goal: Future directions after the core framework is mature.*

## Phase 13 — Future Vision

* [ ] Image widget (ASCII / Unicode art)
* [ ] Animation system
* [ ] Async task integration
* [ ] Virtualized list view
* [ ] Markdown renderer
* [ ] Code editor widget
* [ ] Canvas widget

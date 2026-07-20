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
* [x] Framebuffer
* [x] UTF-8 rendering
* [x] Declarative DSL
* [x] Text widget
* [x] Box widget
* [x] Row layout
* [x] Column layout
* [x] Grid layout
* [x] Margin support
* [x] Padding support

## Phase 2 — Rendering Engine

Improve how frames are produced and displayed.

* [x] ANSI style output
* [x] Incremental rendering
* [x] Dirty cell tracking
* [x] Double buffering
* [x] Clipping
* [ ] Off-screen rendering

### Terminal Backend

* [x] 256-color support
* [x] True-color support
* [x] Backend abstraction

---

# Milestone 2: Interactive Core

*Goal: Keyboard input, focus handling, flexible layouts, and theming.*

## Phase 3 — Input System

Everything related to user input and events.

* [ ] Keyboard input
* [ ] Mouse input
* [ ] Clipboard
* [ ] Terminal resize events

### Event System

* [ ] Event objects
* [ ] Event dispatcher
* [ ] Event propagation
* [ ] Event bubbling
* [ ] Event capture

### Focus System

* [ ] Focus manager
* [ ] Focus traversal
* [ ] Tab navigation
* [ ] Focus scopes

## Phase 4 — Layout Engine

Make layouts more flexible and expressive.

### Containers

* [ ] Flex
* [ ] Stack
* [ ] Overlay
* [ ] Viewports
* [ ] Scroll container
* [ ] Split pane

### Constraints

* [ ] Fixed size
* [ ] Preferred size
* [ ] Fill / expand
* [ ] Min / max size
* [ ] Percentage sizing
* [ ] Child alignment

## Phase 5 — Styling System

Move from per-widget styling to reusable themes.

### Styles

* [x] Per-widget style

### Themes

* [ ] Theme manager
* [ ] Style inheritance
* [ ] Theme switching
* [ ] Global theme

### Widget States

* [ ] Hover
* [ ] Focused
* [ ] Active
* [ ] Disabled
* [ ] Selected

### Style System

* [ ] Style classes
* [ ] CSS-like selectors
* [ ] State-dependent styles

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

* [ ] Dirty widget rendering
* [ ] Layout caching
* [ ] Render caching
* [ ] Frame timing
* [ ] Memory optimizations

## Phase 12 — Developer Experience

Improve usability for library users and contributors.

* [ ] Documentation
* [ ] Example gallery
* [ ] Unit tests
* [ ] Benchmarks
* [ ] CI/CD
* [ ] Package manager support
* [ ] Better compiler diagnostics

---

# Milestone 5: Long-Term Goals

*Goal: Future directions after the core framework is mature.*

## Phase 13 — Future Vision

* [ ] Image widget (ASCII / Unicode)
* [ ] Animation system
* [ ] Async task integration
* [ ] Virtualized list view
* [ ] Markdown renderer
* [ ] Code editor widget
* [ ] Canvas widget

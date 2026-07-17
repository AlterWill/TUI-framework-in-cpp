# Roadmap

The project is developed in phases. Each phase builds upon the previous one to create a stable and extensible TUI framework.

---

# Phase 1 — Core Foundation ✅

The basic architecture of the framework.

- [x] Widget base class
- [x] Widget tree
- [x] Single-child widgets
- [x] Multi-child widgets
- [x] Layout pass
- [x] Render pass
- [x] Framebuffer
- [x] UTF-8 rendering
- [x] Declarative DSL
- [x] Text widget
- [x] Box widget
- [x] Row layout
- [x] Column layout
- [x] Grid layout
- [x] Margin support
- [x] Padding support

---

# Phase 2 — Rendering Engine

Improve how frames are produced and displayed.

## Rendering Pipeline

- [ ] ANSI style output
- [ ] Incremental rendering
- [ ] Dirty cell tracking
- [ ] Dirty rectangle tracking
- [ ] Double buffering
- [ ] Clipping
- [ ] Viewports
- [ ] Off-screen rendering

## Terminal Backend

- [ ] Automatic terminal resize detection
- [ ] 256-color support
- [ ] True-color support
- [ ] Backend abstraction

---

# Phase 3 — Layout Engine

Make layouts much more flexible.

## Containers

- [ ] Flex
- [ ] Stack
- [ ] Overlay
- [ ] Scroll Container
- [ ] Split Pane

## Constraints

- [ ] Fixed size
- [ ] Preferred size
- [ ] Fill / Expand
- [ ] Min / Max size
- [ ] Percentage sizing
- [ ] Child alignment
- [ ] Layout caching

---

# Phase 4 — Input System

Everything related to user input.

- [ ] Keyboard input
- [ ] Mouse input
- [ ] Clipboard
- [ ] Terminal resize events

## Event System

- [ ] Event objects
- [ ] Event dispatcher
- [ ] Event propagation
- [ ] Event bubbling
- [ ] Event capture

## Focus System

- [ ] Focus manager
- [ ] Focus traversal
- [ ] Tab navigation
- [ ] Focus scopes

---

# Phase 5 — Styling System

Move from per-widget styling to reusable themes.

## Styles

- [x] Per-widget style

## Themes

- [ ] Theme manager
- [ ] Style inheritance
- [ ] Theme switching
- [ ] Global theme

## Widget States

- [ ] Hover
- [ ] Focused
- [ ] Active
- [ ] Disabled
- [ ] Selected

## Style System

- [ ] Style classes
- [ ] CSS-like selectors
- [ ] State-dependent styles

---

# Phase 6 — Basic Widgets

Simple display widgets.

- [ ] Spacer
- [ ] Divider
- [ ] Progress Bar
- [ ] Spinner
- [ ] Image (ASCII / Unicode)

---

# Phase 7 — Interactive Widgets

Widgets requiring events and focus.

- [ ] Button
- [ ] Checkbox
- [ ] Radio Button
- [ ] Toggle Switch
- [ ] Slider

---

# Phase 8 — Input Widgets

Editable controls.

- [ ] TextInput
- [ ] PasswordInput
- [ ] TextArea
- [ ] NumberInput

---

# Phase 9 — Advanced Widgets

Larger UI components.

- [ ] ListView
- [ ] Table
- [ ] TreeView
- [ ] Tabs
- [ ] Menu
- [ ] Status Bar
- [ ] Toolbar

---

# Phase 10 — Dialogs

Transient UI.

- [ ] Popup
- [ ] Modal
- [ ] Tooltip
- [ ] Notification
- [ ] Context Menu

---

# Phase 11 — Performance

Improve runtime efficiency.

- [ ] Dirty widget rendering
- [ ] Layout caching
- [ ] Render caching
- [ ] Frame timing
- [ ] Memory optimizations

---

# Phase 12 — Developer Experience

Improve usability for library users and contributors.

- [ ] Documentation
- [ ] Example gallery
- [ ] Unit tests
- [ ] Benchmarks
- [ ] CI/CD
- [ ] Package manager support
- [ ] Better compiler diagnostics

---

# Phase 13 — Long-Term Goals

Future directions after the core framework is mature.

- [ ] Animation system
- [ ] Async task integration
- [ ] Virtualized ListView
- [ ] Markdown renderer
- [ ] Code editor widget
- [ ] Canvas widget

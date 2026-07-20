# TUI Library

A C++ terminal UI library built around a retained widget tree, incremental rendering, and a clean separation between layout, events, and terminal output.

## Overview

This project is a TUI framework for building terminal-based interfaces with reusable widgets, flexible layouts, and a declarative composition style. The goal is to make terminal UIs feel structured and scalable, while still staying lightweight and fast.

The library currently focuses on the core runtime:

* widget trees
* layout and render passes
* framebuffer-based drawing
* ANSI output
* incremental redraw
* UTF-8 rendering
* a simple declarative DSL

## Design Goals

The library is being built around a few core ideas:

* **Retained widget tree**: widgets stay alive and keep their own state.
* **Separation of concerns**: layout, rendering, input, focus, and styling should be separate systems.
* **Incremental rendering**: only redraw what changed.
* **Extensibility**: the architecture should support future widgets like buttons, inputs, dialogs, tables, and scroll views.
* **Practicality**: the framework should be useful for real terminal apps, not just demos.
* **Backend flexibility**: support different terminal capabilities like 256-color and true-color output.

## Current State

The current implementation already includes:

* Widget base class
* Widget tree
* Single-child widgets
* Multi-child widgets
* Layout pass
* Render pass
* Framebuffer
* UTF-8 rendering
* Declarative DSL
* Text widget
* Box widget
* Row layout
* Column layout
* Grid layout
* Margin support
* Padding support
* ANSI style output
* Incremental rendering
* Dirty cell tracking
* Double buffering

## Architecture

The library should be structured around a few clear layers.

### 1. App Layer

The app layer owns the overall UI state and runtime loop.

It is responsible for:

* holding the application state
* creating the root widget tree
* processing events
* triggering layout and rendering
* handling future command/task execution

### 2. Widget Tree

Widgets are the building blocks of the UI.

Each widget should be responsible for:

* `layout()` — deciding child rectangles
* `event()` — reacting to input
* `render()` — painting into the framebuffer

Widgets should not print directly to the terminal.

### 3. Layout Engine

The layout engine decides how space is distributed.

It should eventually support:

* row
* column
* grid
* flex
* stack
* overlay
* split panes
* viewports
* scroll containers

Layout should be based on constraints rather than hardcoded geometry, so future widgets can grow naturally.

### 4. Event System

The event system will handle:

* keyboard input
* mouse input
* resize events
* clipboard input
* custom events
* tick/update events

Events should move through the widget tree with focus-aware dispatch and optional propagation.

### 5. Focus System

Focus is needed for interactive widgets like:

* buttons
* inputs
* menus
* sliders
* dialogs

The focus system should support:

* focus manager
* focus traversal
* tab navigation
* focus scopes
* focused/hovered/active widget states

### 6. Renderer

Rendering should be separate from widgets.

The renderer should:

* draw into the framebuffer
* apply styles
* clip output
* support off-screen rendering
* perform dirty-region diffing
* flush changes to the terminal backend

### 7. Backend

The backend layer should abstract terminal-specific behavior.

It should be responsible for:

* terminal size detection
* color capability detection
* cursor control
* alternate screen support
* raw mode handling
* terminal capability differences

## Suggested Folder Structure

```text
src/
  app/
    app.hpp
    app.cpp
    runtime.hpp

  core/
    widget.hpp
    event.hpp
    rect.hpp
    constraints.hpp
    style.hpp
    focus.hpp

  layout/
    row.hpp
    column.hpp
    grid.hpp
    flex.hpp
    stack.hpp
    overlay.hpp
    scroll_view.hpp

  render/
    framebuffer.hpp
    renderer.hpp
    clipping.hpp
    dirty_region.hpp

  backend/
    backend.hpp
    linux_backend.hpp
    ansi_encoder.hpp

  widgets/
    text.hpp
    box.hpp
    button.hpp
    checkbox.hpp
    input.hpp
    list.hpp
    table.hpp

  theme/
    theme.hpp
    classes.hpp

  utils/
    unicode.hpp
    tools.hpp
```

## Render Flow

A frame should follow this flow:

1. Read input
2. Convert it into an event
3. Dispatch the event
4. Update application state
5. Recompute layout if needed
6. Render into the framebuffer
7. Diff the framebuffer
8. Flush changes to the terminal

This keeps the system predictable and easy to extend.

## What the Library Already Does Well

The current implementation already has a strong foundation:

* a retained widget tree
* a framebuffer model
* dirty cell tracking
* incremental rendering
* a declarative DSL
* Unicode and ANSI output
* basic layout containers

That is enough to grow into a real TUI framework.

## What Comes Next

The next important systems to build are:

1. **Clipping**
2. **Backend abstraction**
3. **Keyboard input**
4. **Mouse input**
5. **Resize events**
6. **Event objects and dispatch**
7. **Focus management**
8. **Constraint-based layout**
9. **Theme system**

After that, the library can start getting standard widgets.

## Roadmap

## Milestone 1: MVP Runtime

Goal: Render text, boxes, layouts, incremental redraw, and terminal resize handling.

### Phase 1 — Core Engine

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

### Phase 2 — Rendering Engine

* [x] ANSI style output
* [x] Incremental rendering
* [x] Dirty cell tracking
* [x] Double buffering
* [ ] Clipping
* [ ] Off-screen rendering

### Terminal Backend

* [ ] 256-color support
* [ ] True-color support
* [ ] Backend abstraction

## Milestone 2: Interactive Core

Goal: Keyboard input, focus handling, flexible layouts, and theming.

### Phase 3 — Input System

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

### Phase 4 — Layout Engine

#### Containers

* [ ] Flex
* [ ] Stack
* [ ] Overlay
* [ ] Viewports
* [ ] Scroll container
* [ ] Split pane

#### Constraints

* [ ] Fixed size
* [ ] Preferred size
* [ ] Fill / expand
* [ ] Min / max size
* [ ] Percentage sizing
* [ ] Child alignment

### Phase 5 — Styling System

#### Styles

* [x] Per-widget style

#### Themes

* [ ] Theme manager
* [ ] Style inheritance
* [ ] Theme switching
* [ ] Global theme

#### Widget States

* [ ] Hover
* [ ] Focused
* [ ] Active
* [ ] Disabled
* [ ] Selected

#### Style System

* [ ] Style classes
* [ ] CSS-like selectors
* [ ] State-dependent styles

## Milestone 3: Usable Widgets

Goal: A practical standard library of common TUI controls.

### Phase 6 — Basic Widgets

* [ ] Spacer
* [ ] Divider
* [ ] Progress bar
* [ ] Spinner

### Phase 7 — Interactive Widgets

* [ ] Button
* [ ] Checkbox
* [ ] Radio button
* [ ] Toggle switch
* [ ] Slider

### Phase 8 — Input Widgets

* [ ] Text input
* [ ] Password input
* [ ] Text area
* [ ] Number input

### Phase 9 — Advanced Widgets

* [ ] List view
* [ ] Table
* [ ] Tree view
* [ ] Tabs
* [ ] Menu
* [ ] Status bar
* [ ] Toolbar

### Phase 10 — Dialogs

* [ ] Popup
* [ ] Modal
* [ ] Tooltip
* [ ] Notification
* [ ] Context menu

## Milestone 4: Polish

Goal: Performance, documentation, testing, and developer experience.

### Phase 11 — Performance

* [ ] Dirty widget rendering
* [ ] Layout caching
* [ ] Render caching
* [ ] Frame timing
* [ ] Memory optimizations

### Phase 12 — Developer Experience

* [ ] Documentation
* [ ] Example gallery
* [ ] Unit tests
* [ ] Benchmarks
* [ ] CI/CD
* [ ] Package manager support
* [ ] Better compiler diagnostics

## Milestone 5: Long-Term Goals

Goal: Future directions after the core framework is mature.

### Phase 13 — Future Vision

* [ ] Image widget (ASCII / Unicode)
* [ ] Animation system
* [ ] Async task integration
* [ ] Virtualized list view
* [ ] Markdown renderer
* [ ] Code editor widget
* [ ] Canvas widget

## Example Usage

```cpp
#include "tui.hpp"

int main() {
    run_app(
        box(
            grid(
                rows = 2,
                cols = 2,
                box(text("Top Left")),
                box(text("Top Right")),
                box(text("Bottom Left")),
                box(text("Bottom Right"))
            )
        )
    );
}
```

## Project Philosophy

This library is meant to grow like a real framework:

* core first
* features second
* keep abstractions small
* avoid premature complexity
* make the architecture support future widgets before adding too many widgets

## License

Add your chosen license here.

## Contributing

Contributions are welcome once the core architecture is stable enough for outside use.

Suggested contribution areas:

* backend support
* input handling
* layout improvements
* new widgets
* rendering optimizations
* documentation
* tests

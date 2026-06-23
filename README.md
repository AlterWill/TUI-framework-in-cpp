# TUI — Terminal UI Framework in C++20

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/compiler_support/20)
[![CMake](https://img.shields.io/badge/CMake-3.28+-green.svg)](https://cmake.org/)

**TUI** is a terminal UI framework built in **C++20** around a **widget tree + layout container** architecture.
Instead of manually drawing terminal characters in application code, interfaces are composed from widgets such as **Box**, **Text**, **Row**, **Column**, and **Grid**, then rendered into a terminal framebuffer.

The project is currently focused on the **core rendering and layout foundation**:

* a `Widget` base class
* container widgets with child layout logic
* recursive widget-tree rendering
* Unicode-aware framebuffer output
* basic layout primitives for terminal UIs

The long-term goal is to grow this into a more complete terminal UI framework with **better layout constraints, input handling, focus management, interactive widgets, and a cleaner declarative C++ API** built with templates / builder helpers.

---

# Table of Contents

* [Overview](#overview)
* [Current Status](#current-status)
* [Goals](#goals)
* [Non-Goals](#non-goals)
* [Architecture](#architecture)
* [Core Components](#core-components)
* [Current Widget System](#current-widget-system)
* [Example](#example)
* [Project Structure](#project-structure)
* [Roadmap](#roadmap)
* [Future Plans](#future-plans)
* [Contributing](#contributing)
* [License](#license)
* [Getting Started](#getting-started)

---

# Overview

TUI is currently built around a **widget tree** rather than direct terminal drawing calls.

Instead of writing code like “draw a border at `(x, y)` and then draw text at `(x + 1, y + 1)`”, the interface is composed from widgets and layout containers:

* **`Text`** renders aligned and wrapped text
* **`Box`** draws borders and hosts an inner layout container
* **`Row`** splits space horizontally among children
* **`Column`** splits space vertically among children
* **`Grid`** arranges children in a rectangular grid

A root `WidgetTree` owns the top-level container, computes child rectangles recursively, and renders the tree into a `frameBuffer`, which is then written to the terminal.

---

# Current Status

The project is currently in the **core widget / layout / rendering stage**.

## Implemented

* `Widget` base class
* `Container` base class
* `Row` layout container
* `Column` layout container
* `Grid` layout container
* `Box` widget
* `Text` widget
* recursive widget-tree traversal and rendering
* terminal `frameBuffer`
* ANSI cursor / screen control helpers
* Unicode UTF-8 conversion helpers
* basic text wrapping utilities
* text alignment (`left`, `right`, `center`)

## In Progress / Next Focus

* padding and margin
* better layout constraints
* proper render / layout separation cleanup
* clipping / overflow handling
* terminal resize handling
* keyboard input and event propagation
* focus system and interactive widgets

---

# Goals

## Primary Goals

### 1) Declarative terminal UI composition

Build terminal interfaces by composing widgets into a tree instead of manually drawing each element.

### 2) Clear layout containers

Provide reusable layout primitives such as `Row`, `Column`, and `Grid` so complex UIs can be structured cleanly.

### 3) Separation of layout and rendering

Widgets should first compute geometry (`Rect`) and then render based on that geometry.

### 4) Efficient terminal rendering

Use a framebuffer-based rendering model that can later support diff-based or partial redraws.

### 5) Extensible widget system

Make it easy to add new widgets such as buttons, inputs, lists, or progress bars without rewriting the rendering core.

### 6) Modern C++ design

Use C++20 features while keeping the architecture understandable and easy to extend.

---

# Non-Goals

* This is **not** a desktop GUI toolkit for X11, Wayland, Win32, or Cocoa.
* This is **not** a web framework.
* This is **not** an ncurses wrapper.
* This is **not** intended to fully reproduce HTML/CSS layout behavior.
* The project currently prioritizes **rendering architecture, layout design, and control over the terminal pipeline** over API stability.

---

# Architecture

At a high level, the current framework follows this pipeline:

```text
Application code
      │
      ▼
 [ Widget Tree ]
      │
      ▼
 [ Layout Step ]
 - assign root rect
 - containers compute child rects
 - recurse through tree
      │
      ▼
 [ Render Step ]
 - each widget draws into frameBuffer
 - recurse through widget tree
      │
      ▼
 [ Terminal Output ]
 - convert framebuffer cells to UTF-8 output
 - write to stdout
```

The current codebase is centered around **runtime widget objects** connected in a tree, rather than a parser or markup system.

---

# Core Components

## `Widget`

Base abstract class for every drawable UI element.

Current responsibilities:

* stores its rectangle (`Rect`)
* stores a pointer to its parent
* exposes a virtual `render(frameBuffer&)` function

Conceptually:

```cpp
class Widget {
public:
    Rect rect;
    Widget* parent = nullptr;

    virtual void render(frameBuffer& fb) = 0;
    virtual ~Widget() = default;
};
```

---

## `Container`

Base class for widgets that hold child widgets.

Current responsibilities:

* owns `std::vector<std::unique_ptr<Widget>> children`
* provides `addChild`, `removeChild`, and `clearChildren`
* exposes `setRectForChildren()` so derived layout containers can place children

Derived layout containers include:

* `Row`
* `Column`
* `Grid`

---

## `frameBuffer`

Represents the terminal drawing surface.

Current responsibilities:

* stores terminal width and height
* stores the current cell buffer
* stores a previous frame buffer (foundation for future diff rendering)
* supports glyph and style assignment per cell
* produces the final terminal output string

---

## `WidgetTree`

Owns the root container and drives recursive layout/render traversal.

Current responsibilities:

* sets the root rectangle to the terminal size
* recursively calls `setRectForChildren()` through the tree
* renders leaf widgets into the framebuffer
* displays the framebuffer contents

---

## `tools`

Small terminal helper functions for:

* hiding / showing the cursor
* moving the cursor to the home position
* clearing the screen
* setting cursor position

---

## `unicode`

Utility helpers for converting `char32_t` / UTF-32 text into UTF-8 strings for terminal output.

---

# Current Widget System

## `Text`

`Text` is the main text rendering widget.

### Current features

* stores a `std::string`
* supports a `Sytle` object for foreground/background color and text attributes
* supports alignment:

  * `left`
  * `right`
  * `center`
* wraps text using helper functions before drawing into its rectangle

### Current behavior

Text is rendered inside the widget’s assigned rectangle, line by line, with optional alignment per line.

---

## `Box`

`Box` draws a bordered rectangle using a chosen outline style and can host an inner layout container.

### Current features

* multiple border presets:

  * light
  * heavy
  * double border
  * rounded
  * dashed
  * block
  * ascii
* optional inner layout container (`Row`, `Column`, etc.)
* renders children inside the inner content area of the box

### Current behavior

When rendered, `Box`:

1. draws its border
2. shrinks the inner area by one cell on each side
3. assigns that inner rectangle to its internal layout container
4. renders the layout container’s children inside the box

---

## `Row`

Splits the available width among children.

### Current behavior

* divides the parent width equally among all children
* the last child receives the remaining width to avoid truncation from integer division

---

## `Column`

Splits the available height among children.

### Current behavior

* divides the parent height equally among all children
* the last child receives the remaining height

---

## `Grid`

Arranges children into a fixed number of rows and columns.

### Current behavior

* each child is assigned a grid cell based on index
* the last row / column receives any leftover space from integer division

---

# Example

Below is a small example based on the current architecture style of the project.

```cpp
#include <unistd.h>

#include <memory>

#include "ColumnContainer.hpp"
#include "GridContainer.hpp"
#include "RowContainer.hpp"
#include "box.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widgetTree.hpp"

int main() {
  tools::invisiableCursor();
  tools::clearScreen();

  auto MainContainer = std::make_unique<Column>();

  auto text1 = std::make_unique<Text>("Hello neighbor");

  auto innerBox = std::make_unique<Box>(
      std::make_unique<Row>(),
      std::vector<std::unique_ptr<Widget>>{}
  );

  auto text2 = std::make_unique<Text>("Hi");
  innerBox->layoutType->addChild(std::move(text2));

  MainContainer->addChild(std::move(innerBox));
  MainContainer->addChild(std::move(text1));

  WidgetTree tree(std::move(MainContainer));

  for (;;) {
    tools::cursorHomePosition();
    tree.fb.resizeBuffer();
    tree.dfs();
    tree.display();
    usleep(50000);
  }

  tools::visiableCursor();
  return 0;
}
```

This produces a UI tree roughly like:

```text
Column
├── Box
│   └── Row
│       └── Text("Hi")
└── Text("Hello neighbor")
```

---

# Project Structure

The exact file structure may evolve, but the current project is organized around these core files:

```text
.
├── box.hpp
├── ColumnContainer.hpp
├── container.hpp
├── frameBuffer.hpp
├── GridContainer.hpp
├── RowContainer.hpp
├── splitParagraphs.hpp
├── text.hpp
├── tools.hpp
├── unicode.hpp
├── widget.hpp
├── widgetTree.hpp
├── tools.cpp
├── unicode.cpp
└── main.cpp
```

## File Roles

* **`widget.hpp`** — base widget abstraction and `Rect`
* **`container.hpp`** — base class for widgets with children
* **`RowContainer.hpp`** — horizontal layout container
* **`ColumnContainer.hpp`** — vertical layout container
* **`GridContainer.hpp`** — grid layout container
* **`box.hpp`** — bordered box widget with optional inner layout
* **`text.hpp`** — text rendering widget
* **`splitParagraphs.hpp`** — text wrapping helpers
* **`frameBuffer.hpp`** — terminal cell buffer and display pipeline
* **`widgetTree.hpp`** — recursive tree traversal and rendering
* **`tools.hpp` / `tools.cpp`** — ANSI terminal helper functions
* **`unicode.hpp` / `unicode.cpp`** — UTF-8 conversion helpers
* **`main.cpp`** — example / testing entry point

---

# Roadmap

The roadmap below reflects the **current actual direction of the project**.

---

## Phase 1 — Core Rendering Foundation

* [x] `Widget` base class
* [x] `Container` base class
* [x] `Text` widget
* [x] `Box` widget
* [x] `Row` layout container
* [x] `Column` layout container
* [x] `Grid` layout container
* [x] recursive widget tree rendering
* [x] terminal framebuffer
* [x] Unicode-aware terminal output helpers

---

## Phase 2 — Layout System Refinement

* [ ] padding support
* [ ] margin support
* [ ] child alignment inside containers
* [ ] widget size constraints (`fixed`, `auto`, `fill`)
* [ ] min / max size constraints
* [ ] better clipping / overflow handling
* [ ] cleaner layout-pass structure

---

## Phase 3 — Text System Improvements

* [x] basic paragraph wrapping helpers
* [x] left / right / center text alignment
* [ ] better text measurement
* [ ] multi-style / rich text support
* [ ] paragraph widget
* [ ] text clipping / ellipsis behavior

---

## Phase 4 — Runtime / Rendering Improvements

* [ ] terminal resize handling
* [ ] safer framebuffer bounds handling
* [ ] diff-based redraw using `previousFrame`
* [ ] partial redraw / dirty-cell optimizations
* [ ] cleaner render loop abstraction

---

## Phase 5 — Input & Events

* [ ] non-blocking keyboard input
* [ ] event abstraction (`KeyPress`, `Resize`, etc.)
* [ ] event propagation through widget tree
* [ ] focus management
* [ ] focus traversal / tab navigation

---

## Phase 6 — Interactive Widgets

* [ ] button
* [ ] checkbox
* [ ] radio group
* [ ] toggle switch
* [ ] menu / list selection widgets

---

## Phase 7 — Input Widgets

* [ ] text input
* [ ] password input
* [ ] text area
* [ ] cursor editing behavior
* [ ] selection / editing state management

---

## Phase 8 — Advanced Layout & Containers

* [ ] scrollable containers
* [ ] split panes
* [ ] stack / overlay containers
* [ ] more flexible grid sizing
* [ ] flex-style layout system

---

## Phase 9 — Data / Complex Widgets

* [ ] list view
* [ ] table widget
* [ ] tree view
* [ ] tabs
* [ ] progress bars
* [ ] modal / popup support

---

## Phase 10 — API & Ecosystem

* [ ] declarative builder helpers for cleaner widget construction
* [ ] template-based UI composition helpers
* [ ] style / theme system
* [ ] markdown renderer
* [ ] syntax-highlighted code widget
* [ ] documentation demos and examples

---

# Future Plans

The project is **not** moving toward a JSX / parser-based architecture.
The more likely long-term direction is a **cleaner declarative C++ API** built using **templates and helper builders**.

## 1) Cleaner C++ construction API

The current construction style is functional, but verbose because it relies heavily on `std::make_unique` and explicit container setup.

A future goal is to allow code like:

```cpp
auto ui = box(
    column(
        text("Dashboard"),
        row(
            box(text("CPU")),
            box(text("RAM")),
            box(text("Disk"))
        )
    )
);
```

This would keep the framework fully in C++ while making UI construction feel more declarative.

---

## 2) Better layout constraints

The current layout containers divide space equally.
A future layout system should support things like:

* fixed sizes
* fill / flex behavior
* minimum and maximum sizes
* padding / margin
* alignment within parent containers

---

## 3) Interactive widgets and input handling

Once the layout and rendering core is stable, the next major milestone is building actual terminal applications with:

* keyboard input
* focus handling
* buttons
* inputs
* menus
* scrollable views

---

## 4) Better rendering performance

The current framebuffer already stores a `previousFrame`, which can later be used for:

* diff-based redraw
* reduced terminal writes
* dirty-cell tracking
* more efficient rendering loops

---

## 5) Stronger styling system

The project already has a basic `Sytle` structure for foreground/background colors and attributes.
This can grow into a more complete styling system with:

* widget-level style presets
* theme support
* state-based styles for focus / active / disabled widgets

---

# Contributing

Contributions, suggestions, and architecture feedback are welcome.

If you want to contribute:

1. Fork the repository
2. Create a feature branch
3. Keep the code C++20-compatible
4. Prefer focused pull requests
5. If you change architecture or public API direction, document the reasoning clearly in the PR description

---

# License

This project is licensed under the [MIT License](LICENSE).

---

# Getting Started

## Requirements

* C++20 compiler
* CMake 3.28+
* Ninja recommended

## Clone the repository

```bash
git clone https://github.com/AlterWill/TUI.git
cd TUI
```

## Build

```bash
rm -rf build
cmake -G Ninja -B build
cmake --build build
```

## Run

```bash
./build/tui
```

---

## Suggested Reading Order for the Codebase

If you want to understand the project internals, a good reading order is:

1. `widget.hpp`
2. `container.hpp`
3. `RowContainer.hpp`, `ColumnContainer.hpp`, `GridContainer.hpp`
4. `box.hpp`
5. `text.hpp`
6. `frameBuffer.hpp`
7. `widgetTree.hpp`
8. `main.cpp`

---

## Suggested Next Milestones for Development

If you are actively building the framework, the most useful next steps are:

1. **Padding + margin**
2. **Layout constraints (`fixed`, `fill`, `auto`)**
3. **Clipping / overflow handling**
4. **Terminal resize handling**
5. **Keyboard event system**
6. **Focus management**
7. **Button / input widgets**
8. **Cleaner template-based construction helpers**

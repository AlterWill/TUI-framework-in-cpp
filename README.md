# TUI Library

A C++ terminal UI framework with a retained widget tree, constraint-based layout, and incremental cell-diff rendering.

## Overview

This is a modern TUI framework for building terminal-based interfaces in C++. It provides a structured widget hierarchy, flexible layout containers (`Row`, `Column`, `Grid`, `Stack`, `Scroll`), styled text rendering, keyboard and mouse input, and an incremental rendering pipeline that only redraws cells that change between frames.

## What Is Currently Implemented

### Core Framework

- **`Widget`** — pure abstract interface (`measure`, `layout`, `render`, `handleEvent`)
- **`WidgetBase`** — common widget state (focus, padding)
- **`LayoutNode`** — tree node wrapping a child `widget`, `width`/`height` `SizeSpec`, `SizeConstraints`, cached `measured` size, allocated `rect`, `margin`, and horizontal/vertical alignments
- **`SingleChildWidget` & `MultiChildWidget`** — base structs providing automatic top-down layout traversal and generic child rendering
- **`WidgetTree`** — root manager owning the widget tree, driving layout, render, and display passes
- **`LinearLayoutSolver`** — shared 4-step priority measurement engine:
  1. `Fixed` — exact dimensions
  2. `Percentage` — fraction of parent available space
  3. `Content` — intrinsic preferred sizing with proportional Clay-style deficit shrink
  4. `Flex` — proportional allocation of remaining space

### Display Widgets

- **`Text`** — multi-line text widget with word-wrapping, alignment (`Left`, `Center`, `Right`), styles, and fluent builder methods (`.withStyle()`, `.withAlignment()`, `.withPadding()`)
- **`Box`** — bordered container with configurable border styles (`light`, `heavy`, `double`, `rounded`, `dashed`, `block`, `ascii`), padding, child margins, title, and fluent builder methods

### Layout Containers

- **`Row`** — horizontal linear container powered by `LinearLayoutSolver` with gap, padding, and vertical child alignments (`Top`, `Center`, `Bottom`)
- **`Column`** — vertical linear container powered by `LinearLayoutSolver` with gap, padding, and horizontal child alignments (`Left`, `Center`, `Right`)
- **`Grid<Rows, Cols>`** — compile-time 2D grid using `std::array` with row/column gaps, cell alignments, and builder methods
- **`Stack`** — overlapping layer container with independent horizontal/vertical alignments per child
- **`Scroll`** — viewport scroll container with virtual off-screen `Buffer` rendering, viewport blitting, and mouse wheel event handling

### Rendering

- **`Surface`** (`surface.hpp`) manages double buffers (`current` and `previous` frames)
- **`Buffer`** (`buffer.hpp`) — flat cell grid with `blitTo()` for region copies
- **`RenderContext`** (`renderContext.hpp`) — render pass context passed to widgets
- **Incremental display** (`incrementDisplay()`) diffs `current` vs. `previous` buffer and emits only changed cells as ANSI escape sequences
- **Full redraw** (`display()`) available for forced full renders

### Color and Styling

- **`Colour`** — ARGB color with true-color (24-bit), 256-color, and 16-color fallback output
- **`ColourPair`** — foreground/background color pair
- **`NamedColour`** — named palette constants
- **`Style`** — color pair plus text style flags: Bold, Dim, Italic, Underline, Blink, Reverse, Hidden, StrikeThrough

### Input and Events

- **`backend`** — abstract terminal backend interface
- **`linux_backend`** — Linux implementation with raw mode and SGR mouse protocol
- **`keyEvent`** / **`MouseEvent`** — keyboard and mouse event definitions (`Event = std::variant<keyEvent, MouseEvent>`)

---

## Directory Structure

```text
include/
├── core/
│   ├── layoutNode.hpp       # LayoutNode struct (bounds, specs, margins, alignments)
│   ├── multiChildWidget.hpp # MultiChildWidget base with generic render
│   ├── singleChildWidget.hpp# SingleChildWidget base with generic render
│   ├── widget.hpp           # Widget interface & WidgetBase
│   └── widgetTree.hpp       # WidgetTree root manager
├── input/
│   ├── event.hpp            # Event definitions (keyEvent, MouseEvent)
│   └── eventHandler.hpp     # Event dispatcher & focus traversal
├── layout/
│   ├── ColumnContainer.hpp  # Vertical linear container (Column)
│   ├── GridContainer.hpp    # Fixed-size 2D grid container (Grid<Rows, Cols>)
│   ├── LinearLayoutSolver.hpp # Shared 4-step layout solver engine
│   ├── RowContainer.hpp     # Horizontal linear container (Row)
│   ├── ScrollContainer.hpp  # Scrollable viewport container (Scroll)
│   ├── Size.hpp             # 2D Size struct
│   ├── SizeConstraints.hpp  # Size constraints (minSize, maxSize)
│   ├── StackContainer.hpp   # Overlapping stack container (Stack)
│   └── sizeType.hpp         # SizeType (Fixed, Percentage, Content, Flex) & SizeSpec
├── rendering/
│   ├── buffer.hpp           # Cell grid buffer
│   ├── cell.hpp             # Glyph and Style cell
│   ├── renderContext.hpp    # Context passed to render()
│   └── surface.hpp          # Framebuffer with incremental diffing
├── styling/
│   ├── colour.hpp           # ARGB colour representation
│   ├── colourPair.hpp       # Fg/Bg colour pair
│   ├── named_colour.hpp     # Palette constants
│   └── style.hpp            # Style struct
├── terminal/
│   ├── backend.hpp          # Backend interface
│   ├── linux_backend.hpp    # Linux terminal backend
│   └── tools.hpp            # ANSI terminal escape utilities
├── utilities/
│   ├── Point.hpp            # 2D point
│   ├── Rect.hpp             # 2D rectangle
│   ├── alignment.hpp        # Horizontal & vertical alignment enums
│   ├── insets.hpp           # Padding & margin insets
│   ├── splitParagraphs.hpp  # Paragraph line-wrapping utilities
│   ├── unicode.hpp          # UTF-8 & unicode display widths
│   └── logger.hpp           # Compile-time toggled diagnostic logger
└── widgets/
    ├── box.hpp              # Bordered box container widget
    └── text.hpp             # Multi-line styled text widget

src/
└── main.cpp                 # Main executable demonstration
```

---

## Getting Started

### Prerequisites

- C++20 compliant **Clang** compiler (`clang++`)
- CMake 3.28+
- Ninja (recommended) or Make
- Linux terminal

### Building & Running

```bash
git clone https://github.com/AlterWill/TUI.git
cd TUI

cmake -G Ninja -B build
cmake --build build

./build/tui
```

Press `q` to exit.

---

## License

MIT License — see the LICENSE file for details.

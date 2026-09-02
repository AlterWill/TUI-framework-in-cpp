# TUI Library

A C++ terminal UI framework with a retained widget tree, constraint-based layout, and incremental cell-diff rendering.

## Overview

This is an early-stage TUI framework for building terminal-based interfaces in C++. It provides a structured widget hierarchy, several layout containers, styled text rendering, keyboard and mouse input, and an incremental rendering pipeline that only redraws cells that change between frames.

The project is under active development. There are no interactive widgets yet (buttons, inputs, etc.), and some systems are partially implemented or stubbed.

## What Is Currently Implemented

### Core Framework

- `Widget` base class with `Rect` bounds, padding, margin, and focus state
- `SingleChildWidget` and `MultiChildWidget` base classes for composing trees
- `WidgetTree` — root manager that owns the widget tree and drives layout/render/display passes
- Constraint-based layout via `SizeConstraints` (`minSize` / `maxSize`) and per-widget flex values
- `Surface` — double-buffered framebuffer; holds `current` and `previous` cell grids, diffs them on each frame, and flushes only dirty cells to the terminal

### Display Widgets

- `Text` — multi-line text widget with left-aligned rendering and word wrapping. Center and right alignment are stubbed but not yet implemented.
- `Box` — bordered container with configurable border styles (`light`, `heavy`, `double`, `rounded`, `dashed`, `block`, `ascii`), padding, and background fill

### Layout Containers

- `Row` — horizontal layout; divides width equally among children, respects margin and padding
- `Column` — vertical layout; divides height equally among children, respects margin and padding
- `Grid` — 2D matrix layout with configurable row/column counts
- `Flex` — flex-ratio-based layout along a row or column axis (note: constructor is currently private; cannot be instantiated directly)
- `Stack` — overlapping layout; all children receive the same rect as the parent

### Not Yet Implemented (Stubs)

- `Scroll` (`ScrollContainer.hpp`) — class stub exists with no body; scroll offset and viewport logic are not written
- `react_dsl.hpp` — file exists but is empty; no DSL is implemented

### Rendering

- `Surface` (`surface.hpp`) manages two `Buffer` instances (current and previous frames)
- `Buffer` (`buffer.hpp`) — flat cell grid with `blitTo()` for region copies
- `RenderContext` (`renderContext.hpp`) — thin wrapper passed to `render()`; widgets write cells through it. No clip stack is implemented.
- Incremental display (`incrementDisplay()`) diffs current vs. previous buffer and emits only changed cells as ANSI escape sequences
- Full redraw (`display()`) available for initial or forced full renders
- `resizeBuffer()` re-queries terminal size and resets the buffers on dimension change

### Color and Styling

- `Colour` — ARGB color with true-color (24-bit), 256-color, and 16-color fallback output
- `ColourPair` — foreground/background color pair
- `NamedColour` — named palette constants
- `Style` — color pair plus text style flags: Bold, Dim, Italic, Underline, Blink, Reverse, Hidden, StrikeThrough
- Color tier auto-detected from `COLORTERM` and `TERM` environment variables at startup

### Input and Events

- `backend` — abstract terminal backend interface (`readEvent()`, `findTerminalSize()`, etc.)
- `linux_backend` — Linux implementation: raw mode terminal, non-blocking stdin read, SGR mouse protocol
- `keyEvent` — keyboard event with key code and modifier flags (Ctrl, Shift, Alt)
- `MouseEvent` — mouse event with action (press, release, move, drag, scroll up/down), button, and coordinates
- `EventDispatcher` (`eventHandler.hpp`) — dispatches keyboard events to the focused widget via bubble propagation; dispatches mouse events by hit-testing the widget tree with DFS
- Focus traversal: `nextFocus()` / `previousFocus()` walk a flat focusable-widget list; wiring to Tab/Shift-Tab must be done in the application loop

### Logging

- `tui::Logger` — compile-time toggled singleton logger. Enable by building with `-DENABLE_LOGGING`. Writes to `tui_debug.log`. Zero-overhead when disabled (`if constexpr`).

## Directory Structure

```text
.
├── include/
│   ├── backend.hpp              # Abstract terminal backend interface
│   ├── linux_backend.hpp        # Linux raw mode terminal & SGR mouse input
│   ├── buffer.hpp               # Flat cell grid (Buffer class)
│   ├── surface.hpp              # Double-buffered framebuffer & differential flush (Surface)
│   ├── renderContext.hpp        # Thin render pass context passed to widgets
│   ├── cell.hpp                 # Framebuffer cell (glyph + Style)
│   ├── colour.hpp               # ARGB color with true-color/256/16 output
│   ├── colourPair.hpp           # Foreground/background color pair
│   ├── named_colour.hpp         # Named color palette constants
│   ├── style.hpp                # Style struct (ColourPair + text style flags)
│   ├── Point.hpp                # 2D coordinate
│   ├── Rect.hpp                 # Rectangle bounds
│   ├── Size.hpp                 # 2D dimensions
│   ├── SizeConstraints.hpp      # Min/max layout constraints & LayoutProperties
│   ├── insets.hpp               # Padding/margin dimensions
│   ├── event.hpp                # keyEvent, MouseEvent, Event variant
│   ├── eventHandler.hpp         # EventDispatcher with focus traversal
│   ├── widget.hpp               # Abstract Widget base class
│   ├── widgetTree.hpp           # WidgetTree: root manager & render loop driver
│   ├── singleChildWidget.hpp    # Single-child widget base
│   ├── multiChildWidget.hpp     # Multi-child widget base
│   ├── text.hpp                 # Text widget (left alignment implemented)
│   ├── box.hpp                  # Bordered container widget
│   ├── RowContainer.hpp         # Horizontal layout (Row)
│   ├── ColumnContainer.hpp      # Vertical layout (Column)
│   ├── GridContainer.hpp        # Grid layout (Grid)
│   ├── FlexContainer.hpp        # Flex-ratio layout (Flex) — constructor currently private
│   ├── StackContainer.hpp       # Overlapping stack layout (Stack)
│   ├── ScrollContainer.hpp      # Scroll container stub (not yet implemented)
│   ├── react_dsl.hpp            # Empty — DSL not yet implemented
│   ├── splitParagraphs.hpp      # String splitting utilities
│   ├── unicode.hpp              # UTF-8 encode/decode & display-width calculation
│   ├── logger.hpp               # Compile-time-toggled file logger
│   └── tools.hpp                # Terminal control escape utilities
├── src/
│   └── main.cpp                 # Demo: Text widget render loop with frame timing output
├── CMakeLists.txt               # Build configuration (requires clang++)
├── README.md
├── ROADMAP.md
├── TODO.md
└── plan.md                      # Architecture notes & future design ideas
```

## Getting Started

### Prerequisites

- C++20 compliant **Clang** compiler (`clang++`) — the build explicitly sets `CMAKE_CXX_COMPILER=clang++`
- CMake 3.28+
- Ninja (recommended) or Make
- Linux terminal — the only implemented backend is `linux_backend`

### Building

```bash
git clone https://github.com/AlterWill/TUI.git
cd TUI

cmake -G Ninja -B build
cmake --build build

./build/tui
```

Press `q` to exit. The demo prints startup timing in milliseconds after exiting.

### Enabling the Logger

```bash
cmake -G Ninja -B build -DENABLE_LOGGING=ON
cmake --build build
./build/tui
# Diagnostic output written to tui_debug.log
```

## Render Pipeline

Each frame follows this sequence:

1. `resizeBuffer()` — re-queries terminal dimensions; reallocates buffers if size changed
2. `layout(rect)` — recursively sets `Rect` bounds on each widget from the root down
3. `render()` — widgets write styled cells into `RenderContext` (wraps the current `Buffer`)
4. `incrementDisplay()` — diffs `current` vs `previous` buffer; emits ANSI sequences only for changed cells; copies current to previous
5. `readEvent()` — non-blocking stdin read; returns `keyEvent` or `MouseEvent` (or nothing)

## Usage Example

```cpp
#include "terminal/linux_backend.hpp"
#include "widgets/text.hpp"
#include "terminal/tools.hpp"
#include "core/widgetTree.hpp"

int main() {
    linux_backend terminal(/*keyboard=*/true, /*mouse=*/true);

    tools::alternateScreenBuffer();
    tools::clearScreen();
    tools::invisiableCursor();

    auto root = std::make_unique<Text>(
        "Hello, TUI!",
        Style{.colours = {.fg = NamedColour::Orange}},
        Alignment::left
    );

    WidgetTree tree(std::move(root), terminal);

    bool running = true;
    while (running) {
        tools::cursorHomePosition();
        tree.fb.resizeBuffer();
        tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});
        tree.render();
        tree.display();

        auto event = terminal.readEvent();
        if (event) {
            if (auto key = std::get_if<keyEvent>(&event.value())) {
                if (key->key == 'q') running = false;
            }
        }
    }

    tools::visiableCursor();
    tools::clearScreen();
    return 0;
}
```

## Current Limitations

- **Linux only** — no Windows or macOS backend
- **No interactive widgets** — no Button, TextInput, Checkbox, etc.
- **`Scroll` container is a stub** — the class exists but has no working implementation
- **`Flex` constructor is private** — `Flex` cannot be instantiated directly
- **`Text` only renders left-aligned text** — center and right alignment are not implemented
- **No terminal resize events** — `resizeBuffer()` detects size changes on each frame, but there is no `SIGWINCH`-based event dispatch
- **No theme or global styling system**
- **No focus scopes** — Tab traversal is global across the entire widget tree
- **No automated tests** and no headless backend for CI testing
- **No DSL** — `react_dsl.hpp` is an empty file

## Further Reading

- [ROADMAP.md](ROADMAP.md) — development milestones and planned features
- [TODO.md](TODO.md) — concrete implementation tasks with completion criteria
- [plan.md](plan.md) — architecture notes and future design ideas

## License

MIT License — see the LICENSE file for details.

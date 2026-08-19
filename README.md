# TUI Library

A C++ terminal UI framework built around a retained widget tree, incremental rendering, and a clean separation between layout, events, and terminal output.

## Overview

This project is a TUI framework for building terminal-based interfaces with reusable widgets, flexible layouts, and a declarative composition style. The goal is to make terminal UIs feel structured and scalable while remaining lightweight and fast.

The library currently provides core framework runtime features:

* Retained widget trees
* Layout and render passes with constraint resolution
* Framebuffer-based drawing with double buffering
* ANSI output with 256-color and true-color support
* Incremental redraw with dirty cell tracking
* UTF-8 rendering & Unicode width calculation
* Keyboard and mouse event handling with event propagation
* Focus management and Tab navigation
* Declarative React-like DSL

## Design Goals

* **Retained widget tree**: Widgets maintain their identity, hierarchy, and state across frames.
* **Separation of concerns**: Layout, rendering, input, focus, and styling operate cleanly as independent systems.
* **Incremental rendering**: Only dirty cells that actually change between frames are flushed to the terminal.
* **Extensibility**: Architectural support for containers (Row, Column, Grid, Flex, Stack, Scroll) and interactive widgets (Buttons, Inputs, Dialogs).
* **Practicality**: High-performance runtime suitable for real-world terminal applications.
* **Backend flexibility**: Abstraction layers for terminal backends (Linux backend with ANSI escape sequences).

## Current State

The implementation includes:

* `Widget` base class with hierarchy and bounds tracking (`Rect`, `Insets`)
* `WidgetTree` for root layout orchestration and double-buffered rendering
* Single-child (`SingleChildWidget`) and multi-child (`MultiChildWidget`) base containers
* Constraint-based layout engine (`SizeConstraints`, `measure()`, `layout()`)
* Container Layouts: `Row`, `Column`, `Grid`, `Flex`, `Stack`, `ScrollContainer`
* Display Widgets: `Text`, `Box` (with customizable borders and padding)
* Framebuffer & Cell state management (`frameBuffer.hpp`, `cell.hpp`)
* Incremental double-buffering & dirty cell diffing
* Terminal Backends: `linux_backend` with non-blocking raw mode support
* Color output: `Colour`, `ColourPair`, `NamedColour`, 256-color, and True-color (RGB) support
* Event Handling: Keyboard (`keyEvent`) and Mouse (`MouseEvent`) dispatch with propagation
* Focus System: `FocusManager` with Tab / Shift-Tab focus traversal
* React-inspired DSL (`react_dsl.hpp`) for declarative UI construction

## Directory Structure

```text
.
├── include/
│   ├── backend.hpp              # Abstract terminal backend interface
│   ├── linux_backend.hpp        # Linux terminal raw mode & event reader
│   ├── cell.hpp                 # Framebuffer cell representation
│   ├── colour.hpp               # RGB & 256-color definitions
│   ├── colourPair.hpp           # Foreground / background color pairs
│   ├── named_colour.hpp         # Standard palette enum constants
│   ├── Point.hpp                # 2D coordinate structure
│   ├── Rect.hpp                 # Rectangle bounds & geometry helper
│   ├── Size.hpp                 # 2D dimensions
│   ├── SizeConstraints.hpp      # Min/max layout constraints
│   ├── insets.hpp               # Padding & margin dimensions
│   ├── style.hpp                # Widget styling attributes
│   ├── renderContext.hpp        # Render buffer drawing context & clipping
│   ├── frameBuffer.hpp          # Double buffer & cell differential flusher
│   ├── event.hpp                # Keyboard and mouse event primitives
│   ├── eventHandler.hpp         # Event dispatcher & focus manager
│   ├── widget.hpp               # Abstract base Widget class
│   ├── widgetTree.hpp           # Root tree manager & render loop driver
│   ├── singleChildWidget.hpp    # Single child widget base class
│   ├── multiChildWidget.hpp     # Multi child widget base class
│   ├── text.hpp                 # Multi-line text & alignment widget
│   ├── box.hpp                  # Bordered container widget
│   ├── RowContainer.hpp         # Horizontal linear layout
│   ├── ColumnContainer.hpp      # Vertical linear layout
│   ├── GridContainer.hpp        # Matrix grid layout
│   ├── FlexContainer.hpp        # Flexbox ratio-based layout
│   ├── StackContainer.hpp       # Z-index / overlapping stack container
│   ├── ScrollContainer.hpp      # Viewport scroll container
│   ├── react_dsl.hpp            # Declarative DSL helper macro/functions
│   ├── splitParagraphs.hpp      # UTF-8 text wrapping helpers
│   ├── unicode.hpp              # UTF-8 character decoding & width calculation
│   └── tools.hpp                # Terminal control escape utilities
├── src/
│   └── main.cpp                 # Application entry point & demo benchmark
├── CMakeLists.txt               # Build configuration
├── README.md                    # Framework overview & docs
├── ROADMAP.md                   # Feature development roadmap
├── TODO.md                      # Detailed task roadmap
└── plan.md                      # Architecture design notes & raw ideas
```

## Getting Started

### Prerequisites

* C++20 compliant compiler (GCC, Clang)
* CMake 3.20+
* Ninja (recommended) or Make

### Building and Running

```bash
# Clone the repository
git clone https://github.com/AlterWill/TUI.git
cd TUI

# Clean any existing build directory (optional)
rm -rf build

# Configure using CMake with Ninja generator
cmake -G Ninja -B build

# Build the binary
cmake --build build

# Run the application demo
./build/tui
```

## Render Flow

A frame follows this execution pipeline:

1. Read input events from `Backend` non-blocking stdin
2. Convert raw escape codes into `KeyEvent` / `MouseEvent`
3. Dispatch events to focused widget or propagate down widget tree
4. Update application state on user input
5. Run layout pass: `measure()` constraints and set target `Rect` bounds
6. Run render pass: widgets write styled glyphs into `RenderContext`
7. Diff current `Framebuffer` against previous frame to identify dirty cells
8. Flush dirty cell ANSI color & cursor escape sequences to the terminal backend

## Declarative DSL Example

```cpp
#include "react_dsl.hpp"
#include "tui.hpp" // main framework include

int main() {
    // Declarative UI tree creation using DSL syntax
    auto app = box(
        Column({
            Text("TUI Framework Demo", Style{.fg = NamedColour::Cyan}, Alignment::center),
            Row({
                Text("Left Column"),
                Text("Right Column")
            })
        })
    );

    // Run main application loop
    // (See src/main.cpp for full integration example)
}
```

## Roadmap Overview

### Milestone 1: MVP Runtime (Completed ✅)
* [x] Retained widget tree & base classes
* [x] Layout pass (`measure` & `layout`)
* [x] Framebuffer & UTF-8 renderer
* [x] Basic containers (`Row`, `Column`, `Grid`)
* [x] Double buffering & dirty cell diffing
* [x] ANSI color & 256 / true-color output

### Milestone 2: Interactive Core (In Progress 🚧)
* [x] Keyboard & Mouse input handling
* [x] Event dispatcher & propagation
* [x] Focus manager & Tab traversal
* [x] Flex, Stack, and Scroll layout containers
* [x] Constraint-based layout resolution
* [ ] Terminal resize handling (`SIGWINCH`)
* [ ] Focus scopes & modal focus containment
* [ ] Theme manager & global styling rules

### Milestone 3: Standard Widget Library (Planned 📋)
* [ ] Interactive controls: `Button`, `Checkbox`, `Radio`, `Slider`
* [ ] Input controls: `TextInput`, `TextArea`, `PasswordInput`
* [ ] Display controls: `ProgressBar`, `Spinner`, `Divider`, `Spacer`
* [ ] Data controls: `ListView`, `Table`, `TreeView`, `Tabs`
* [ ] Dialogs & Overlays: `Popup`, `Modal`, `Tooltip`, `Notification`

See [ROADMAP.md](file:///home/alterwill/Github/TUI/ROADMAP.md) for detailed phase breakdowns and [TODO.md](file:///home/alterwill/Github/TUI/TODO.md) for active developer tasks.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

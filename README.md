# TUI Library

A C++ terminal UI framework with a retained widget tree, constraint-based layout, and incremental cell-diff rendering.

## Overview

This is a modern TUI framework for building terminal-based interfaces in C++. It provides a structured widget hierarchy, flexible layout containers (`Row`, `Column`, `Grid`, `Stack`, `Scroll`, `SplitPane`, `Overlay`), styled text rendering, keyboard and mouse input, and an incremental rendering pipeline that only redraws cells that change between frames.

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
- **`SplitPane`** — two-pane container with a `Horizontal`/`Vertical` orientation, a configurable split ratio (clamped by min/max), and a draggable divider bar. Divider appearance is chosen from the `dividerStyle` namespace (`light`, `heavy`, `doubleBorder`, `dashed`, `block`, `ascii`, `none`)
- **`Overlay`** — built on `Stack`, manages a base application layer plus a Z-ordered list of popups, modals, dropdowns, and tooltips. Supports modal click-outside-to-dismiss and per-overlay dismissal callbacks

### Interactive Widgets

- **`Button`** — focusable, clickable button with press/release callbacks and fluent builder
- **`Checkbox`** — toggleable boolean control with customizable glyph styles (`checkboxStyle`)
- **`RadioGroup`** — group of mutually-exclusive radio buttons with selection callback
- **`Toggle`** — on/off toggle switch with visual indicator and callback
- **`Slider`** — read-only numeric value slider with styled track/thumb
- **`InteractiveSlider`** — focusable, keyboard- and mouse-draggable slider with `onChange` callback and `sliderStyle` customization

### Input Widgets

- **`TextInput`** — single- or multi-line text field with:
  - External `std::string` binding or internal state
  - Placeholder text with dimmed style
  - Password masking (any `char32_t` mask character)
  - Three cursor shapes: `Block`, `Beam`, `Underline`
  - Full keyboard editing: arrows, Home/End, Backspace, Delete, Enter
  - Mouse click-to-position support
  - `onChange` / `onSubmit` callbacks
  - Pluggable `Validator` and `InputFilter` functions
  - Fluent builder: `.withText()`, `.withPlaceholder()`, `.withMask()`, `.withMultiline()`, `.withStyle()`, `.withCursorShape()`, etc.
- **`TextArea`** — convenience factory returning a `TextInput` with `isMultiline = true`

### Basic Widgets

- **`Divider`** — horizontal/vertical separator with preset glyph styles (`dividerStyle`)
- **`Spacer`** — zero-content flex spacer for pushing siblings apart
- **`SingleCharSpinner`** — animated spinner cycling through character frames (`singleSpinnerStyle`)
- **`ProgressBar`** — styled fill-bar widget with configurable styles (`progressBarStyles`)

### Animation System

- **`animatableWidget`** — abstract base for time-driven widgets; exposes `update(time_point)`
- **`AnimationManager`** — owns a list of `animatableWidget*` pointers, calling `update()` on each per-frame tick

### Rendering

- **`Surface`** (`surface.hpp`) manages double buffers (`current` and `previous` frames)
- **`Buffer`** (`buffer.hpp`) — flat cell grid with `blitTo()` for region copies
- **`RenderContext`** (`renderContext.hpp`) — render pass context passed to widgets
- **Incremental display** (`incrementDisplay()`) diffs `current` vs. `previous` buffer and emits only changed cells as ANSI escape sequences
- **Full redraw** (`display()`) available for forced full renders
- **`ansi`** namespace (`ansi.hpp`) — centralized ANSI escape helpers (cursor movement, screen clearing, alternate screen buffer, style reset)

### Color and Styling

- **`Colour`** — ARGB color with true-color (24-bit), 256-color, and 16-color fallback output
- **`ColourPair`** — foreground/background color pair
- **`NamedColour`** — named palette constants
- **`Style`** — color pair plus text style flags: Bold, Dim, Italic, Underline, Blink, Reverse, Hidden, StrikeThrough
- **`lerpColour` / `generateGradient`** (`gradientColorHelper.hpp`) — linear color interpolation and multi-stop gradient generation helpers

### Input and Events

- **`backend`** — abstract terminal backend interface
- **`linux_backend`** — Linux implementation with raw mode and SGR mouse protocol
- **`keyEvent`** / **`MouseEvent`** — keyboard and mouse event definitions (`Event = std::variant<keyEvent, MouseEvent>`)

### Utilities

- **`validators`** namespace — reusable `TextInput` validators: `integer()`, `number()`, `email()`
- **`filters`** namespace — reusable `TextInput` input filters: `digits()`
- **`checkboxStyle`** — glyph style presets for `Checkbox`
- **`dividerStyle`** — glyph style presets for `Divider` and `SplitPane`
- **`progressBarStyles`** — fill/track glyph presets for `ProgressBar`
- **`singleSpinnerStyle`** — frame-sequence presets for `SingleCharSpinner`
- **`sliderStyle`** — track/thumb glyph presets for `Slider`
- **`sliderDrag`** — drag-state helper for `InteractiveSlider`
- **`Orientation`** — `Horizontal`/`Vertical` enum shared by layout and widgets

---

## Directory Structure

```text
include/
├── animation/
│   ├── animationEntry.hpp    # Per-widget animation registration entry
│   └── animationManager.hpp  # Frame-tick animation manager (animatableWidget list)
├── core/
│   ├── layoutNode.hpp        # LayoutNode struct (bounds, specs, margins, alignments)
│   ├── multiChildWidget.hpp  # MultiChildWidget base with generic render
│   ├── singleChildWidget.hpp # SingleChildWidget base with generic render
│   ├── widget.hpp            # Widget interface & WidgetBase
│   └── widgetTree.hpp        # WidgetTree root manager
├── input/
│   ├── event.hpp             # Event definitions (keyEvent, MouseEvent)
│   └── eventHandler.hpp      # Event dispatcher & focus traversal
├── layout/
│   ├── ColumnContainer.hpp   # Vertical linear container (Column)
│   ├── GridContainer.hpp     # Fixed-size 2D grid container (Grid<Rows, Cols>)
│   ├── LinearLayoutSolver.hpp# Shared 4-step layout solver engine
│   ├── Overlay.hpp           # Stack-based overlay container (Overlay)
│   ├── RowContainer.hpp      # Horizontal linear container (Row)
│   ├── ScrollContainer.hpp   # Scrollable viewport container (Scroll)
│   ├── Size.hpp              # 2D Size struct
│   ├── SizeConstraints.hpp   # Size constraints (minSize, maxSize)
│   ├── SplitPane.hpp         # Two-pane resizable container (SplitPane)
│   ├── StackContainer.hpp    # Overlapping stack container (Stack)
│   └── sizeType.hpp          # SizeType (Fixed, Percentage, Content, Flex) & SizeSpec
├── rendering/
│   ├── buffer.hpp            # Cell grid buffer
│   ├── cell.hpp              # Glyph and Style cell
│   ├── renderContext.hpp     # Context passed to render()
│   └── surface.hpp           # Framebuffer with incremental diffing
├── styling/
│   ├── colour.hpp            # ARGB colour representation
│   ├── colourPair.hpp        # Fg/Bg colour pair
│   ├── named_colour.hpp      # Palette constants
│   └── style.hpp             # Style struct
├── terminal/
│   ├── ansi.hpp              # Centralized ANSI escape sequence helpers
│   ├── backend.hpp           # Backend interface
│   └── linux_backend.hpp     # Linux terminal backend
├── utilities/
│   ├── Orientation.hpp       # Horizontal/Vertical orientation enum
│   ├── Point.hpp             # 2D point
│   ├── Rect.hpp              # 2D rectangle
│   ├── alignment.hpp         # Horizontal & vertical alignment enums
│   ├── checkboxStyle.hpp     # Glyph style presets for Checkbox
│   ├── dividerStyle.hpp      # Glyph style presets for Divider & SplitPane
│   ├── gradientColorHelper.hpp # lerpColour & generateGradient helpers
│   ├── insets.hpp            # Padding & margin insets
│   ├── logger.hpp            # Compile-time toggled diagnostic logger
│   ├── progressBarStyles.hpp # Fill/track glyph presets for ProgressBar
│   ├── singleSpinnerStyle.hpp# Frame presets for SingleCharSpinner
│   ├── sliderDrag.hpp        # Drag-state helper for InteractiveSlider
│   ├── sliderStyle.hpp       # Track/thumb glyph presets for Slider
│   ├── splitParagraphs.hpp   # Paragraph line-wrapping utilities
│   ├── unicode.hpp           # UTF-8 & unicode display widths
│   └── validators.hpp        # TextInput validators (integer, number, email) & filters
└── widgets/
    ├── animatebleWidget.hpp  # Abstract base for time-driven animated widgets
    ├── box.hpp               # Bordered box container widget
    ├── button.hpp            # Focusable button widget
    ├── checkbox.hpp          # Toggleable checkbox widget
    ├── divider.hpp           # Horizontal/vertical separator widget
    ├── interactiveSlider.hpp # Focusable, draggable slider widget
    ├── progressBar.hpp       # Styled progress bar widget
    ├── radioGroup.hpp        # Mutually-exclusive radio button group
    ├── singleCharSpinner.hpp # Animated single-character spinner widget
    ├── slider.hpp            # Read-only value slider widget
    ├── spacer.hpp            # Flex spacer widget
    ├── text.hpp              # Multi-line styled text widget
    ├── textInput.hpp         # Single/multi-line text input & TextArea factory
    └── toggle.hpp            # On/off toggle switch widget

src/
└── main.cpp                  # Main executable demonstration
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

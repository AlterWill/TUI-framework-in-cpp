# Architecture & Component Design Plan

This document outlines architectural design decisions, widget concepts, layout algorithms, and future system capabilities for the TUI framework.

---

## 1. Core Framework Architecture

### Lifecycle Pipeline
The framework follows a 4-pass execution loop per frame:
```
[Event Input] ──► [Measure Pass] ──► [Layout Pass] ──► [Render Pass] ──► [Differential Flush]
```
1. **Measure Pass**: Recursively computes preferred/minimum `SizeConstraints` for each widget.
2. **Layout Pass (`setRectForChildren`)**: Parent widgets compute final child `Rect` bounds based on constraints, flex ratios, and alignment.
3. **Render Pass**: Widgets write styled character cells into `RenderContext`.
4. **Differential Flush**: Framebuffer computes dirty cell diffs and flushes minimal ANSI escape sequences to the terminal.

---

## 2. Layout & Constraint System

### Supported Container Types
- **RowContainer & ColumnContainer**: Linear 1D layout splitting space along the main axis.
- **FlexContainer**: Main-axis space distribution based on flex ratios or equal weights.
- **GridContainer**: 2D row/column matrix space allocation.
- **StackContainer**: Multi-layer Z-axis stacking (children inherit parent content bounds).
- **ScrollContainer**: Viewport container managing X/Y scroll offsets for overflow content.
- **Overlay Stack**: Special root overlay layer for dialogs, tooltips, popups, and context menus.

### Constraint Model
- **Fixed Size**: Hardcoded width/height dimensions.
- **Min / Max Constraints**: Sizing bounds enforced via `std::clamp`.
- **Flex Ratios**: Fractional space allocation based on total children flex weights.
- **Fill / Expand**: Takes up remaining space along main axis.
- **Visibility Modes**:
  - `Visible`: Normal layout and rendering.
  - `Hidden`: Not rendered, but occupies space in layout.
  - `Collapsed`: Excluded from layout computation (0 size).

---

## 3. Planned Widget Library

### Display & Utility Widgets
- **Box**: Customizable borders (top, bottom, left, right) and border title alignment (top/bottom, left/right).
- **Paragraph / Text**: UTF-8 text with alignment, text wrapping, and markdown styling.
- **Spacer & Divider**: Empty space fillers and visual horizontal/vertical split line separators.
- **Image & Video**: ASCII/Unicode pixel matrix renderer for image buffers and frame animation.

### Interactive Controls
- **Button**: Focusable clickable element supporting keypress (Enter/Space) and mouse events.
- **CheckBox & Radio**: Single and multi-select list controls.
- **Slider**: Numeric range adjustment control with mouse drag & arrow key navigation.

### Input Controls
- **TextInput**: Single-line text editor with cursor management, input validation, and password masking (`*`).
- **TextArea**: Multi-line scrollable text editor with memoized rendering for performance.

### Data & Navigation
- **ListView**: Selectable list with customizable item rendering and scroll binding.
- **Table**: Data grid with column headers, cell alignment, and row selection.
- **Tabs & Paginator**: Page switching container with dot/numeric indicators.
- **TreeView**: Hierarchical collapsible tree node structure.
- **Calendar**: Grid-based month/date selector with date styling hooks.

### Dynamic & Timed Components
- **Progress Bar & Spinner**: Animated progress indicators (gradient colors, spring physics, half-block resolution).
- **Timer & Stopwatch**: Counter widgets driven by background time loop events.
- **Charts**: Sparkline, bar chart, and histogram data visualization base classes.

---

## 4. Subsystems & System Capabilities

### Focus & Event Management
- **Focus Scope**: Traps Tab focus within active modal dialogs or sub-trees.
- **Event Propagation**: Bubbling event dispatch with option for parent intercept/capture.

### System Integration & Async Tasks
- **Subprocess / Command Runner**: Run external shell commands (`rg`, `git`, `vim`) with terminal output capture.
- **Async Thread Execution**: Background task worker threads dispatching update events back to the main UI looper without blocking render cycles.

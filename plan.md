# Architecture & Component Design Plan

This document describes the actual framework architecture and design decisions, and labels future/planned work explicitly. Consult the source code for the definitive implementation.

---

## 1. Core Framework Architecture (Implemented)

### Lifecycle Pipeline

The framework follows a 4-pass execution loop per frame:

```
[Event Input] ──► [Resize Check] ──► [Layout Pass] ──► [Render Pass] ──► [Differential Flush]
```

1. **Resize Check** (`Surface::resizeBuffer()`): Re-queries terminal dimensions via `ioctl`; reallocates `current` and `previous` buffers if dimensions changed.
2. **Layout Pass** (`WidgetTree::layout()` → `Widget::layout()` → `setRectForChildren()`): Parent widgets recursively assign `Rect` bounds to children.
3. **Render Pass** (`WidgetTree::render()` → `Widget::render(RenderContext&)`): Widgets write styled `Cell` values into the current `Buffer` via `RenderContext`.
4. **Differential Flush** (`Surface::incrementDisplay()`): Compares `current` and `previous` cell grids; emits ANSI cursor-position and SGR escape sequences only for changed cells; then copies `current` into `previous`.

A `measure()` method exists on each widget and is called by containers during `setRectForChildren()`, but there is currently no separate, explicit measure phase driven by `WidgetTree`. Containers call `measure()` on children internally.

---

## 2. Layout & Constraint System (Implemented)

### Supported Container Types

- **`Row`**: Horizontal layout. Divides available width equally among children; last child receives remaining space. Respects child margin and container padding.
- **`Column`**: Vertical layout. Divides available height equally among children; last child receives remaining height.
- **`Grid`**: 2D row/column matrix. Cells sized by dividing usable width/height by column/row count. Last row/column receives remainder.
- **`Stack`**: All children are given the full parent rect and rendered in order (last child on top).
- **`Flex`** *(constructor currently private — not yet usable)*: Main-axis space distribution based on flex ratios (`LayoutProperties::flex`). Children with `flex > 0` receive a proportional share; children with `flex < 0` receive the remaining space after flex children are placed.

### Not Yet Implemented

- **`Scroll`** (`ScrollContainer.hpp`): Class stub with design comments only. No scroll offset or viewport logic is implemented.
- **Overlay Stack**: No overlay layer for dialogs, tooltips, or context menus.
- **Split pane**: Not started.

### Constraint Model

- **`SizeConstraints`**: Holds `minSize` and `maxSize` as `Size` structs. Used in container `measure()` calls to pass down available space bounds.
- **`LayoutProperties`** (on each `Widget`): Holds `SizeConstraints` and a `flex` integer.
- Min/max constraints are enforced via `std::clamp` in container layout code.
- Percentage sizing is not yet implemented.

---

## 3. Planned Widget Library

The following widgets are **not yet implemented**. This section captures design intent only.

### Display & Utility Widgets

- **Spacer & Divider**: Empty space fillers and visual split line separators.
- **Image**: ASCII/Unicode pixel matrix renderer for image buffers.

### Interactive Controls

- **Button**: Focusable clickable element. Responds to Enter/Space keypress and mouse click. Calls an `onClick` callback.
- **CheckBox & Radio**: Single and multi-select list controls.
- **Slider**: Numeric range adjustment with mouse drag and arrow key navigation.

### Input Controls

- **TextInput**: Single-line text editor with cursor management, input validation, and password masking (`*`).
- **TextArea**: Multi-line scrollable text editor.

### Data & Navigation

- **ListView**: Selectable list with customizable item rendering and scroll binding.
- **Table**: Data grid with column headers, cell alignment, and row selection.
- **Tabs**: Page switching container.
- **TreeView**: Hierarchical collapsible tree node structure.

### Dynamic & Timed Components

- **Progress Bar & Spinner**: Animated progress indicators.
- **Charts**: Sparkline, bar chart, histogram.

---

## 4. Subsystems

### Event & Focus System (Implemented)

- **`EventDispatcher`** (`include/eventHandler.hpp`): Manages focus state and event dispatch.
  - Keyboard events are dispatched to the currently focused widget and bubble up via the `parent` chain until `handleEvent()` returns `true`.
  - Mouse events use DFS hit-testing (`widgetTreeDFS`) to find the deepest widget under the cursor, then bubble upward.
  - `nextFocus()` / `previousFocus()` traverse a flat list of focusable widgets collected from the tree.
  - **Tab/Shift-Tab wiring** is the application's responsibility — `EventDispatcher` provides the methods but does not intercept Tab keys automatically.

### Focus Scopes (Planned)

- Restrict Tab focus navigation to a specific subtree (e.g., an open modal dialog). Not yet implemented. Requires `pushFocusScope(Widget*)` / `popFocusScope()` on `EventDispatcher`.

### Clip Rectangle Stack (Planned)

- `RenderContext` currently passes buffer writes through without bounds checking. A clip stack (`pushClipRect` / `popClipRect`) is needed for the scroll container to prevent child widgets from rendering outside their viewport.

### System Integration & Async Tasks (Future)

- **Subprocess / Command Runner**: Run external shell commands with terminal output capture. Not started.
- **Async Thread Execution**: Background worker threads dispatching update events to the main render loop. Not started.

---

## 5. Debugging, Logging & Testing

### File-Based Diagnostic Logging (Implemented)

- **`tui::Logger`** (`include/logger.hpp`): Compile-time toggled singleton. Enable with `-DENABLE_LOGGING` at CMake configuration time.
- Uses `if constexpr (is_logging_enabled)` so all logging code is eliminated by the compiler in release builds without the flag.
- Writes to `tui_debug.log` by default. Calls `flush()` on every write for crash-safe output.
- Does not use environment variables — the toggle is a compile-time preprocessor flag only.

### Headless Testing Architecture (Planned)

- **In-Memory Terminal Backend**: A `HeadlessBackend` class inheriting from `backend` that stores rendered cell output in an in-memory `Buffer` rather than writing to stdout. Allows CI pipelines to run layout and render tests without a real TTY.
- Not yet implemented.

### Automated Testing Strategy (Planned)

- **Layout Math & Geometry Tests**: Assert exact `Rect` coordinates for `Row`, `Column`, `Grid` children under known dimensions.
- **Text & UTF-8 Clipping Tests**: Validate character widths, multi-byte UTF-8 string truncation, and style preservation under tight bounding boxes.
- **Event & Focus Propagation**: Simulate `keyEvent` and `MouseEvent` to verify bubble propagation and `EventDispatcher` focus cycling.
- No test infrastructure exists yet. See `TODO.md` for the concrete task definition.

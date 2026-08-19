# TODO List

This document tracks active development tasks for the TUI framework.

### Measured Size Caching
- **Goal**: Optimize layout passes by caching widget sizes to avoid redundant calculations.
- **Completion Requirement**: Add `cachedSize` and `isLayoutDirty` member variables to the `Widget` class. Update `Widget::measure` to check and return the cached size if the layout is clean, and set the dirty flag when widget properties change.

### Enhanced Box Widget Borders and Titles
- **Goal**: Allow selective border side rendering (top, bottom, left, right) and title display on box borders with alignment options.
- **Completion Requirement**: Update `Box` in `include/box.hpp` with border flag bitmasks (`BorderSide::Top`, `BorderSide::Bottom`, etc.), a `setTitle(const std::string& title, TitleAlign align)` method, and render logic that embeds the title into the upper or lower border frame.

### Terminal Resize Event Handling
- **Goal**: Automatically catch `SIGWINCH` or terminal resize input sequences and trigger layout recalculation across the widget tree.
- **Completion Requirement**: Add `ResizeEvent` struct to `include/event.hpp`, handle `SIGWINCH` / ANSI resize queries in `linux_backend.hpp`, and dispatch `ResizeEvent` through `WidgetTree` to trigger buffer reallocation and subtree relayout.

### Off-Screen Buffer Rendering and Clipping Stack
- **Goal**: Support rendering complex or layered components into off-screen framebuffers with a clip rectangle stack for viewports and scroll containers.
- **Completion Requirement**: Implement `pushClipRect(const Rect& rect)` and `popClipRect()` in `RenderContext` (`include/renderContext.hpp`), and add `OffscreenBuffer` class in `include/frameBuffer.hpp` allowing sub-rendering into isolated cell grids.

### Focus Scopes and Modal Keyboard Traversal
- **Goal**: Restrict Tab/Shift-Tab keyboard focus navigation to specific subtree scopes (such as modal dialogs or popups).
- **Completion Requirement**: Extend `FocusManager` in `include/eventHandler.hpp` to support `pushFocusScope(Widget* scopeRoot)` and `popFocusScope()`, ensuring focus traversal cycles exclusively through focusable children within the active scope.

### Basic Interactive Button Widget
- **Goal**: Provide a reusable interactive button widget that handles focus, click/keypress activation, and visual hover/active states.
- **Completion Requirement**: Create `Button` class in `include/button.hpp` inheriting from `Widget`, implementing `measure()`, `render()`, and `handleEvent()` for `KeyEvent` (Enter/Space) and `MouseEvent` (Click), along with `setOnClick(std::function<void()> callback)`.

### Text Input Widget with Cursor Management
- **Goal**: Enable single-line text input with cursor positioning, character insertion/deletion, and password masking.
- **Completion Requirement**: Create `TextInput` class in `include/input.hpp` with support for normal, password, and hidden modes, `setText()`, `getText()`, cursor movement via arrow keys, and standard editing handling in `handleEvent()`.

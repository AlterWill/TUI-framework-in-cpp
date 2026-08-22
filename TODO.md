# TODO List

This document tracks active implementation tasks for the TUI framework.
Tasks are ordered by priority: core correctness first, then new features.

---

### Fix Flex Container Constructor Visibility
- **Goal**: `Flex` is currently unusable because its constructor is private, which prevents external instantiation.
- **Completion Requirement**: Make the `Flex` constructor in `include/FlexContainer.hpp` `public`. Verify that a `Flex` widget can be constructed and added to a `WidgetTree` in `src/main.cpp` without compiler errors.

### Implement Scroll Container
- **Goal**: The `Scroll` class in `include/ScrollContainer.hpp` is a stub. It needs a working viewport-and-content-area implementation so widgets larger than the visible area can be scrolled.
- **Completion Requirement**: Implement `Scroll` in `include/ScrollContainer.hpp` inheriting from `SingleChildWidget`. Add `scrollX` and `scrollY` offset fields. Override `measure()` to return the viewport size, and `layout()` to position the child at `(-scrollX, -scrollY)` relative to the viewport origin. Override `render()` to blit only the visible region from the child's rendered output using `Buffer::blitTo()`. Wire mouse scroll events in `handleEvent()` to increment/decrement the offsets.

### Complete Text Widget Alignment
- **Goal**: `Text::render()` dispatches on `Alignment`, but only the `left` case is implemented. Center and right rendering are empty.
- **Completion Requirement**: Implement `renderTextCenter()` and `renderTextRight()` in `include/text.hpp`. Centered text should pad each line so it is horizontally centered within `rect.width`. Right-aligned text should right-justify each line within `rect.width`. All three modes must clip output to the widget's bounding rect.

### Measured Size Caching
- **Goal**: Optimize layout passes by caching widget sizes to avoid redundant `measure()` calls when nothing has changed.
- **Completion Requirement**: Add `cachedSize` and `isLayoutDirty` member variables to the `Widget` class in `include/widget.hpp`. Update `measure()` in each concrete widget to return the cached size if `isLayoutDirty` is false. Set `isLayoutDirty = true` when widget properties (text, style, size constraints) change.

### Terminal Resize Event Handling
- **Goal**: The framework re-queries terminal size each frame via `resizeBuffer()`, but there is no event-driven mechanism for `SIGWINCH`. Layout recalculation only happens at the top of the next frame, which can miss intermediate resizes.
- **Completion Requirement**: Add a `ResizeEvent` struct to `include/event.hpp` and extend the `Event` variant to include it. Handle `SIGWINCH` in `linux_backend.hpp` (via `sigaction`) to set a flag that `readEvent()` can check and return as a `ResizeEvent`. In the main loop, dispatch `ResizeEvent` to trigger `resizeBuffer()` and a full relayout.

### Clip Rectangle Stack in RenderContext
- **Goal**: `RenderContext` currently provides direct buffer access with no bounds enforcement. Widgets can write outside their allocated rect. A clip stack would prevent this and is required for the scroll container to restrict child rendering to its viewport.
- **Completion Requirement**: Add `pushClipRect(const Rect& rect)` and `popClipRect()` methods to `RenderContext` in `include/renderContext.hpp`. Store a `std::vector<Rect>` clip stack. Override `setCell()` to silently discard writes outside the current clip rect (intersection of all active rects).

### Off-Screen Buffer Rendering
- **Goal**: Support rendering widget subtrees into an isolated off-screen buffer, required for the scroll container and future overlay layers.
- **Completion Requirement**: Implement an `OffscreenBuffer` type (or reuse `Buffer` directly) that can be passed to a widget's `render()` call independently of the main `Surface`. Use `Buffer::blitTo()` to composite the result into the main frame buffer at the desired destination rect.

### Focus Scopes and Modal Keyboard Traversal
- **Goal**: Restrict Tab/Shift-Tab keyboard focus navigation to a specific subtree, such as when a modal dialog is open.
- **Completion Requirement**: Extend `EventDispatcher` in `include/eventHandler.hpp` with `pushFocusScope(Widget* scopeRoot)` and `popFocusScope()`. When a scope is active, `nextFocus()` and `previousFocus()` must rebuild the focus order using only focusable descendants of the scope root, ignoring widgets outside it.

### Enhanced Box Widget Borders and Titles
- **Goal**: Allow selective border side rendering (top, bottom, left, right independently) and an optional title string embedded in the border.
- **Completion Requirement**: Update `Box` in `include/box.hpp` with a border-side bitmask (`BorderSide::Top`, `BorderSide::Bottom`, `BorderSide::Left`, `BorderSide::Right`), a `setTitle(const std::string& title, TitleAlign align)` method, and render logic that draws the title embedded into the top or bottom border row at the specified horizontal alignment.

### Basic Interactive Button Widget
- **Goal**: Provide a reusable interactive button widget that handles focus, click/keypress activation, and a visual pressed state.
- **Completion Requirement**: Create a `Button` class in `include/button.hpp` inheriting from `Widget`. Implement `measure()`, `render()` (with distinct normal and focused visual states), and `handleEvent()` responding to `keyEvent` (Enter/Space to activate) and `MouseEvent` (left click to activate). Add `setOnClick(std::function<void()> callback)`. Set `focusable = true` in the constructor.

### Text Input Widget with Cursor Management
- **Goal**: Enable single-line text input with visible cursor, character insertion and deletion, and optional password masking.
- **Completion Requirement**: Create a `TextInput` class in `include/input.hpp` inheriting from `Widget`. Implement `measure()`, `render()` (showing text with a cursor character at the insertion point), and `handleEvent()` handling printable character keys (insert at cursor), Backspace (delete before cursor), Delete (delete after cursor), and Left/Right arrow keys (move cursor). Add `setText()`, `getText()`, and a `passwordMode` flag that renders `*` in place of actual characters.

### Headless Terminal Backend for CI/CD Testing
- **Goal**: Implement an in-memory terminal backend that allows full layout, rendering, and event propagation testing without an attached TTY.
- **Completion Requirement**: Create a `HeadlessBackend` class in `include/headless_backend.hpp` inheriting from `backend`. Implement `findTerminalSize()` with configurable fixed dimensions, and stub out input methods to return `std::nullopt`. Pair it with a `Surface` and verify that after a full render pass, cell contents can be read back via `Surface::getBuffer().at(x, y)` for assertion in unit tests.

### Core Framework Unit Testing Suite
- **Goal**: Provide automated unit tests for layout calculation, UTF-8 rendering, and event bubbling to prevent regressions.
- **Completion Requirement**: Add a `tests/test_main.cpp` executable target to `CMakeLists.txt`. Write tests that: (1) verify `Row` and `Column` distribute child rects correctly for known dimensions, (2) verify `Text` clips correctly at the bounding rect boundary, (3) verify `EventDispatcher::nextFocus` cycles through focusable widgets in tree order, and (4) verify that a `keyEvent` bubbles from a focused leaf widget up through its parent chain until handled.

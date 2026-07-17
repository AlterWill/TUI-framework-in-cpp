# Next Steps Roadmap

## How this TODO works
This file contains the immediate, actionable next steps for your TUI framework. It is strictly ordered top-to-bottom. 
1. **Pick a task**: Work down the list sequentially. Do not jump to a later phase (e.g., Viewports) before finishing its dependencies (e.g., Keyboard Input).
2. **Meet the requirement**: Each task has a "Completion Requirement" (the definition of done). Once your code fulfills this requirement, the task is complete.
3. **Update the Roadmap**: Check the corresponding `[ ]` box in `README.md` and move on to the next item!

---

## 1. Finish Phase 2: Rendering Engine & Terminal Backend

### Clipping
- **Goal**: Restrict drawing operations to a widget's boundary to prevent visual overflow.
- **Completion Requirement**: A parent widget (or the frame buffer itself) enforces a rectangular bounding box. Any `setCell()` or `setGlyph()` calls made outside of this bounding box are safely ignored without throwing out-of-bounds errors.

### Backend Features (256-color & True-color)
- **Goal**: Expand `AnsiColor` and terminal capabilities.
- **Completion Requirement**: The `displayBufferPixel()` function can output `\x1b[38;5;...m` (256-color) and `\x1b[38;2;r;g;bm` (True-color RGB) ANSI escape sequences based on expanded color definitions. You have also abstracted raw `ioctl` calls into a Backend class.

---

## 2. Start Phase 3: Input System (Milestone 2)

### Keyboard Input
- **Goal**: Process raw user keystrokes.
- **Completion Requirement**: The main application loop can asynchronously read bytes from `STDIN` (with the terminal set to raw mode) without blocking the render loop, successfully capturing standard keys, arrows, and modifiers.

### Terminal Resize Events
- **Goal**: Detect when the terminal window changes size.
- **Completion Requirement**: The framework listens for the `SIGWINCH` signal and automatically triggers a framebuffer resize and layout re-calculation.

### Event Dispatcher
- **Goal**: Propagate keyboard events to widgets.
- **Completion Requirement**: Raw keystrokes are translated into `KeyEvent` objects and routed down the widget tree to the currently "focused" widget.

---

## 3. Start Phase 4: Layout Engine

### Viewports & Scroll Container
- **Goal**: Establish virtual windows for scrollable areas.
- **Completion Requirement**: You can define a virtual canvas larger than the terminal window. A `ScrollContainer` widget applies this viewport, and you can test it by using your Keyboard Input system (arrow keys) to scroll up and down.

### Advanced Constraints
- **Goal**: Add dynamic sizing capabilities.
- **Completion Requirement**: Widgets can accept size configurations like Fill/Expand (taking up all remaining space) and Min/Max bounds.

### Flex & Stack Containers
- **Goal**: Implement advanced layout components.
- **Completion Requirement**: 
  - **Flex**: Dynamically distributes space among children based on ratios.
  - **Stack**: Allows widgets to be layered on top of each other on the Z-axis (e.g., for floating text or popups).

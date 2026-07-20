# Next Steps Roadmap

## How this TODO works
This file contains the immediate, actionable next steps for your TUI framework. It is strictly ordered top-to-bottom. 
1. **Pick a task**: Work down the list sequentially. Do not jump to a later phase before finishing its dependencies.
2. **Meet the requirement**: Each task has a "Completion Requirement" (the definition of done). Once your code fulfills this requirement, the task is complete.
3. **Update the Roadmap**: Check the corresponding `[ ]` box in `ROADMAP.md` and move on to the next item!

---

## 1. Finish Phase 2: Rendering Engine & Terminal Backend

### Off-screen Rendering
- **Goal**: Allow rendering to a secondary buffer before drawing to the main terminal buffer.
- **Completion Requirement**: Create an off-screen surface/canvas that widgets can draw into, which is then composited onto the main `frameBuffer`.

---

## 2. Phase 3: Input System (Milestone 2)

### Terminal Resize Events (via Signals)
- **Goal**: Detect when the terminal window changes size using OS signals.
- **Completion Requirement**: The framework listens for the `SIGWINCH` signal. When triggered, it safely notifies the main loop to automatically trigger a framebuffer resize and layout re-calculation (removing the continuous polling of `resizeBuffer()`).

### Event System (Objects & Dispatcher)
- **Goal**: Propagate keyboard and system events to widgets.
- **Completion Requirement**: Raw keystrokes (from `readKey()`) and resize events are translated into standard `KeyEvent` or `ResizeEvent` objects and routed down the widget tree using a dedicated event dispatcher.

### Focus System (Focus Manager)
- **Goal**: Track which widget currently receives keyboard input.
- **Completion Requirement**: Implement a central Focus Manager that knows which widget is active. Only the active widget receives the dispatched `KeyEvent` objects.

### Mouse Input
- **Goal**: Process mouse clicks and movement.
- **Completion Requirement**: The backend can enable mouse tracking and parse ANSI mouse escape sequences into `MouseEvent` objects, passing them to the event dispatcher.

---

## 3. Phase 4: Layout Engine

### Viewports & Scroll Container
- **Goal**: Establish virtual windows for scrollable areas.
- **Completion Requirement**: You can define a virtual canvas larger than the terminal window. A `ScrollContainer` widget applies this viewport, and you can test it by using keyboard input (arrow keys) to scroll up and down.

### Advanced Constraints
- **Goal**: Add dynamic sizing capabilities.
- **Completion Requirement**: Widgets can accept size configurations like Fill/Expand (taking up all remaining space) and Min/Max bounds.

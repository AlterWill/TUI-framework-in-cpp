# Next Steps Roadmap

## How this TODO works
This file contains the immediate, actionable next steps for your TUI framework. It is strictly ordered top-to-bottom. 
1. **Pick a task**: Work down the list sequentially. Do not jump to a later phase before finishing its dependencies.
2. **Meet the requirement**: Each task has a "Completion Requirement" (the definition of done). Once your code fulfills this requirement, the task is complete.
3. **Update the Roadmap**: Check the corresponding `[ ]` box in `ROADMAP.md` and move on to the next item!

---

## 1. Phase 3: Focus System (Milestone 2)

### Focus Traversal & Tab Navigation
- **Goal**: Allow the user to cycle focus through focusable widgets using keyboard input.
- **Completion Requirement**: Integrate focus cycling into the `EventDispatcher`. When a `Tab` or `Shift+Tab` key event is received, find the next or previous widget in the tree where `focusable = true` and update the active `focusedWidget` accordingly.

### Focus Scopes
- **Goal**: Restrict focus traversal to a subset of widgets (e.g., locking focus within a modal/dialog).
- **Completion Requirement**: Implement focus scope boundaries such that when a focus scope is active, pressing `Tab` or `Shift+Tab` cycles focus only within that scope and does not escape to the rest of the widget tree.

---

## 2. Phase 4: Layout Engine

### Viewports & Scroll Container
- **Goal**: Establish virtual windows for scrollable areas.
- **Completion Requirement**: You can define a virtual canvas larger than the terminal window. A `ScrollContainer` widget applies this viewport, and you can test it by using keyboard input (arrow keys) to scroll up and down.

### Advanced Constraints
- **Goal**: Add dynamic sizing capabilities.
- **Completion Requirement**: Widgets can accept size configurations like Fill/Expand (taking up all remaining space) and Min/Max bounds.

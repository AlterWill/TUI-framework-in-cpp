# Next Steps Roadmap

## How this TODO works
This file contains the immediate, actionable next steps for your TUI framework. It is strictly ordered top-to-bottom.
1. **Pick a task**: Work down the list sequentially. Do not jump to a later phase before finishing its dependencies.
2. **Meet the requirement**: Each task has a "Completion Requirement" (the definition of done). Once your code fulfills this requirement, the task is complete.
3. **Update the Roadmap**: Check the corresponding `[ ]` box in `ROADMAP.md` and move on to the next item!

---

## 1. Phase 4: Container Widgets (Milestone 3)

### Flex Container
- **Goal**: Create a flexible layout container capable of arranging children along a horizontal or vertical axis while distributing available space.
- **Completion Requirement**: Implement a `Flex` widget supporting direction (row/column), spacing, alignment, and flexible sizing. Demonstrate multiple children automatically resizing and repositioning when the parent size changes.

### Stack Container
- **Goal**: Allow multiple widgets to occupy the same layout region.
- **Completion Requirement**: Implement a `Stack` widget where all children receive the same parent rectangle. Children are rendered in insertion order, allowing later widgets to appear on top of earlier ones.

### Overlay Container
- **Goal**: Support floating UI elements that appear above normal content.
- **Completion Requirement**: Implement an `Overlay` container capable of rendering popups, tooltips, or dialogs above the primary widget tree without affecting the underlying layout.

### Viewports
- **Goal**: Restrict rendering to a visible portion of a larger virtual area.
- **Completion Requirement**: Implement viewport clipping so widgets only render within their assigned viewport rectangle. Rendering outside the viewport must be clipped.

### Scroll Container
- **Goal**: Display content larger than the visible viewport.
- **Completion Requirement**: Implement a `ScrollContainer` that maintains horizontal and/or vertical scroll offsets. Keyboard input should allow scrolling through content while rendering remains clipped to the viewport.

### Split Pane
- **Goal**: Divide available space into independently managed regions.
- **Completion Requirement**: Implement a `SplitPane` supporting both horizontal and vertical orientations. Each pane should automatically receive its allocated space, and the divider position should be configurable.

---

## 2. Future Improvements

### Focus Scopes
- **Goal**: Restrict keyboard focus to a subtree of widgets.
- **Completion Requirement**: When a focus scope is active (such as a modal dialog), `Tab` and `Shift+Tab` cycle only through widgets within that scope until the scope is dismissed.

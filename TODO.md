# Next Steps Roadmap

## How this TODO works
This file contains the immediate, actionable next steps for your TUI framework. It is strictly ordered top-to-bottom.
1. **Pick a task**: Work down the list sequentially. Do not jump to a later phase before finishing its dependencies.
2. **Meet the requirement**: Each task has a "Completion Requirement" (the definition of done). Once your code fulfills this requirement, the task is complete.
3. **Update the Roadmap**: Check the corresponding `[ ]` box in `ROADMAP.md` and move on to the next item!

---

## 1. Phase 4: Layout Engine (Milestone 2)

### Measure Pass
- **Goal**: Introduce a dedicated measure phase before layout so widgets can report their preferred size given maximum layout constraints.
- **Completion Requirement**:
  - Add `measure()` to `Widget`.
  - Introduce a `Constraints` structure (max width, max height).
  - Implement `measure()` for `Text`.
  - Update existing containers to use `measure()` before layout.
  - Existing examples should continue working.

### RenderContext Improvements
- **Goal**: Rewrite the render function so that a `RenderContext` is constructed and passed explicitly through the widget tree instead of a raw framebuffer.
- **Completion Requirement**: `WidgetTree::render()` builds a `RenderContext` and passes it to the root widget. All widgets receive and forward the context correctly. Existing rendering behaviour is unchanged.

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

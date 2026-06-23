# TUI Plan

## Current Direction

The project already has a good base:

* `Widget`
* `Container`
* `Row`
* `Column`
* `Grid`
* `Box`
* `Text`
* `WidgetTree`
* `frameBuffer`

The next goal is to turn the current prototype into a **clean TUI framework core** by fixing the tree model, separating layout from rendering, and then building missing layout / input features on top.

---

# Phase 1 — Fix the Core Architecture

## 1. Fix `Box`

Current problem:

* `Box` is a `Widget`, but it also owns an internal layout container.
* This makes it a special case in the tree and mixes layout + rendering responsibilities.

## Goal

Make `Box` part of the normal widget tree model.

## Recommended options

### Option A — `Box : public Container`

`Box` directly owns children and behaves like a visual container.

Responsibilities:

* draw border in `render()`
* compute inner content rect in `layout()`
* lay out children inside that inner rect

### Option B — `Box` as a single-child wrapper

`Box` owns one `std::unique_ptr<Widget> child`.

Responsibilities:

* draw border in `render()`
* compute inner content rect in `layout()`
* assign that rect to `child`

## Recommendation

Use **Option B** long-term because it keeps `Box` simple and clean.

If the goal is the **smallest immediate refactor**, use **Option A** first.

---

## 2. Add `layout()` to `Widget`

Right now layout logic is spread across containers and `WidgetTree`, and `Box` still does layout work during render.

## Goal

Every widget should support a layout phase.

### Base design

```cpp
class Widget {
public:
    Rect rect;
    Widget* parent = nullptr;

    virtual void layout() {}
    virtual void render(frameBuffer& fb) = 0;
    virtual ~Widget() = default;
};
```

---

## 3. Separate layout and rendering completely

Current problem:

* layout and rendering are mixed together
* `Box::render()` still does layout work
* `WidgetTree::dfs()` is doing too much

## Goal

Use a clean two-pass pipeline:

### Pass 1 — Layout

* root gets terminal rect
* each container computes child rects
* recurse through tree

### Pass 2 — Render

* every widget renders using already-computed rects
* no widget should assign child rects during `render()`

---

## 4. Split `WidgetTree::dfs()` into proper stages

Replace the current combined traversal with:

```cpp
tree.layout();
tree.render();
tree.display();
```

## Responsibilities

### `layout()`

* set root rect to terminal size
* recurse through tree
* compute every widget rect

### `render()`

* clear framebuffer
* render widget tree into framebuffer

### `display()`

* write framebuffer to terminal
* later perform diff rendering using `previousFrame`

---

## 5. Clean up ownership and parent pointers

### Fixes

* keep `parent` only in `Widget`
* remove duplicate `parent` members from derived classes
* ensure every child added to a container gets `child->parent = this`

---

# Phase 2 — Improve the Layout System

## 6. Add padding

Every widget / container should be able to reserve inner space.

### Needed for

* text not touching borders
* nested boxes looking clean
* consistent spacing

### Suggested type

```cpp
struct Insets {
    int top = 0;
    int right = 0;
    int bottom = 0;
    int left = 0;
};
```

---

## 7. Add margin

Margin is outer spacing between siblings or between a child and its parent edge.

Use after padding is working.

---

## 8. Add layout constraints

Right now `Row` / `Column` divide space equally. That works for a prototype, but real layouts need constraints.

## Add per-widget sizing rules

Support things like:

* fixed width / height
* auto size
* fill remaining space
* flex weight
* min / max size

### Example shape

```cpp
enum class SizeType {
    Auto,
    Fixed,
    Fill
};

struct AxisConstraint {
    SizeType type = SizeType::Auto;
    int value = 0;
    int min = 0;
    int max = -1;
    int flex = 1;
};

struct Constraints {
    AxisConstraint width;
    AxisConstraint height;
};
```

---

## 9. Improve `Row` and `Column`

Once constraints exist, update layout logic:

### `Row`

* lay out fixed-width children first
* calculate remaining width
* divide remaining width among fill children using flex

### `Column`

* same idea, but with height

---

## 10. Improve `Grid`

Current grid is equal cell split only.

Later improvements:

* per-column width strategies
* per-row height strategies
* spanning support
* alignment inside cells

---

## 11. Add clipping / overflow handling

Widgets should not draw outside their visible area.

### Needed for

* long text
* nested boxes
* future scrolling
* safe rendering when layouts are wrong

---

# Phase 3 — Improve the Render System

## 12. Make framebuffer writes safer

Current framebuffer writes should be guarded better.

## Add helper methods like:

* `setCell(x, y, Cell)`
* `drawText(...)`
* `drawChar(...)`

These should:

* ignore out-of-bounds writes
* optionally clip against a rect

---

## 13. Actually use `previousFrame`

The project already stores a previous frame. Use it for real diff rendering.

## Future display pipeline

1. render current frame
2. compare current vs previous
3. emit only changed cells
4. copy / swap current into previous

---

## 14. Add render clipping support

Eventually `render()` should accept a clip rect or render context so widgets can’t draw outside parent bounds.

---

## 15. Improve text rendering correctness

Current text measurement based on `std::string::length()` will break for Unicode / wide characters.

Future improvements:

* count display width, not byte length
* support wide Unicode cells
* separate text measurement from drawing

---

# Phase 4 — Build a Real Runtime

## 16. Add an `App` or runtime loop abstraction

Instead of driving everything manually in `main()`, introduce a small runtime object.

### Example

```cpp
class App {
public:
    WidgetTree tree;

    void run();
};
```

### Responsibilities

* terminal setup / cleanup
* resize detection
* input polling
* calling layout / render / display each frame

---

## 17. Add terminal resize handling

When terminal size changes:

* resize framebuffer
* update root rect
* rerun layout
* rerender screen

---

## 18. Add a render invalidation strategy

Eventually the app should know when it actually needs to rerender.

Possible triggers:

* terminal resize
* state change
* input event
* widget tree update

---

# Phase 5 — Add Events and Input

## 19. Add an event system

Start simple.

### Event types

* `KeyPress`
* `Resize`
* `Quit`

### Example

```cpp
enum class EventType {
    KeyPress,
    Resize,
    Quit
};

struct Event {
    EventType type;
    int key = 0;
    int width = 0;
    int height = 0;
};
```

---

## 20. Add `handleEvent()` to widgets

```cpp
virtual bool handleEvent(const Event& e) { return false; }
```

Return `true` if the event was handled.

---

## 21. Add focus management

Interactive widgets need a focus system.

### Add to `Widget`

* `bool focusable = false;`
* `bool focused = false;`

### App should manage

* current focused widget
* tab / shift-tab traversal
* moving focus between widgets

---

# Phase 6 — Add Interactive Widgets

## 22. Button

First real interactive widget.

Needs:

* label text
* focused state
* pressed state
* keyboard activation

---

## 23. Checkbox

Needs:

* checked / unchecked state
* focus handling
* toggle behavior

---

## 24. Radio group

Needs:

* grouped selection
* keyboard movement
* visual selection state

---

## 25. Toggle switch

A smaller interactive state widget.

---

# Phase 7 — Add Input Widgets

## 26. TextInput

Needs:

* cursor position
* editable string buffer
* focus state
* key handling
* visible cursor rendering

---

## 27. PasswordInput

Same as `TextInput`, but renders masked characters.

---

## 28. TextArea

Needs:

* multi-line editing
* cursor movement across lines
* scrolling support later

---

# Phase 8 — Add Better Containers and Layouts

## 29. Scrollable container

Needed for long content and future lists / text areas.

---

## 30. Stack / overlay container

Useful for:

* popups
* modal dialogs
* layered UI

---

## 31. Split panes

Useful for editor-style layouts and dashboards.

---

## 32. Better grid sizing

Add support for:

* fixed columns
* fill columns
* mixed sizing

---

## 33. Flex-style layout

Only after the basic constraint system is working.

---

# Phase 9 — Add Data Widgets

## 34. List widget

For menus, selectable items, and scrolling content.

---

## 35. Table widget

Needs:

* column widths
* row rendering
* optional scrolling

---

## 36. Tree view

Needs:

* indentation
* collapse / expand state
* keyboard navigation

---

## 37. Tabs

Useful for multi-view interfaces.

---

## 38. Progress bars / indicators

Simple but useful widget category.

---

# Phase 10 — Clean Up the API

## 39. Add declarative builder helpers

The current tree construction style is verbose because it relies heavily on `std::make_unique`.

Future goal:

```cpp
auto ui = box(
    column(
        text("Dashboard"),
        row(
            box(text("CPU")),
            box(text("RAM"))
        )
    )
);
```

---

## 40. Use templates for nicer construction helpers

Use templates for builder convenience, not to replace runtime polymorphism.

Keep:

* `Widget`
* virtual `render()`
* runtime widget tree

Use templates for:

* `row(...)`
* `column(...)`
* `box(...)`
* `text(...)`

---

## 41. Add style / theme support

The current `Sytle` object can later grow into:

* theme presets
* reusable widget styles
* focused / active / disabled states

---

## 42. Improve documentation and examples

Once the architecture is stable:

* add example apps
* add screenshots / GIFs
* document widget behavior
* document layout rules clearly

---

# Recommended Immediate Order

If working on the project right now, do these next in this exact order:

1. **Refactor `Box`**

   * make it part of the normal tree model
2. **Add `layout()` to `Widget`**
3. **Split `WidgetTree::dfs()` into `layout()`, `render()`, `display()`**
4. **Remove layout work from `render()`**
5. **Fix parent ownership cleanup**
6. **Add padding**
7. **Add layout constraints**
8. **Add clipping / safer framebuffer writes**
9. **Add terminal resize handling**
10. **Add event system**
11. **Add focus**
12. **Add Button**
13. **Add TextInput**

---

# Summary

## Core architectural goals

* one clean widget tree model
* separate layout from rendering
* no special-case traversal rules for `Box`
* safer framebuffer rendering
* layout containers that can grow beyond equal splitting

## Core product goals

* better layout
* better text handling
* input and events
* interactive widgets
* cleaner declarative C++ construction API

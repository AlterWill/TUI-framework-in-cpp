#pragma once

#include "input/event.hpp"
#include "layout/SizeConstraints.hpp"
#include "rendering/renderContext.hpp"
#include "utilities/insets.hpp"

/*
    ┌─────────────────────────────────────────────────────────────┐
    │ 1. Fixed & Gaps                                             │
    │    - Subtract gaps and margins from total available.        │
    │    - Allocate fixed sizes directly.                         │
    ├─────────────────────────────────────────────────────────────┤
    │ 2. Percentage                                               │
    │    - Allocate: parent_available * (pct / 100.0)             │
    ├─────────────────────────────────────────────────────────────┤
    │ 3. Content (Natural / Intrinsic)                            │
    │    - Measure children with remaining budget.		  │
    │    - Fits? Allocate full preferred size.                    │
    │    - Overflows? Clay shrink pass (shrink towards min).      │
    ├─────────────────────────────────────────────────────────────┤
    │ 4. Flex                                                     │
    │    - Takes whatever remaining space is left (> 0).          │
    │    - If space == 0, flex size = 0 (no render).		  │
    ├─────────────────────────────────────────────────────────────┤
    │ 5. Positioning (Layout pass)                                │
    │    - Apply Justification / Alignment (Start, Center, End)   │
    │    - Assign final Rect (x, y, w, h) to each child LayoutNode│
    └─────────────────────────────────────────────────────────────┘
*/

struct Widget {
 public:
  virtual Size measure(const SizeConstraints& constraints) = 0;
  virtual void layout() = 0;
  virtual void render(RenderContext& rendercontext) = 0;
  virtual bool handleEvent(const Event&) { return false; }

  virtual ~Widget() = default;
};

struct WidgetBase {
 protected:
  bool focusable{false};

 public:
  Widget* parent = nullptr;
  bool focused{false};

  Insets padding;
};

/*

class Widget {
 protected:
  bool focusable{false};

 public:
  Widget* parent = nullptr;
  bool focused{false};

  Insets padding;
  // Focus
  bool isFocused() const { return focused; }
  void setFocused(bool val) { focused = val; }

  // Parent
  Widget* getParent() const { return parent; }
  void setParent(Widget* val) { parent = val; }

  // Padding
  Insets getPadding() const { return padding; }
  void setPadding(Insets p) { padding = p; }

  void onFocus() { focused = true; }
  void onUnFocus() { focused = false; }

  bool isFocusable() const { return focusable; }
  void disableFocusable() { focusable = false; }

  std::size_t childrenSize() { return 0; }

  virtual std::vector<Widget*> getChildren() { return {}; }

  virtual Size intrinsicSize() = 0;
  virtual Size measure(const SizeConstraints& constraints) = 0;
  virtual bool renderable(Size size) const = 0;
  virtual void render(RenderContext& rendercontext) = 0;
  virtual void layout() = 0;
  virtual bool handleEvent(const Event&) { return false; }

  virtual ~Widget() {}
};

struct WidgetBase {
 protected:
  bool focusable{false};

 public:
  Widget* parent = nullptr;
  bool focused{false};

  Insets padding;
};

struct Widget {
 public:
  virtual Size measure(const SizeConstraints& constraints) = 0;
  virtual void render(RenderContext& rendercontext) = 0;
  virtual void layout() = 0;
  virtual bool handleEvent(const Event&) { return false; }

  virtual ~Widget() = default;
};
*/

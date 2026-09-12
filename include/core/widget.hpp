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
  Widget* parent = nullptr;
  Insets padding;

  void setFocusable() { focusable = true; }
  bool isFocusable() const { return focusable; }
  void setFocused(bool f) { focused = f; }
  bool isFocused() const { return focused; }

  virtual Size measure(const SizeConstraints& constraints) = 0;
  virtual void layout(const Rect&) {};
  virtual void render(RenderContext& rendercontext) = 0;
  virtual bool handleEvent(const Event&) { return false; }

  virtual ~Widget() = default;

 protected:
  bool focused{false};
  bool focusable{false};
};

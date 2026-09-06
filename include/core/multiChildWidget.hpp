#pragma once

#include <cstddef>
#include <vector>

#include "core/layoutNode.hpp"
#include "core/widget.hpp"

struct MultiChildWidgetBase {
  WidgetBase widgetBase;
  std::vector<LayoutNode> children;
  std::size_t gap{};
};

struct MultiChildWidget : public Widget {
  MultiChildWidgetBase base;

  MultiChildWidget() = default;

  virtual void setRectForChildren() {}

  void layout() override {
    setRectForChildren();
    for (auto& child : base.children) {
      if (child.widget) {
        child.widget->layout();
      }
    }
  }

  void render(RenderContext& rendercontext) override {
    for (auto& child : base.children) {
      if (child.widget) {
        rendercontext.setRect(child.rect);
        child.widget->render(rendercontext);
      }
    }
  }
};

/*

1. measure(constraints):
Calculate and return how much space this container and its children want (running the priority
sizing pass: Fixed → Percentage → Content → Flex).
2. setRectForChildren() (or setRectForChild()):
Calculate and assign child.rect (position x,y and size w,h) for each child.

*/

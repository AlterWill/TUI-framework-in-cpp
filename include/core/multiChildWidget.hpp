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

  virtual void setRectForChildren(const Rect&) {}

  void layout(const Rect& rect) override {
    setRectForChildren(rect);
    for (auto& child : base.children) {
      if (child.widget) {
        child.widget->layout(child.rect);
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

  bool handleEvent(const Event& event) override {
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      // First try hit-testing children under the mouse cursor
      for (auto& child : base.children) {
        if (child.widget && child.rect.contains(mouse->x, mouse->y)) {
          if (child.widget->handleEvent(event)) {
            return true;
          }
        }
      }
    }

    // Fallback or non-mouse event: forward to all children until handled
    for (auto& child : base.children) {
      if (child.widget && child.widget->handleEvent(event)) {
        return true;
      }
    }
    return false;
  }
};

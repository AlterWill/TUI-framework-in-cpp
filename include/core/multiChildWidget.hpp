#pragma once

#include <cstddef>
#include <vector>

#include "core/layoutNode.hpp"
#include "core/widget.hpp"
#include "utilities/Rect.hpp"

struct ConnectionInfo {
  struct Separator {
    Rect rect;
    bool isHorizontal;
    std::size_t childIndexBefore;
    std::size_t childIndexAfter;
  };

  std::vector<Separator> separators;
  std::vector<ConnectionInfo> childConnections;

  ConnectionInfo() = default;

  void addSeparator(const Rect& rect, bool horizontal, std::size_t before, std::size_t after) {
    separators.push_back({rect, horizontal, before, after});
  }

  void addChildConnections(const ConnectionInfo& childConn) {
    childConnections.push_back(childConn);
  }
};

struct MultiChildWidgetBase {
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

  /// Hit-tests children in reverse z-order (topmost child first).
  bool forwardMouseEvent(const MouseEvent& mouse, const Event& rawEvent) {
    for (auto it = base.children.rbegin(); it != base.children.rend(); ++it) {
      if (it->widget && it->rect.contains(mouse.x, mouse.y)) {
        if (it->widget->handleEvent(rawEvent)) {
          return true;
        }
      }
    }
    return false;
  }

  /// Forwards non-mouse events (e.g. keyEvent) to children until one consumes it.
  bool forwardKeyEvent(const Event& rawEvent) {
    for (auto& child : base.children) {
      if (child.widget && child.widget->handleEvent(rawEvent)) {
        return true;
      }
    }
    return false;
  }

  bool handleEvent(const Event& event) override {
    if (const auto* mouse = std::get_if<MouseEvent>(&event)) {
      if (forwardMouseEvent(*mouse, event)) return true;
    } else {
      if (forwardKeyEvent(event)) return true;
    }
    return false;
  }

  virtual ConnectionInfo getConnections() const {
    return {};
  }
};

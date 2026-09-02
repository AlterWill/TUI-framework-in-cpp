#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "layoutNode.hpp"
#include "renderContext.hpp"
#include "widget.hpp"

class MultiChildWidget : public Widget {
 public:
  std::vector<LayoutNode> children;
  std::size_t gap{};

  MultiChildWidget() = default;

  std::size_t childrenSize() { return children.size(); }

  void addChild(std::unique_ptr<Widget> widget) {
    if (!widget) return;
    widget->parent = this;
    LayoutNode node;
    node.widget = std::move(widget);
    children.push_back(std::move(node));
  }

  void addChild(LayoutNode node) {
    if (node.widget) {
      node.widget->parent = this;
    }
    children.push_back(std::move(node));
  }

  void removeChild(std::size_t index) {
    if (index < children.size()) {
      children.erase(children.begin() + static_cast<int>(index));
    }
  }

  void clearChildren() { children.clear(); }

  void setGap(std::size_t gapValue) { gap = gapValue; }
  std::size_t getGap() const { return gap; }

  std::vector<Widget*> getChildren() override {
    std::vector<Widget*> result;
    result.reserve(children.size());
    for (auto& child : children) {
      if (child.widget) {
        result.push_back(child.widget.get());
      }
    }
    return result;
  }

  void render(RenderContext& renderContext) override {
    for (auto& child : children) {
      if (child.widget) {
        child.widget->render(renderContext);
      }
    }
  }

  void layout() override {
    setRectForChildren();
    for (auto& child : children) {
      if (child.widget) {
        child.widget->layout();
      }
    }
  }

  virtual void setRectForChildren() {}
};

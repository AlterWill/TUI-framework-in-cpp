#pragma once

#include <memory>
#include <vector>

#include "widget.hpp"

enum class LayoutType { Row, Column, Grid };

class MultiChildWidget : public Widget {
 public:
  std::vector<std::unique_ptr<Widget>> children;
  std::size_t gap{};

  std::size_t childrenSize() { return children.size(); }

  void addChild(std::unique_ptr<Widget> widget) {
    if (!widget) {
      return;
    }
    widget->parent = this;
    children.push_back(std::move(widget));
  }

  void removeChild(std::size_t index) {
    if (index < children.size()) {
      children.erase(children.begin() + index);
    }
  }

  void clearChildren() { children.clear(); }

  void setGap(std::size_t gapValue) { gap = gapValue; }

  std::size_t getGap() { return gap; }

  std::vector<Widget*> getChildren() override {
    std::vector<Widget*> result;
    for (auto& child : children) {
      result.push_back(child.get());
    }
    return result;
  }

  virtual SizeConstraints buildConstraints(std::size_t childIndex) = 0;

  std::vector<SizeConstraints> buildChildrenSize() {
    std::vector<SizeConstraints> childrenSizes;
    for (std::size_t i = 0; i < children.size(); ++i) {
      auto constraints = buildConstraints(i);

      SizeConstraints size = children[i]->measure();

      childrenSizes.push_back(size);
    }
    return childrenSizes;
  }

  void render(RenderContext& renderContext) override {
    for (auto& child : children) {
      child->render(renderContext);
    }
  }

  void layout() override {
    setRectForChildren();
    for (auto& child : children) {
      child->layout();
    }
  };

  virtual void setRectForChildren() {}
};

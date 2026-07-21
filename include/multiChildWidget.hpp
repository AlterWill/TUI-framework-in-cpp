#pragma once

#include <memory>
#include <vector>

#include "widget.hpp"

enum class LayoutType { Row, Column, Grid };

class MultiChildWidget : public Widget {
 public:
  std::vector<std::unique_ptr<Widget>> children;

  void render(frameBuffer& fb) override {
    for (auto& child : children) {
      child->render(fb);
    }
  }

  size_t childrenSize() { return children.size(); }

  std::vector<Widget*> getChildren() override {
    std::vector<Widget*> result;
    for (auto& child : children) {
      result.push_back(child.get());
    }
    return result;
  }

  void layout() override {
    setRectForChildren();
    for (auto& child : children) {
      child->layout();
    }
  };

  void addChild(std::unique_ptr<Widget> widget) {
    if (!widget) {
      return;
    }
    widget->parent = this;
    children.push_back(std::move(widget));
  }

  void removeChild(size_t index) {
    if (index < children.size()) {
      children.erase(children.begin() + index);
    }
  }

  void clearChildren() { children.clear(); }

  virtual void setRectForChildren() {}
};

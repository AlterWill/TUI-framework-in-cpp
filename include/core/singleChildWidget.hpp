#pragma once

#include <memory>
#include <vector>

#include "core/layoutNode.hpp"
#include "rendering/renderContext.hpp"
#include "core/widget.hpp"

class SingleChildWidget : public Widget {
 public:
  LayoutNode child;

  SingleChildWidget() = default;

  explicit SingleChildWidget(std::unique_ptr<Widget> c) {
    child.widget = std::move(c);
    if (child.widget) {
      child.widget->parent = this;
    }
  }

  explicit SingleChildWidget(LayoutNode c) : child(std::move(c)) {
    if (child.widget) {
      child.widget->parent = this;
    }
  }

  std::size_t childrenSize() { return child.widget ? 1 : 0; }

  std::vector<Widget*> getChildren() override {
    if (child.widget) return {child.widget.get()};
    return {};
  }

  void setChild(std::unique_ptr<Widget> c) {
    child.widget = std::move(c);
    if (child.widget) {
      child.widget->parent = this;
    }
  }

  void setChild(LayoutNode c) {
    child = std::move(c);
    if (child.widget) {
      child.widget->parent = this;
    }
  }

  void removeChild() { child.widget = nullptr; }

  Size intrinsicSize() override {
    if (child.widget) {
      return child.widget->intrinsicSize();
    }
    return Size{0, 0};
  }

  Size measure(const SizeConstraints& constraints) override {
    if (child.widget) {
      return child.widget->measure(constraints);
    }
    return Size{0, 0};
  }

  virtual void setRectForChild() {}

  void render(RenderContext& renderContext) override {
    if (!child.widget) return;
    renderContext.setRect(child.rect);
    child.widget->render(renderContext);
  }

  void layout() override {
    if (!child.widget) return;
    setRectForChild();
    child.widget->layout();
  }
};

#pragma once

#include "core/layoutNode.hpp"
#include "core/widget.hpp"

struct SingleChildWidgetBase {
  WidgetBase widgetBase;
  LayoutNode child;
};

struct SingleChildWidget : public Widget {
  SingleChildWidgetBase base;

  SingleChildWidget() = default;
  explicit SingleChildWidget(std::unique_ptr<Widget> c) {
    base.child.widget = std::move(c);
  }
  explicit SingleChildWidget(LayoutNode c) : base{.widgetBase = {}, .child = std::move(c)} {}

  virtual void setRectForChild() {}

  void layout() override {
    setRectForChild();
    if (base.child.widget) {
      base.child.widget->layout();
    }
  }

  void render(RenderContext& rendercontext) override {
    if (base.child.widget) {
      rendercontext.setRect(base.child.rect);
      base.child.widget->render(rendercontext);
    }
  }
};

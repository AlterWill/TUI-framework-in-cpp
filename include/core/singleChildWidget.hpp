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

  virtual void setRectForChild(const Rect&) {}

  void layout(const Rect& rect) override {
    setRectForChild(rect);
    if (base.child.widget) {
      base.child.widget->layout(base.child.rect);
    }
  }

  void render(RenderContext& rendercontext) override {
    if (base.child.widget) {
      rendercontext.setRect(base.child.rect);
      base.child.widget->render(rendercontext);
    }
  }

  bool handleEvent(const Event& event) override {
    if (base.child.widget) {
      return base.child.widget->handleEvent(event);
    }
    return false;
  }
};

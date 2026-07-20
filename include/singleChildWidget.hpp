#pragma once

#include "widget.hpp"
#include "frameBuffer.hpp"
#include <memory>

class SingleChildWidget : public Widget {
public:
  std::unique_ptr<Widget> child;

  SingleChildWidget(std::unique_ptr<Widget> c) : child(std::move(c)) {
    if (child) {
      child->parent = this;
    }
  }

  void setChild(std::unique_ptr<Widget> c) {
    child = std::move(c);
    child->parent = this;
  }

  void removeChild() { child = nullptr; }

  void render(frameBuffer &fb) override {
    if (!child)
      return;
    child->render(fb);
  }

  void layout() override {
    if (!child) {
      return;
    }
    setRectForChild();
    child->layout();
  }

  virtual void setRectForChild() {}
};

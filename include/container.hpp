#pragma once

#include <memory>
#include <vector>

#include "widget.hpp"

enum class LayoutType { Row, Column, Grid };

class Container : public Widget {
 public:
  std::vector<std::unique_ptr<Widget>> children;
  Widget* parent = nullptr;

  void render(frameBuffer& fb) override;

  void addChild(std::unique_ptr<Widget> widget) {
    children.push_back(std::move(widget));
  }

  void removeChild(size_t index) {
    if (index < children.size()) {
      children.erase(children.begin() + index);
    }
  }

  void clearChildren() {
    children.clear();
  }

  virtual void setRectForChildren() {
  }
};

#pragma once

#include "multiChildWidget.hpp"

class Stack : public MultiChildWidget {
 public:
  Size measure(const SizeConstraints& constraints) override {
    if (children.empty()) {
      return Size{0, 0};
    }
    std::size_t maxHeight = 0;
    std::size_t maxWidth = 0;
    for (auto& child : children) {
      Size childSize = child->measure(constraints);
      maxHeight = std::max(maxHeight, childSize.getHeight());
      maxWidth = std::max(maxWidth, childSize.getWidth());
    }
    return Size{maxHeight, maxWidth};
  }

  void setRectForChildren() override {
    for (auto& child : children) {
      child->setRect(rect);
    }
  }
};

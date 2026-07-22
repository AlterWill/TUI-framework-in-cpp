#pragma once

#include "multiChildWidget.hpp"

class Stack : public MultiChildWidget {
 public:
  void setRectForChildren() override {
    for (auto& child : children) {
      child->setRect(rect);
    }
  }
};

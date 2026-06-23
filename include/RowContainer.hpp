#pragma once

#include "container.hpp"

class Row : public Container {
 public:
  Row() = default;

  Row(std::vector<std::unique_ptr<Widget>> c) {
    children = std::move(c);
  }

  void setRectForChildren() override {
    int childrenLen = children.size();
    int ChildWidth = rect.width / childrenLen;
    int currentX;
    for (int i = 0; i < childrenLen - 1; i++) {
      currentX = i * ChildWidth;
      children[i]->setRect(rect.x + currentX, rect.y, rect.height, ChildWidth);
    }
    currentX = (childrenLen - 1) * ChildWidth;
    children[childrenLen - 1]->setRect(rect.x + currentX, rect.y, rect.height, rect.width - currentX);
  }
};

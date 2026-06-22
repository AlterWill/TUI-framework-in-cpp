#pragma once

#include <cmath>

#include "container.hpp"

class Row : public Container {
  Row(std::vector<std::unique_ptr<Widget>> c) : children(std::move(c)) {
  }

  void setRectForChildren() override {
    int childrenLen = children.size();
    int Childwidth = static_cast<double>(rect.width) / childrenLen;
    int Childheight = static_cast<double>(rect.height) / childrenLen;
    int currentXY;
    for (int i = 0; i < childrenLen - 1; i++) {
      int currentXY = i * std::floor(Childheight);
      children[i]->setRect(rect.x + currentXY, rect.y, rect.height, currentXY);
    }
    currentXY = (childrenLen - 1) * std::ceil(Childwidth);
    children[childrenLen - 1]->setRect(rect.x + currentXY, rect.y, rect.height, currentXY);
  }
};

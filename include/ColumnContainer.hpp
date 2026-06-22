#pragma once

#include "container.hpp"
#include <cmath>

class Column : public Container {
public:
  void setRectForChildren() override {
    int childrenLen = children.size();
    int Childwidth = static_cast<double>(rect.width) / childrenLen;
    int Childheight = static_cast<double>(rect.height) / childrenLen;
    int currentXY;
    for (int i = 0; i < childrenLen - 1; i++) {
      int currentXY = i * std::floor(Childheight);
      children[i]->setRect(rect.x, rect.y + currentXY, currentXY, rect.width);
    }
    currentXY = (childrenLen - 1) * std::ceil(Childheight);
    children[childrenLen - 1]->setRect(rect.x, rect.y + currentXY, rect.height, Childwidth);
  }
};

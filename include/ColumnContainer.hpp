#pragma once

#include "multiChildWidget.hpp"

class Column : public MultiChildWidget {
public:
  void setRectForChildren() override {
    int childrenLen = children.size();
    if (childrenLen == 0)
      return;
    int Childheight = rect.height / childrenLen;
    int currentY = 0;
    for (int i = 0; i < childrenLen - 1; i++) {
      currentY = i * Childheight;
      children[i]->setRect(rect.x, rect.y + currentY, Childheight, rect.width);
    }
    currentY = (childrenLen - 1) * Childheight;
    children[childrenLen - 1]->setRect(rect.x, rect.y + currentY,
                                       rect.height - currentY, rect.width);
  }
};

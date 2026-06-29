#pragma once

#include "multiChildWidget.hpp"

class Row : public MultiChildWidget {
public:
  Row() = default;

  Row(std::vector<std::unique_ptr<Widget>> c) { children = std::move(c); }

  void setRectForChildren() override {
    if (children.empty()) {
      return;
    }
    size_t childrenLen = children.size();
    int ChildWidth = rect.width / static_cast<int>(childrenLen);
    int currentX;
    for (size_t i = 0; i < childrenLen - 1; i++) {
      currentX = static_cast<int>(i) * ChildWidth;
      children[i]->setRect(rect.x + currentX, rect.y, rect.height, ChildWidth);
    }
    currentX = static_cast<int>(childrenLen - 1) * ChildWidth;
    children[childrenLen - 1]->setRect(rect.x + currentX, rect.y, rect.height,
                                       rect.width - currentX);
  }
};

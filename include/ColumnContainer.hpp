#pragma once

#include "multiChildWidget.hpp"

class Column : public MultiChildWidget {
 public:
  virtual bool handleEvent(const Event& e) override { return false;};

  void setRectForChildren() override {
    if (children.empty()) {
      return;
    }

    int usableWidth = std::max(0, static_cast<int>(rect.width) - padding.left - padding.right);
    int usableHeight = std::max(0, static_cast<int>(rect.height) - padding.top - padding.bottom);
    int startX = rect.x + padding.left;
    int startY = rect.y + padding.top;

    size_t childrenLen = children.size();
    int Childheight = usableHeight / childrenLen;
    int currentY = 0;
    for (size_t i = 0; i < childrenLen - 1; i++) {
      currentY = i * Childheight;
      // clang-format off
      children[i]->setRect(
        startX + children[i]->margin.left,
        startY + currentY + children[i]->margin.top,
        std::max(0, Childheight - children[i]->margin.top - children[i]->margin.bottom),
        std::max(0, usableWidth - children[i]->margin.left - children[i]->margin.right)
      );
      // clang-format on
    }
    currentY = (childrenLen - 1) * Childheight;
    // clang-format off
    children[childrenLen - 1]->setRect(
      startX + children[childrenLen -1 ]->margin.left,
      startY + children[childrenLen -1]->margin.top + currentY,
      std::max(0,usableHeight - currentY - children[childrenLen-1 ]->margin.top- children[childrenLen -1 ]->margin.bottom),
      std::max(0,usableWidth - children[childrenLen-1 ]->margin.left - children[childrenLen -1 ]->margin.right)
    );
    // clang-format on
  }
};

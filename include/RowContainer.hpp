#pragma once

#include "multiChildWidget.hpp"

class Row : public MultiChildWidget {
public:
  Row() = default;

  Row(std::vector<std::unique_ptr<Widget>> c) { children = std::move(c); }

  virtual bool handleEvent(const Event& ) override { return false;};

  void setRectForChildren() override {
    if (children.empty()) {
      return;
    }

    int usableWidth = std::max(0, static_cast<int>(rect.width - padding.left - padding.right));
    int usableHeight = std::max(0, static_cast<int>(rect.height - padding.top - padding.bottom));
    int startX = rect.x + padding.left;
    int startY = rect.y + padding.top;

    std::size_t childrenLen = children.size();
    int ChildWidth = usableWidth / static_cast<int>(childrenLen);
    int currentX;

    for (std::size_t i = 0; i < childrenLen - 1; i++) {
      currentX = static_cast<int>(i) * ChildWidth;
      // clang-format off
      children[i]->setRect(
        startX + currentX + children[i]->margin.left,
        startY + children[i]->margin.top,
        std::max(0,usableHeight - children[i]->margin.top - children[i]->margin.bottom),
        std::max(0,ChildWidth - children[i]->margin.left - children[i]->margin.right)
      );
      // clang-format on
    }

    currentX = static_cast<int>(childrenLen - 1) * ChildWidth;

    // clang-format off
    children[childrenLen - 1]->setRect(
      startX + currentX + children[childrenLen - 1]->margin.left,
      startY + children[childrenLen - 1]->margin.top,
      std::max(
        0,
        usableHeight - children[childrenLen - 1]->margin.top - children[childrenLen - 1]->margin.bottom
      ),
      std::max(
        0,
        usableWidth - currentX - children[childrenLen - 1]->margin.left - children[childrenLen - 1]->margin.right
      )
    ); // clang-format on
  }
};

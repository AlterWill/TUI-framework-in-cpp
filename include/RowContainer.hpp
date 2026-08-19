#pragma once

#include <algorithm>

#include "multiChildWidget.hpp"

// in row, there is one row and there is multiple children in that row
// width gets divided, not height
class Row : public MultiChildWidget {
 public:
  Row() = default;

  Row(std::vector<std::unique_ptr<Widget>> c) { children = std::move(c); }

  Size measure(const SizeConstraints& constraints) override {
    if (children.empty()) {
      return Size{0, 0};
    }

    std::size_t totalGapSpace{(children.size() - 1) * gap};

    // Subtract the gap space from the available constraint
    if (constraints.getMaxWidth() < totalGapSpace) {
      return Size{0, 0};
    }
    std::size_t availableWidth{constraints.getMaxWidth() - totalGapSpace};

    std::size_t accumulatedWidth{totalGapSpace};
    std::size_t maxHeight{};

    SizeConstraints childConstraints = constraints;
    childConstraints.setMaxWidth(availableWidth / children.size());

    Size childSize{};
    for (std::size_t i = 0; i < children.size() - 1; i++) {
      childSize = children[i]->measure(childConstraints);
      accumulatedWidth += childSize.getWidth();
      maxHeight = std::max(maxHeight, childSize.getHeight());
    }

    // remaining width goes to last element
    childConstraints.setMaxWidth(static_cast<float>(availableWidth) / children.size());
    childSize = children[children.size() - 1]->measure(childConstraints);
    accumulatedWidth += childSize.getWidth();
    maxHeight = std::max(maxHeight, childSize.getHeight());

    return Size{maxHeight, accumulatedWidth};
  }

  virtual bool handleEvent(const Event&) override { return false; };

  void setRectForChildren() override {
    if (children.empty()) return;

    std::size_t childrenLen = children.size();
    std::size_t totalGapSpace = (childrenLen-1)*gap;
    std::size_t usableWidth = std::max(0, static_cast<int>(rect.width) - padding.left - padding.right  );
    std::size_t usableHeight = std::max(0, static_cast<int>(rect.height - padding.top - padding.bottom));
    std::size_t startX = rect.x + padding.left;
    std::size_t startY = rect.y + padding.top;

    std::size_t ChildWidth{usableWidth / childrenLen};
    std::size_t currentX{startX};

    for (std::size_t i{}; i < childrenLen - 1; i++) {
      if(i>0) currentX+=gap;
      // clang-format off
      children[i]->setRect(
        currentX + children[i]->margin.left,
        startY + children[i]->margin.top,
        std::max(0,static_cast<int>(usableHeight) - children[i]->padding.getTop() - children[i]->padding.getBottom() - children[i]->margin.getTop() - children[i]->margin.getBottom()),
        std::max(0,static_cast<int>(ChildWidth) -children[i]->padding.getRight() - children[i]->padding.getLeft() - children[i]->margin.getLeft() - children[i]->margin.getRight())
      );
      // clang-format on
      startX += ChildWidth;
    }

    currentX = (childrenLen - 1) * ChildWidth;

    // clang-format off
    children[childrenLen - 1]->setRect(
      startX + currentX + children[childrenLen - 1]->margin.left,
      startY + children[childrenLen - 1]->margin.top,
      std::max(
        0,
        static_cast<int>(usableHeight) - children[childrenLen - 1]->margin.top - children[childrenLen - 1]->margin.bottom
      ),
      std::max(
        0,
        static_cast<int>(usableWidth) - static_cast<int>(currentX) - children[childrenLen - 1]->margin.getLeft() - children[childrenLen - 1]->margin.getRight()
      )
    );  // clang-format on
  }
};

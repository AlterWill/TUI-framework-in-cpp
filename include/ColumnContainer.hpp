#pragma once

#include <algorithm>

#include "multiChildWidget.hpp"

// height gets divided, not width
class Column : public MultiChildWidget {
 public:
  virtual bool handleEvent(const Event&) override { return false; };

  Size measure(const SizeConstraints& constraints) override {
    if (children.empty()) {
      return Size{0, 0};
    }
    std::size_t height{};
    std::size_t maxWidth{};
    SizeConstraints childConstraints = constraints;

    // Distribute max height among children
    childConstraints.setMaxHeight(constraints.getMaxHeight() / children.size());

    for (auto& child : children) {
      Size childSize = child->measure(childConstraints);
      height += childSize.getHeight();
      maxWidth = std::max(maxWidth, childSize.getWidth());
    }

    height = std::clamp(height, constraints.getMinHeight(), constraints.getMaxHeight());
    maxWidth = std::clamp(maxWidth, constraints.getMinWidth(), constraints.getMaxWidth());

    return Size{height, maxWidth};
  }

  void setRectForChildren() override {
    if (children.empty()) {
      return;
    }

    int usableWidth = std::max(0, static_cast<int>(rect.getWidth()) - padding.left - padding.right);
    int usableHeight = std::max(0, static_cast<int>(rect.getHeight()) - padding.top - padding.bottom);
    int startX = rect.getX() + padding.left;
    int startY = rect.getY() + padding.top;

    std::size_t childrenLen = children.size();
    int childHeight = usableHeight / childrenLen;
    int currentY = 0;
    for (std::size_t i = 0; i < childrenLen - 1; i++) {
      currentY = i * childHeight;
      // clang-format off
      children[i]->setRect(
        startX + children[i]->margin.left,
        startY + currentY + children[i]->margin.top,
        std::max(0, childHeight - children[i]->margin.top - children[i]->margin.bottom),
        std::max(0, usableWidth - children[i]->margin.left - children[i]->margin.right)
      );
      // clang-format on
    }
    currentY = (childrenLen - 1) * childHeight;
    // clang-format off
    children[childrenLen - 1]->setRect(
      startX + children[childrenLen - 1]->margin.left,
      startY + children[childrenLen - 1]->margin.top + currentY,
      std::max(0, usableHeight - currentY - children[childrenLen - 1]->margin.top - children[childrenLen - 1]->margin.bottom),
      std::max(0, usableWidth - children[childrenLen - 1]->margin.left - children[childrenLen - 1]->margin.right)
    );
    // clang-format on
  }


};

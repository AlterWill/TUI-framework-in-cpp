#pragma once

#include <cstddef>
#include <vector>

#include "core/multiChildWidget.hpp"
#include "layout/LinearLayoutSolver.hpp"

struct RowBase {
  Rect rect{};
};

struct Row : public MultiChildWidget {
  RowBase rowBase;

  Row() = default;

  // Builder methods
  Row& withGap(std::size_t g) {
    base.gap = g;
    return *this;
  }

  Row& withPadding(Insets p) {
    base.widgetBase.padding = p;
    return *this;
  }

  Row& addChild(LayoutNode node) {
    base.children.push_back(std::move(node));
    return *this;
  }

  Row& addChild(std::unique_ptr<Widget> w, SizeSpec wSpec = {}, SizeSpec hSpec = {}) {
    LayoutNode node;
    node.widget = std::move(w);
    node.width = wSpec;
    node.height = hSpec;
    base.children.push_back(std::move(node));
    return *this;
  }

  bool handleEvent(const Event&) override { return false; }

  Size measure(const SizeConstraints& constraints) override {
    return LinearLayoutSolver::solveMeasure(base.children, Axis::Horizontal, constraints, base.widgetBase.padding,
                                           base.gap);
  }

  void setRectForChildren() override {
    if (base.children.empty()) return;

    const auto& padding = base.widgetBase.padding;
    std::size_t currentX = rowBase.rect.x + padding.left;
    std::size_t startY = rowBase.rect.y + padding.top;
    std::size_t usableHeight =
        rowBase.rect.height > (padding.top + padding.bottom) ? rowBase.rect.height - padding.top - padding.bottom : 0;

    for (auto& child : base.children) {
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;

      // Vertical alignment in row height
      std::size_t childY = startY + child.margin.top;
      std::size_t totalChildH = childH + child.margin.top + child.margin.bottom;
      if (usableHeight > totalChildH) {
        std::size_t extraVSpace = usableHeight - totalChildH;
        if (child.verticalAlignment == VerticalAlignment::Center) {
          childY += extraVSpace / 2;
        } else if (child.verticalAlignment == VerticalAlignment::Bottom) {
          childY += extraVSpace;
        }
      }

      child.rect = Rect{currentX + child.margin.left, childY, childH, childW};
      currentX += childW + child.margin.left + child.margin.right + base.gap;
    }
  }
};

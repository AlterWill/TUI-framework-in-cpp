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
    padding = p;
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

  Size measure(const SizeConstraints& constraints) override {
    return LinearLayoutSolver::solveMeasure(base.children, Axis::Horizontal, constraints, padding,
                                           base.gap);
  }

  void setRectForChildren(const Rect& rect) override {
    if (base.children.empty()) return;

    rowBase.rect = rect;
    std::size_t currentX = rect.x + padding.left;
    std::size_t startY = rect.y + padding.top;
    std::size_t usableHeight =
        rect.height > padding.vertical() ? rect.height - padding.vertical() : 0;

    for (auto& child : base.children) {
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;

      std::size_t childY = alignCoordinate(startY, usableHeight, childH,
                                           child.margin.top, child.margin.bottom,
                                           child.verticalAlignment);

      child.rect = Rect{currentX + child.margin.left, childY, childH, childW};
      currentX += childW + child.margin.horizontal() + base.gap;
    }
  }
};

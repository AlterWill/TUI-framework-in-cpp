#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "core/multiChildWidget.hpp"
#include "layout/LinearLayoutSolver.hpp"

struct ColumnBase {
  Rect rect{};
};

struct Column : public MultiChildWidget {
  ColumnBase colBase;

  Column() = default;

  // Builder methods
  Column& withGap(std::size_t g) {
    base.gap = g;
    return *this;
  }

  Column& withPadding(Insets p) {
    base.widgetBase.padding = p;
    return *this;
  }

  Column& addChild(LayoutNode node) {
    base.children.push_back(std::move(node));
    return *this;
  }

  Column& addChild(std::unique_ptr<Widget> w, SizeSpec hSpec = {}, SizeSpec wSpec = {}) {
    LayoutNode node;
    node.widget = std::move(w);
    node.height = hSpec;
    node.width = wSpec;
    base.children.push_back(std::move(node));
    return *this;
  }

  Size measure(const SizeConstraints& constraints) override {
    return LinearLayoutSolver::solveMeasure(base.children, Axis::Vertical, constraints, base.widgetBase.padding,
                                           base.gap);
  }

  void setRectForChildren(const Rect& rect) override {
    if (base.children.empty()) return;

    colBase.rect = rect;
    const auto& padding = base.widgetBase.padding;
    std::size_t startX = rect.x + padding.left;
    std::size_t currentY = rect.y + padding.top;
    std::size_t usableWidth =
        rect.width > (padding.left + padding.right) ? rect.width - padding.left - padding.right : 0;

    for (auto& child : base.children) {
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;

      std::size_t childX = alignCoordinate(startX, usableWidth, childW,
                                           child.margin.left, child.margin.right,
                                           child.horizontalAlignment);

      child.rect = Rect{childX, currentY + child.margin.top, childH, childW};
      currentY += childH + child.margin.top + child.margin.bottom + base.gap;
    }
  }
};

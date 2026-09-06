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

  bool handleEvent(const Event&) override { return false; }

  Size measure(const SizeConstraints& constraints) override {
    return LinearLayoutSolver::solveMeasure(base.children, Axis::Vertical, constraints, base.widgetBase.padding,
                                           base.gap);
  }

  void setRectForChildren() override {
    if (base.children.empty()) return;

    const auto& padding = base.widgetBase.padding;
    std::size_t startX = colBase.rect.x + padding.left;
    std::size_t currentY = colBase.rect.y + padding.top;
    std::size_t usableWidth =
        colBase.rect.width > (padding.left + padding.right) ? colBase.rect.width - padding.left - padding.right : 0;

    for (auto& child : base.children) {
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;

      // Horizontal alignment in column width
      std::size_t childX = startX + child.margin.left;
      std::size_t totalChildW = childW + child.margin.left + child.margin.right;
      if (usableWidth > totalChildW) {
        std::size_t extraHSpace = usableWidth - totalChildW;
        if (child.horizontalAlignment == HorizontalAlignment::Center) {
          childX += extraHSpace / 2;
        } else if (child.horizontalAlignment == HorizontalAlignment::Right) {
          childX += extraHSpace;
        }
      }

      child.rect = Rect{childX, currentY + child.margin.top, childH, childW};
      currentY += childH + child.margin.top + child.margin.bottom + base.gap;
    }
  }
};

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
    padding = p;
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
    return LinearLayoutSolver::solveMeasure(base.children, Axis::Vertical, constraints, padding,
                                           base.gap);
  }

  void setRectForChildren(const Rect& rect) override {
    if (base.children.empty()) return;

    colBase.rect = rect;
    std::size_t startX = rect.x + padding.left;
    std::size_t currentY = rect.y + padding.top;
    std::size_t usableWidth =
        rect.width > padding.horizontal() ? rect.width - padding.horizontal() : 0;
    std::size_t maxY = rect.y + rect.height - padding.bottom;

    for (auto& child : base.children) {
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;

      std::size_t childX = alignCoordinate(startX, usableWidth, childW,
                                           child.margin.left, child.margin.right,
                                           child.horizontalAlignment);

      std::size_t startChildY = currentY + child.margin.top;
      std::size_t clampedH = 0;
      if (startChildY < maxY) {
        clampedH = std::min(childH, maxY - startChildY);
      }

      child.rect = Rect{childX, startChildY, clampedH, std::min(childW, usableWidth)};
      currentY += childH + child.margin.vertical() + base.gap;
    }
  }

  ConnectionInfo getConnections() const override {
    ConnectionInfo info;
    if (base.children.size() < 2 || base.gap == 0) return info;

    const Rect& rect = colBase.rect;
    std::size_t currentY = rect.y + padding.top;

    for (std::size_t i = 0; i < base.children.size(); ++i) {
      const auto& child = base.children[i];
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;
      std::size_t childX = child.rect.x;
      currentY += childH + child.margin.vertical();

      if (i + 1 < base.children.size()) {
        Rect sepRect{childX, currentY, base.gap, childW};
        info.addSeparator(sepRect, true, i, i + 1);
        currentY += base.gap;
      }
    }

    for (const auto& child : base.children) {
      if (child.widget) {
        auto* mcw = dynamic_cast<const MultiChildWidget*>(child.widget.get());
        if (mcw) {
          info.addChildConnections(mcw->getConnections());
        }
      }
    }

    return info;
  }
};

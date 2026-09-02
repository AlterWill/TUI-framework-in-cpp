#pragma once

#include <algorithm>
#include <stdexcept>
#include "core/multiChildWidget.hpp"

class Grid : public MultiChildWidget {
 public:
  std::size_t rows = 1;
  std::size_t columns = 1;

  Grid(std::size_t row, std::size_t col) : rows(row), columns(col) {}

  virtual bool handleEvent(const Event&) override { return false; };

  void setRowCol(std::size_t row, std::size_t col) {
    rows = row;
    columns = col;
  }

  Size measure(const SizeConstraints& constraints) override {
    if (children.empty() || rows == 0 || columns == 0) {
      return Size{0, 0};
    }

    std::size_t extraWidth = padding.left + padding.right;
    std::size_t extraHeight = padding.top + padding.bottom;

    std::size_t usableMaxWidth = constraints.getMaxWidth() > extraWidth ? constraints.getMaxWidth() - extraWidth : 0;
    std::size_t usableMaxHeight = constraints.getMaxHeight() > extraHeight ? constraints.getMaxHeight() - extraHeight : 0;

    std::size_t baseWidth = usableMaxWidth / columns;
    std::size_t lastColumnWidth = usableMaxWidth - (baseWidth * (columns - 1));

    std::size_t baseHeight = usableMaxHeight / rows;
    std::size_t lastRowHeight = usableMaxHeight - (baseHeight * (rows - 1));

    std::size_t totalWidth = 0;
    std::size_t totalHeight = 0;

    // Track the maximum size needed in each row / column
    // Sum Column Widths
    for (std::size_t c = 0; c < columns; c++) {
      std::size_t maxColWidth = 0;
      std::size_t currentMaxWidth = (c == columns - 1) ? lastColumnWidth : baseWidth;

      for (std::size_t r = 0; r < rows; r++) {
        std::size_t idx = r * columns + c;
        if (idx >= children.size()) break;

        auto& child = children[idx];
        std::size_t childExtraW = child->margin.left + child->margin.right;
        std::size_t childExtraH = child->margin.top + child->margin.bottom;

        std::size_t currentMaxHeight = (r == rows - 1) ? lastRowHeight : baseHeight;

        SizeConstraints cellConstraints;
        cellConstraints.setMaxWidth(currentMaxWidth > childExtraW ? currentMaxWidth - childExtraW : 0);
        cellConstraints.setMaxHeight(currentMaxHeight > childExtraH ? currentMaxHeight - childExtraH : 0);

        Size childSize = child->measure(cellConstraints);
        maxColWidth = std::max(maxColWidth, childSize.getWidth() + childExtraW);
      }
      totalWidth += maxColWidth;
    }

    // Sum Row Heights
    for (std::size_t r = 0; r < rows; r++) {
      std::size_t maxRowHeight = 0;
      std::size_t currentMaxHeight = (r == rows - 1) ? lastRowHeight : baseHeight;

      for (std::size_t c = 0; c < columns; c++) {
        std::size_t idx = r * columns + c;
        if (idx >= children.size()) break;

        auto& child = children[idx];
        std::size_t childExtraW = child->margin.left + child->margin.right;
        std::size_t childExtraH = child->margin.top + child->margin.bottom;

        std::size_t currentMaxWidth = (c == columns - 1) ? lastColumnWidth : baseWidth;

        SizeConstraints cellConstraints;
        cellConstraints.setMaxWidth(currentMaxWidth > childExtraW ? currentMaxWidth - childExtraW : 0);
        cellConstraints.setMaxHeight(currentMaxHeight > childExtraH ? currentMaxHeight - childExtraH : 0);

        Size childSize = child->measure(cellConstraints);
        maxRowHeight = std::max(maxRowHeight, childSize.getHeight() + childExtraH);
      }
      totalHeight += maxRowHeight;
    }

    totalWidth += extraWidth;
    totalHeight += extraHeight;

    // Clamp to constraints
    totalWidth = std::clamp(totalWidth, constraints.getMinWidth(), constraints.getMaxWidth());
    totalHeight = std::clamp(totalHeight, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{totalHeight, totalWidth};
  }

  void setRectForChildren() override {
    if (rows <= 0 || columns <= 0) {
      throw std::runtime_error("Invalid amount of rows and columns");
    }
    std::size_t childrenLen = children.size();
    if (childrenLen == 0) {
      return;
    }

    int usableWidth = std::max(0, static_cast<int>(rect.width) - static_cast<int>(padding.left + padding.right));
    int usableHeight = std::max(0, static_cast<int>(rect.height) - static_cast<int>(padding.top + padding.bottom));
    std::size_t startX = rect.x + padding.left;
    std::size_t startY = rect.y + padding.top;

    std::size_t childWidth = usableWidth / columns;
    std::size_t childHeight = usableHeight / rows;

    for (std::size_t i = 0; i < childrenLen; i++) {
      std::size_t r = i / columns;
      std::size_t c = i % columns;

      if (r >= rows) {
        break;
      }

      auto& child = children[i];
      std::size_t cellWidth = (c == columns - 1) ? (usableWidth - c * childWidth) : childWidth;
      std::size_t cellHeight = (r == rows - 1) ? (usableHeight - r * childHeight) : childHeight;

      std::size_t x = startX + c * childWidth + child->margin.left;
      std::size_t y = startY + r * childHeight + child->margin.top;

      std::size_t w = std::max(0, static_cast<int>(cellWidth) - static_cast<int>(child->margin.left + child->margin.right));
      std::size_t h = std::max(0, static_cast<int>(cellHeight) - static_cast<int>(child->margin.top + child->margin.bottom));

      child->setRect(x, y, h, w);
    }
  }
};

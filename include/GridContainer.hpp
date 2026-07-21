#pragma once

#include "multiChildWidget.hpp"

class Grid : public MultiChildWidget {
 public:
  int rows = 1;
  int columns = 1;

  Grid(int row, int col) : rows(row), columns(col) {}

  virtual bool handleEvent(const Event& e) override { return false;};

  void setRowCol(int row, int col) {
    rows = row;
    columns = col;
  }

  void setRectForChildren() override {
    if (rows <= 0 || columns <= 0) {
      throw std::runtime_error("Invalid amount of rows and columns");
    }
    int childrenLen = children.size();
    if (childrenLen == 0) {
      return;
    }

    int childWidth = std::max(0, static_cast<int>(rect.width) - padding.right - padding.left) / columns;
    int childHeight = std::max(0, static_cast<int>(rect.height) - padding.top - padding.bottom) / rows;
    int startX = rect.x + padding.left;
    int startY = rect.y + padding.top;

    for (int i = 0; i < childrenLen; i++) {
      int r = i / columns;
      int c = i % columns;

      if (r >= rows) {
        break;
      }

      int x = startX + c * childWidth;
      int y = startY + r * childHeight;

      int w = (c == columns - 1) ? (rect.width - c * childWidth) : childWidth - padding.right;
      int h = (r == rows - 1) ? (rect.height - r * childHeight) : childHeight - padding.left;

      children[i]->setRect(x, y, h, w);
    }
  };
};

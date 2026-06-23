#pragma once

#include "container.hpp"

class Grid : public Container {
public:
  int rows = 1;
  int columns = 1;

  Grid(int row, int col) : rows(row), columns(col) {}

  void setRowCol(int row, int col) {
    rows = row;
    columns = col;
  }

  void setRectForChildren() override {
    int childrenLen = children.size();
    if (rows <= 0 || columns <= 0) {
      throw std::runtime_error("Invalid amount of rows and columns");
    }
    int childWidth = rect.width / columns;
    int childHeight = rect.height / rows;

    for (int i = 0; i < childrenLen; i++) {
      int r = i / columns;
      int c = i % columns;

      int x = rect.x + c * childWidth;
      int y = rect.y + r * childHeight;
      int w = (c == columns - 1) ? (rect.width - c * childWidth) : childWidth;
      int h = (r == rows - 1) ? (rect.height - r * childHeight) : childHeight;

      children[i]->setRect(x, y, h, w);
    }
  };
};

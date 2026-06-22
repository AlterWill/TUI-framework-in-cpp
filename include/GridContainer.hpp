#pragma once

#include <cmath>

#include "container.hpp"

class Grid : public Container {
 public:
  int rows = 1;
  int columns = 1;

  void setRowCol(int row, int col) {
    rows = row;
    columns = col;
  }

  void setRectForChildren() override {
    int childrenLen = children.size();
    if (rows <= 0 || columns <= 0) {
      throw std::runtime_error("Invalid amount of rows and columns");
    }
    if (rows * columns < childrenLen) {
      throw std::runtime_error("There is more children than allowed places to allocate");
    }
    float Childwidth = static_cast<double>(rect.width) / columns;
    float Childheight = static_cast<double>(rect.height) / rows;
    int currentY, currentX;
    for (int i = 0; i < rows; i++) {
      currentY = i * std::floor(Childheight);
      for (int j = 0; j < columns; j++) {
        currentX = j * std::floor(Childwidth);
        children[i * columns + j]->setRect(rect.x + currentX, rect.y + currentY, Childheight, Childwidth);
      }
      currentX = (columns - 1) * std::ceil(Childwidth);
      children[i * columns + (columns - 1)]->setRect(rect.x + currentX, rect.y + currentY, Childheight, Childwidth);
    }
    currentY = (rows - 1) * std::ceil(Childheight);
    currentX = (columns - 1) * std::ceil(Childwidth);
    children[rows * columns - 1]->setRect(rect.x + currentX, rect.y + currentY, Childheight, Childwidth);
  };
};

#pragma once

#include "frameBuffer.hpp"
struct Rect {
  int x = 0;
  int y = 0;
  int height = 0;
  int width = 0;
};

class Widget {
 public:
  Rect rect;
  Widget* parent = nullptr;

  void setRect(int x, int y, int height, int width) {
    rect.x = x;
    rect.y = y;
    rect.height = height;
    rect.width = width;
  }

  virtual void render(frameBuffer& fb) = 0;

  virtual ~Widget() {
  }
};

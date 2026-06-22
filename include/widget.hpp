#pragma once

#include "frameBuffer.hpp"
struct Rect {
  int x;
  int y;
  int height;
  int width;
};

class Widget {
public:
  Rect rect;
  Widget *parent = nullptr;

  void setRect(int x, int y, int height, int width) {
    rect.x = x;
    rect.y = y;
    rect.height = height;
    rect.width = width;
  }

  virtual void render(frameBuffer &fb) = 0;

  virtual ~Widget() {}
};

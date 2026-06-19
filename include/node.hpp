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

  virtual void render(frameBuffer &fb) = 0;

  virtual ~Widget() {}
};

#pragma once

#include "rect.hpp"
#include "insets.hpp"
#include "frameBuffer.hpp"


class Widget {
public:
  Rect rect;
  Rect clip;
  Widget *parent = nullptr;

  Insets padding;
  Insets margin;

  void setClip(){
    clip.height = std::max(rect.height - padding.top -padding.bottom,static_cast<std::size_t>(0));
    clip.width = std::max(rect.width - padding.right -padding.left,static_cast<size_t>(0));
    clip.x = rect.x + padding.left;
    clip.y = rect.y + padding.top;
  }

  void setPadding(Insets p) { padding = p;setClip(); }

  void setMargin(Insets m) { margin = m; }

  void setRect(int x, int y, int height, int width) {
    rect.x = x;
    rect.y = y;
    rect.height = height;
    rect.width = width;
    setClip();
  }

  virtual void render(frameBuffer &fb) = 0;
  virtual void layout() = 0;

  virtual ~Widget() {}
};

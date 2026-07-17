#pragma once

#include "widget.hpp"
#include <memory>

class WidgetTree {
public:
  std::unique_ptr<Widget> root;
  Rect rect;
  frameBuffer fb;

  WidgetTree(std::unique_ptr<Widget> rootContainer)
      : root(std::move(rootContainer)) {
    rect.x = 0;
    rect.y = 0;
    rect.width = fb.col;
    rect.height = fb.row;
  }

  void display() {
    //fb.display();
    fb.incrementDisplay();
  }

  void setRoot(std::unique_ptr<Widget> r) { root = std::move(r); }

  void render() {
    if (!root) {
      return;
    }
    root->render(fb);
  }

  void layout(const Rect &r) {
    if (!root) {
      return;
    }
    root->setRect(r.x, r.y, r.height, r.width);
    root->layout();
  }
};

#pragma once

#include "widget.hpp"
#include "backend.hpp"
#include <memory>

class WidgetTree {
public:
  std::unique_ptr<Widget> root;
  Rect rect;
  backend& terminal;
  frameBuffer fb;

  WidgetTree(std::unique_ptr<Widget> rootContainer,backend& t)
      : root(std::move(rootContainer)), terminal(t), fb(terminal) {
    rect.x = 0;
    rect.y = 0;
    rect.width = terminal.col;
    rect.height = terminal.row;
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

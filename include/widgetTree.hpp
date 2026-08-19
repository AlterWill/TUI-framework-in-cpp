#pragma once

#include <memory>

#include "backend.hpp"
#include "surface.hpp"
#include "widget.hpp"

class WidgetTree {
 public:
  std::unique_ptr<Widget> root;
  Rect rect;
  backend& terminal;
  Surface fb;

  WidgetTree(std::unique_ptr<Widget> rootContainer, backend& t)
      : root(std::move(rootContainer)), terminal(t), fb(terminal) {
    rect.x = 0;
    rect.y = 0;
    rect.width = terminal.col;
    rect.height = terminal.row;
  }

  void incrementDisplay() { fb.incrementDisplay(); }

  void display() { fb.display(); }

  void setRoot(std::unique_ptr<Widget> r) { root = std::move(r); }

  void render() {
    if (!root) {
      return;
    }
    RenderContext context{fb.getBuffer(), rect, Point{0, 0}};
    root->render(context);
  }

  void layout(const Rect& r) {
    if (!root) {
      return;
    }
    root->setRect(r.x, r.y, r.height, r.width);
    // Size prefrededSize = root->measure(SizeConstraints{Size{0,0},Size{rect.getHeight(),rect.getWidth()}});
    root->layout();
  }
};

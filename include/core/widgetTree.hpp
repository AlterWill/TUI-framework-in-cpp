#pragma once

#include <memory>

#include "terminal/backend.hpp"
#include "rendering/surface.hpp"
#include "core/widget.hpp"

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
    RenderContext context{fb.getBuffer()};
    root->render(context);
  }

  void layout(const Rect& r) {
    if (!root) return;
    if(r.height < root->margin.getTop() + root->margin.getBottom() || r.width < root->margin.getLeft() + root->margin.getRight()) return;
    std::size_t rootHeight =  r.height - root->margin.getTop() - root->margin.getBottom();
    std::size_t rootWidth = r.width - root->margin.getLeft() - root->margin.getRight();
    // clang-format off
    Size prefrededSize = root->measure(
      SizeConstraints{
        Size{0,0},
        Size{rootHeight,rootWidth}
      }
    );
    root->setRect(
      r.x + root->margin.getLeft(),
      r.y + root->margin.getTop(),
      prefrededSize.getHeight(),
      prefrededSize.getWidth()
    );
    // clang-format on
    root->layout();
  }
};

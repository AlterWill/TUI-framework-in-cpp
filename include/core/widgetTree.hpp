#pragma once

#include <memory>

#include "core/layoutNode.hpp"
#include "core/widget.hpp"
#include "rendering/surface.hpp"
#include "terminal/backend.hpp"

class WidgetTree {
 public:
  LayoutNode root;
  backend& terminal;
  Surface fb;

  WidgetTree(std::unique_ptr<Widget> rootContainer, backend& t, Insets margin = Insets{})
      : terminal(t), fb(terminal) {
    root.widget = std::move(rootContainer);
    root.margin = margin;
    root.rect.x = 0;
    root.rect.y = 0;
    root.rect.width = terminal.col;
    root.rect.height = terminal.row;
  }

  WidgetTree(LayoutNode rootNode, backend& t)
      : root(std::move(rootNode)), terminal(t), fb(terminal) {
    if (root.rect.width == 0 && root.rect.height == 0) {
      root.rect.x = 0;
      root.rect.y = 0;
      root.rect.width = terminal.col;
      root.rect.height = terminal.row;
    }
  }

  void incrementDisplay() { fb.incrementDisplay(); }

  void display() { fb.display(); }

  void setRoot(std::unique_ptr<Widget> r) { root.widget = std::move(r); }
  void setRoot(LayoutNode r) { root = std::move(r); }

  void render() {
    if (!root.widget) {
      return;
    }
    RenderContext context{fb.getBuffer()};
    context.setRect(root.rect);
    root.widget->render(context);
  }

  void layout(const Rect& r) {
    if (!root.widget) return;
    if (r.height < root.margin.vertical() ||
        r.width < root.margin.horizontal())
      return;
    std::size_t rootHeight = r.height - root.margin.vertical();
    std::size_t rootWidth = r.width - root.margin.horizontal();
    
    Size preferredSize = root.widget->measure(
      SizeConstraints{
        Size{0, 0},
        Size{rootHeight, rootWidth}
      }
    );
    root.rect = Rect{
      r.x + root.margin.left,
      r.y + root.margin.top,
      preferredSize.height,
      preferredSize.width
    };
    root.widget->layout(root.rect);
  }
};

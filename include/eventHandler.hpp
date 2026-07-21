#include "widget.hpp"
#include "widgetTree.hpp"

class EventDispatcher {
 public:
  Widget* focusedWidget = nullptr;

  void dispatchEvent(const Event& event, WidgetTree& tree) {
    if (std::holds_alternative<keyEvent>(event) && focusedWidget) {
      dispatchBubble(focusedWidget, event);
      return;
    }

    if (std::holds_alternative<MouseEvent>(event)) {
      const MouseEvent& mouse = std::get<MouseEvent>(event);
      if (Widget* hit = widgetTreeDFS(*tree.root, mouse.x, mouse.y)) {
        focusedWidget = hit;
        dispatchBubble(hit, event);
      }
    }
  }

 private:
  void dispatchBubble(Widget* node, const Event& event) {
    while (node) {
      if (node->handleEvent(event))
        return;
      else
        node = node->parent;
    }
  }

  Widget* widgetTreeDFS(Widget& node, size_t x, size_t y) {
    if (!node.containsPoint(x, y)) {
      return nullptr;
    }
    for (Widget* child : node.getChildren()) {
      if (Widget* hit = widgetTreeDFS(*child, x, y)) {
        return hit;
      }
    }
    return &node;
  }
};

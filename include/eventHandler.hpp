#include <algorithm>

#include "widget.hpp"
#include "widgetTree.hpp"

class EventDispatcher {
 protected:
  std::vector<Widget*> focusOrder{};
  std::size_t focusIndex{};
  Widget* focusedWidget = nullptr;

 public:
  void setFocus(Widget* widget) {
    if (focusedWidget == widget) return;

    if (focusedWidget) focusedWidget->focused = false;

    focusedWidget = widget;

    if (focusedWidget) focusedWidget->focused = true;
    auto it = std::find(focusOrder.begin(), focusOrder.end(), widget);
    if (it != focusOrder.end()) focusIndex = std::distance(focusOrder.begin(), it);
  }

  void dispatchEvent(const Event& event, WidgetTree& tree) {
    if (std::holds_alternative<keyEvent>(event) && focusedWidget) {
      dispatchBubble(focusedWidget, event);
      return;
    }

    if (std::holds_alternative<MouseEvent>(event)) {
      const MouseEvent& mouse = std::get<MouseEvent>(event);
      if (Widget* hit = widgetTreeDFS(*tree.root, mouse.x, mouse.y)) {
        setFocus(hit);
        dispatchBubble(hit, event);
      }
    }
  }

  void nextFocus(Widget* root) {
    if (focusOrder.empty()) {
      rebuildFocusOrder(root);
      if (focusOrder.empty()) return;
      focusIndex = 0;
      setFocus(focusOrder[focusIndex]);
      return;
    }

    focusIndex = (focusIndex + 1) % focusOrder.size();
    setFocus(focusOrder[focusIndex]);
  }

  void previousFocus(Widget* root) {
    if (focusOrder.empty()) {
      rebuildFocusOrder(root);
      if (focusOrder.empty()) return;
      focusIndex = 0;
      setFocus(focusOrder[focusIndex]);
      return;
    }
    focusIndex = (focusIndex + focusOrder.size() - 1) % focusOrder.size();
    setFocus(focusOrder[focusIndex]);
  }

 private:
  void rebuildFocusOrder(Widget* root) {
    focusOrder.clear();
    collectFocusable(root);
  }

  void collectFocusable(Widget* node) {
    if (!node) return;
    if (node->isFocusable()) {
      focusOrder.push_back(node);
    }
    for (auto child : node->getChildren()) {
      collectFocusable(child);
    }
  }

  void dispatchBubble(Widget* node, const Event& event) {
    while (node) {
      if (node->handleEvent(event))
        return;
      else
        node = node->parent;
    }
  }

  Widget* widgetTreeDFS(Widget& node, std::size_t x, std::size_t y) {
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

#pragma once

#include <algorithm>
#include <vector>

#include "core/layoutNode.hpp"
#include "core/multiChildWidget.hpp"
#include "core/singleChildWidget.hpp"
#include "core/widget.hpp"
#include "core/widgetTree.hpp"
#include "input/event.hpp"

class EventDispatcher {
 protected:
  std::vector<Widget*> focusOrder{};
  std::size_t focusIndex{0};
  bool hasFocus{false};

 public:
  Widget* getFocusedWidget() const {
    if (hasFocus && focusIndex < focusOrder.size()) {
      return focusOrder[focusIndex];
    }
    return nullptr;
  }

  void setFocus(std::size_t index) {
    if (focusOrder.empty()) return;

    if (hasFocus && focusIndex < focusOrder.size()) {
      focusOrder[focusIndex]->setFocused(false);
    }

    focusIndex = index % focusOrder.size();
    focusOrder[focusIndex]->setFocused(true);
    hasFocus = true;
  }

  void setFocus(Widget* widget) {
    if (!widget) {
      clearFocus();
      return;
    }
    auto it = std::find(focusOrder.begin(), focusOrder.end(), widget);
    if (it != focusOrder.end()) {
      setFocus(static_cast<std::size_t>(std::distance(focusOrder.begin(), it)));
    }
  }

  void clearFocus() {
    if (hasFocus && focusIndex < focusOrder.size()) {
      focusOrder[focusIndex]->setFocused(false);
    }
    hasFocus = false;
  }

  void nextFocus() {
    if (focusOrder.empty()) return;
    if (!hasFocus) {
      setFocus(0);
    } else {
      setFocus((focusIndex + 1) % focusOrder.size());
    }
  }

  void previousFocus() {
    if (focusOrder.empty()) return;
    if (!hasFocus) {
      setFocus(focusOrder.size() - 1);
    } else {
      setFocus((focusIndex + focusOrder.size() - 1) % focusOrder.size());
    }
  }

  void rebuildFocusOrder(LayoutNode& root) {
    Widget* prev = getFocusedWidget();
    focusOrder.clear();
    collectFocusable(root);

    if (prev) {
      auto it = std::find(focusOrder.begin(), focusOrder.end(), prev);
      if (it != focusOrder.end()) {
        focusIndex = static_cast<std::size_t>(std::distance(focusOrder.begin(), it));
        hasFocus = true;
      } else {
        clearFocus();
      }
    }
  }

  bool dispatchEvent(const Event& event, WidgetTree& tree) {
    // 1. Keyboard event handling
    if (auto key = std::get_if<keyEvent>(&event)) {
      // Tab navigation
      if (key->key == '\t' || key->key == 9) {
        if (key->mods.getShift()) {
          previousFocus();
        } else {
          nextFocus();
        }
        return true;
      }

      // Bubble from focused widget up parent chain
      if (Widget* target = getFocusedWidget()) {
        if (dispatchBubble(target, event)) {
          return true;
        }
      }

      // Fallback to tree root
      if (tree.root.widget && tree.root.widget->handleEvent(event)) {
        return true;
      }
      return false;
    }

    // 2. Mouse click hit testing
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      if (mouse->action == MouseAction::Press && mouse->button == MouseButton::Left) {
        Widget* hit = findHit(tree.root, mouse->x, mouse->y);
        if (hit) {
          if (hit->isFocusable()) {
            setFocus(hit);
          }
          return dispatchBubble(hit, event);
        }
      }

      // Fallback for other mouse actions (scroll, move, drag) to tree root
      if (tree.root.widget && tree.root.widget->handleEvent(event)) {
        return true;
      }
      return false;
    }

    return false;
  }

 private:
  void collectFocusable(LayoutNode& node) {
    if (!node.widget) return;

    if (auto* single = dynamic_cast<SingleChildWidget*>(node.widget.get())) {
      if (single->base.child.widget) {
        single->base.child.widget->parent = single;
      }
      collectFocusable(single->base.child);
    } else if (auto* multi = dynamic_cast<MultiChildWidget*>(node.widget.get())) {
      for (auto& child : multi->base.children) {
        if (child.widget) {
          child.widget->parent = multi;
        }
        collectFocusable(child);
      }
    }

    if (node.widget->isFocusable()) {
      focusOrder.push_back(node.widget.get());
    }
  }

  Widget* findHit(LayoutNode& node, std::size_t x, std::size_t y) {
    if (!node.widget) return nullptr;

    if (auto* multi = dynamic_cast<MultiChildWidget*>(node.widget.get())) {
      for (auto it = multi->base.children.rbegin(); it != multi->base.children.rend(); ++it) {
        if (it->rect.contains(x, y)) {
          if (Widget* hit = findHit(*it, x, y)) {
            return hit;
          }
        }
      }
    } else if (auto* single = dynamic_cast<SingleChildWidget*>(node.widget.get())) {
      if (single->base.child.rect.contains(x, y)) {
        if (Widget* hit = findHit(single->base.child, x, y)) {
          return hit;
        }
      }
    }

    if (node.rect.contains(x, y)) {
      return node.widget.get();
    }

    return nullptr;
  }

  bool dispatchBubble(Widget* node, const Event& event) {
    Widget* current = node;
    while (current) {
      if (current->handleEvent(event)) {
        return true;
      }
      current = current->parent;
    }
    return false;
  }
};

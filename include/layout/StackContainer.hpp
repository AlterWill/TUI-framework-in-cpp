#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "core/multiChildWidget.hpp"

struct StackBase {
  Rect rect{};
};

struct Stack : public MultiChildWidget {
  StackBase stackBase;

  Stack() = default;

  // Builder methods
  Stack& withPadding(Insets p) {
    base.widgetBase.padding = p;
    return *this;
  }

  Stack& addChild(LayoutNode node) {
    base.children.push_back(std::move(node));
    return *this;
  }

  Stack& addChild(std::unique_ptr<Widget> w, HorizontalAlignment hAlign = HorizontalAlignment::Left,
                  VerticalAlignment vAlign = VerticalAlignment::Top) {
    LayoutNode node;
    node.widget = std::move(w);
    node.horizontalAlignment = hAlign;
    node.verticalAlignment = vAlign;
    base.children.push_back(std::move(node));
    return *this;
  }

  bool handleEvent(const Event&) override { return false; }

  Size measure(const SizeConstraints& constraints) override {
    if (base.children.empty()) {
      return Size{0, 0};
    }

    const auto& padding = base.widgetBase.padding;
    std::size_t padW = padding.left + padding.right;
    std::size_t padH = padding.top + padding.bottom;

    std::size_t usableMaxW = constraints.getMaxWidth() > padW ? constraints.getMaxWidth() - padW : 0;
    std::size_t usableMaxH = constraints.getMaxHeight() > padH ? constraints.getMaxHeight() - padH : 0;

    std::size_t maxChildW = 0;
    std::size_t maxChildH = 0;

    for (auto& child : base.children) {
      if (!child.widget) continue;

      std::size_t margW = child.margin.left + child.margin.right;
      std::size_t margH = child.margin.top + child.margin.bottom;

      std::size_t childMaxW = usableMaxW > margW ? usableMaxW - margW : 0;
      std::size_t childMaxH = usableMaxH > margH ? usableMaxH - margH : 0;

      if (child.dirty || child.measured.width == 0 || child.measured.height == 0) {
        SizeConstraints sc{Size{0, 0}, Size{childMaxH, childMaxW}};
        child.constraints = sc;
        child.measured = child.widget->measure(sc);
        child.dirty = false;
      }

      maxChildW = std::max(maxChildW, child.measured.width + margW);
      maxChildH = std::max(maxChildH, child.measured.height + margH);
    }

    std::size_t finalW = std::clamp(maxChildW + padW, constraints.getMinWidth(), constraints.getMaxWidth());
    std::size_t finalH = std::clamp(maxChildH + padH, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{finalH, finalW};
  }

  void setRectForChildren() override {
    if (base.children.empty()) return;

    const auto& padding = base.widgetBase.padding;
    std::size_t startX = stackBase.rect.x + padding.left;
    std::size_t startY = stackBase.rect.y + padding.top;

    std::size_t usableW =
        stackBase.rect.width > (padding.left + padding.right) ? stackBase.rect.width - padding.left - padding.right : 0;
    std::size_t usableH =
        stackBase.rect.height > (padding.top + padding.bottom) ? stackBase.rect.height - padding.top - padding.bottom : 0;

    for (auto& child : base.children) {
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;

      std::size_t childX = startX + child.margin.left;
      std::size_t totalChildW = childW + child.margin.left + child.margin.right;
      if (usableW > totalChildW) {
        std::size_t extraW = usableW - totalChildW;
        if (child.horizontalAlignment == HorizontalAlignment::Center) {
          childX += extraW / 2;
        } else if (child.horizontalAlignment == HorizontalAlignment::Right) {
          childX += extraW;
        }
      }

      std::size_t childY = startY + child.margin.top;
      std::size_t totalChildH = childH + child.margin.top + child.margin.bottom;
      if (usableH > totalChildH) {
        std::size_t extraH = usableH - totalChildH;
        if (child.verticalAlignment == VerticalAlignment::Center) {
          childY += extraH / 2;
        } else if (child.verticalAlignment == VerticalAlignment::Bottom) {
          childY += extraH;
        }
      }

      child.rect = Rect{childX, childY, childH, childW};
    }
  }
};

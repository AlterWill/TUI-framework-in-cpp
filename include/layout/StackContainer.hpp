#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "core/multiChildWidget.hpp"
#include "rendering/renderContext.hpp"
#include "styling/style.hpp"

struct StackBase {
  Rect rect{};
};

struct Stack : public MultiChildWidget {
  StackBase stackBase;

  bool transparent{false};
  Colour backgroundColour;

  Stack() = default;

  // Builder methods
  Stack& withPadding(Insets p) {
    padding = p;
    return *this;
  }

  Stack& withTransparent(bool t) {
    transparent = t;
    return *this;
  }

  Stack& withBackgroundColour(Colour c) {
    backgroundColour = c;
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

  Size measure(const SizeConstraints& constraints) override {
    if (base.children.empty()) {
      return Size{0, 0};
    }

    std::size_t padW = padding.horizontal();
    std::size_t padH = padding.vertical();

    std::size_t usableMaxW = constraints.getMaxWidth() > padW ? constraints.getMaxWidth() - padW : 0;
    std::size_t usableMaxH = constraints.getMaxHeight() > padH ? constraints.getMaxHeight() - padH : 0;

    std::size_t maxChildW = 0;
    std::size_t maxChildH = 0;

    for (auto& child : base.children) {
      if (!child.widget) continue;

      std::size_t margW = child.margin.horizontal();
      std::size_t margH = child.margin.vertical();

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

  void setRectForChildren(const Rect& rect) override {
    if (base.children.empty()) return;

    stackBase.rect = rect;
    std::size_t startX = rect.x + padding.left;
    std::size_t startY = rect.y + padding.top;

    std::size_t usableW =
        rect.width > padding.horizontal() ? rect.width - padding.horizontal() : 0;
    std::size_t usableH =
        rect.height > padding.vertical() ? rect.height - padding.vertical() : 0;

    for (auto& child : base.children) {
      std::size_t childW = usableW;  // stretch to fill available width
      std::size_t childH = usableH;  // stretch to fill available height

      std::size_t childX = alignCoordinate(startX, usableW, childW,
                                           child.margin.left, child.margin.right,
                                           child.horizontalAlignment);

      std::size_t childY = alignCoordinate(startY, usableH, childH,
                                           child.margin.top, child.margin.bottom,
                                           child.verticalAlignment);

      child.rect = Rect{childX, childY, childH, childW};
    }
  }

  void render(RenderContext& rendercontext) override {
    if (!transparent) {
      const Rect& r = rendercontext.getRect();
      Cell fill;
      fill.glyph = U' ';
      fill.style.colours.bg = backgroundColour;

      for (std::size_t y = r.y; y < r.y + r.height; ++y) {
        for (std::size_t x = r.x; x < r.x + r.width; ++x) {
          rendercontext.setCell(x, y, fill);
        }
      }
    }

    MultiChildWidget::render(rendercontext);
  }
};

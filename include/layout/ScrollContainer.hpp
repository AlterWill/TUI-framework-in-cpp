#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>

#include "core/singleChildWidget.hpp"
#include "rendering/buffer.hpp"

enum class ScrollDirection {
  Vertical,
  Horizontal,
  Both
};

struct ScrollBase {
  Rect rect{};
  std::size_t offsetX{0};
  std::size_t offsetY{0};
  ScrollDirection direction{ScrollDirection::Vertical};
  Buffer contentBuffer{};
};

struct Scroll : public SingleChildWidget {
  ScrollBase scrollBase;

  Scroll() = default;
  explicit Scroll(std::unique_ptr<Widget> childWidget, ScrollDirection dir = ScrollDirection::Vertical)
      : SingleChildWidget(std::move(childWidget)) {
    scrollBase.direction = dir;
  }
  explicit Scroll(LayoutNode childNode, ScrollDirection dir = ScrollDirection::Vertical)
      : SingleChildWidget(std::move(childNode)) {
    scrollBase.direction = dir;
  }

  // Builder methods
  Scroll& withDirection(ScrollDirection dir) {
    scrollBase.direction = dir;
    return *this;
  }

  Scroll& withOffset(std::size_t x, std::size_t y) {
    scrollBase.offsetX = x;
    scrollBase.offsetY = y;
    return *this;
  }

  Scroll& withPadding(Insets p) {
    base.widgetBase.padding = p;
    return *this;
  }

  // Measure: Unbounds the scroll axis for the child
  Size measure(const SizeConstraints& constraints) override {
    if (!base.child.widget) return Size{0, 0};

    const auto& padding = base.widgetBase.padding;
    std::size_t padW = padding.left + padding.right;
    std::size_t padH = padding.top + padding.bottom;

    std::size_t maxW = constraints.getMaxWidth() > padW ? constraints.getMaxWidth() - padW : 0;
    std::size_t maxH = constraints.getMaxHeight() > padH ? constraints.getMaxHeight() - padH : 0;

    // Unbound scroll axis
    std::size_t childMaxW = (scrollBase.direction == ScrollDirection::Horizontal ||
                             scrollBase.direction == ScrollDirection::Both)
                                ? static_cast<std::size_t>(-1)
                                : maxW;

    std::size_t childMaxH = (scrollBase.direction == ScrollDirection::Vertical ||
                             scrollBase.direction == ScrollDirection::Both)
                                ? static_cast<std::size_t>(-1)
                                : maxH;

    SizeConstraints childConstraints{Size{0, 0}, Size{childMaxH, childMaxW}};
    base.child.measured = base.child.widget->measure(childConstraints);

    // Viewport size clamped to parent's constraints
    std::size_t viewportW =
        std::clamp(base.child.measured.width + padW, constraints.getMinWidth(), constraints.getMaxWidth());
    std::size_t viewportH =
        std::clamp(base.child.measured.height + padH, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{viewportH, viewportW};
  }

  void setRectForChild() override {
    if (!base.child.widget) return;

    std::size_t contentW = std::max(base.child.measured.width, scrollBase.rect.width);
    std::size_t contentH = std::max(base.child.measured.height, scrollBase.rect.height);

    // Child renders relative to (0, 0) inside offscreen buffer
    base.child.rect = Rect{0, 0, contentH, contentW};

    // Ensure offscreen buffer matches virtual content dimension
    if (scrollBase.contentBuffer.getWidth() != contentW || scrollBase.contentBuffer.getHeight() != contentH) {
      scrollBase.contentBuffer.resize(contentW, contentH);
    }
  }

  // Render: Uses offscreen buffer and blits visible viewport
  void render(RenderContext& rendercontext) override {
    if (!base.child.widget) return;

    scrollBase.contentBuffer.clear();
    RenderContext offscreenCtx{scrollBase.contentBuffer, base.child.rect};

    // Child draws into offscreen buffer
    base.child.widget->render(offscreenCtx);

    const auto& padding = base.widgetBase.padding;
    const Rect& viewportRect = rendercontext.getRect();

    std::size_t padW = padding.left + padding.right;
    std::size_t padH = padding.top + padding.bottom;
    std::size_t visibleW = viewportRect.width > padW ? viewportRect.width - padW : 0;
    std::size_t visibleH = viewportRect.height > padH ? viewportRect.height - padH : 0;

    // Clamp scroll offsets
    std::size_t maxOffsetX = (base.child.rect.width > visibleW) ? base.child.rect.width - visibleW : 0;
    std::size_t maxOffsetY = (base.child.rect.height > visibleH) ? base.child.rect.height - visibleH : 0;
    scrollBase.offsetX = std::min(scrollBase.offsetX, maxOffsetX);
    scrollBase.offsetY = std::min(scrollBase.offsetY, maxOffsetY);

    Rect srcViewport{scrollBase.offsetX, scrollBase.offsetY, visibleH, visibleW};
    Rect dstViewport{viewportRect.x + padding.left, viewportRect.y + padding.top, visibleH, visibleW};

    // Blit from offscreen buffer into actual terminal framebuffer
    scrollBase.contentBuffer.blitTo(rendercontext.getBuffer(), srcViewport, dstViewport);
  }

  bool handleEvent(const Event& event) override {
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      if (mouse->action == MouseAction::ScrollDown) {
        scrollBase.offsetY += 1;
        return true;
      } else if (mouse->action == MouseAction::ScrollUp) {
        if (scrollBase.offsetY > 0) scrollBase.offsetY -= 1;
        return true;
      }
    }
    return false;
  }
};

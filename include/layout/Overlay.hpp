#pragma once

#include <cstddef>
#include <functional>
#include <vector>

#include "core/layoutNode.hpp"
#include "core/widget.hpp"
#include "layout/StackContainer.hpp"
#include "rendering/renderContext.hpp"

struct OverlayMeta {
  bool modal{false};
  bool dismissOnClickOutside{true};
  std::function<void()> onDismiss;
};

struct Overlay : public Stack {
  std::vector<OverlayMeta> overlayMeta;

  Overlay() = default;

  Overlay& withPadding(Insets p) {
    padding = p;
    return *this;
  }

  Overlay& withTransparent(bool t) {
    transparent = t;
    return *this;
  }

  Overlay& withBackgroundColour(Colour c) {
    backgroundColour = c;
    return *this;
  }

  Overlay& setBaseLayer(std::unique_ptr<Widget> w) {
    ensureBaseLayerExists();
    base.children[0].widget = std::move(w);
    return *this;
  }

  Overlay& setBaseLayer(LayoutNode node) {
    ensureBaseLayerExists();
    base.children[0] = std::move(node);
    return *this;
  }

  Overlay& addOverlay(std::unique_ptr<Widget> w, bool modal = false,
                       bool dismissOnClickOutside = true, std::function<void()> onDismiss = nullptr) {
    LayoutNode node;
    node.widget = std::move(w);
    base.children.push_back(std::move(node));
    overlayMeta.push_back({modal, dismissOnClickOutside, std::move(onDismiss)});
    return *this;
  }

  Overlay& addOverlay(LayoutNode node, bool modal = false,
                       bool dismissOnClickOutside = true, std::function<void()> onDismiss = nullptr) {
    base.children.push_back(std::move(node));
    overlayMeta.push_back({modal, dismissOnClickOutside, std::move(onDismiss)});
    return *this;
  }

  void dismissTopOverlay() {
    if (overlayMeta.empty()) return;

    auto& meta = overlayMeta.back();
    if (meta.onDismiss) meta.onDismiss();
    overlayMeta.pop_back();
    base.children.pop_back();
  }

  void dismissAllOverlays() {
    for (auto& meta : overlayMeta) {
      if (meta.onDismiss) meta.onDismiss();
    }
    std::size_t baseLayerCount = base.children.size() - overlayMeta.size();
    overlayMeta.clear();
    base.children.resize(baseLayerCount);
  }

  std::size_t overlayCount() const { return overlayMeta.size(); }

  bool hasModalOverlay() const {
    for (const auto& meta : overlayMeta) {
      if (meta.modal) return true;
    }
    return false;
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

      SizeConstraints sc{Size{0, 0}, Size{usableMaxH, usableMaxW}};
      child.constraints = sc;
      child.measured = child.widget->measure(sc);
      child.dirty = false;

      maxChildW = std::max(maxChildW, child.measured.width);
      maxChildH = std::max(maxChildH, child.measured.height);
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
      std::size_t childW = child.measured.width;
      std::size_t childH = child.measured.height;

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

  bool handleEvent(const Event& event) override {
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      // Find the topmost modal overlay and check for backdrop click
      for (std::size_t i = 0; i < overlayMeta.size(); ++i) {
        std::size_t metaIdx = overlayMeta.size() - 1 - i;
        std::size_t childIdx = metaIdx + 1; // +1 because children[0] is base layer

        if (childIdx >= base.children.size()) continue;

        if (overlayMeta[metaIdx].modal) {
          const Rect& modalRect = base.children[childIdx].rect;

          if (mouse->action == MouseAction::Press) {
            if (!modalRect.contains(mouse->x, mouse->y) && overlayMeta[metaIdx].dismissOnClickOutside) {
              dismissTopOverlay();
              return true;
            }
          }

          if (modalRect.contains(mouse->x, mouse->y)) {
            if (base.children[childIdx].widget && base.children[childIdx].widget->handleEvent(event)) {
              return true;
            }
          }
          break; // topmost modal blocks everything below
        }
      }

      // No modal blocking: forward to children top-to-bottom (reverse order)
      for (auto it = base.children.rbegin(); it != base.children.rend(); ++it) {
        if (it->widget && it->rect.contains(mouse->x, mouse->y)) {
          if (it->widget->handleEvent(event)) return true;
        }
      }
    }
    return false;
  }

 private:
  void ensureBaseLayerExists() {
    if (base.children.empty()) {
      base.children.emplace_back();
    }
  }
};

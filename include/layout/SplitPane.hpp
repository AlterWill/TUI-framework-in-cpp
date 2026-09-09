#pragma once

#include <algorithm>
#include <cstddef>

#include "core/layoutNode.hpp"
#include "core/widget.hpp"
#include "rendering/renderContext.hpp"
#include "utilities/Orientation.hpp"
#include "utilities/dividerStyle.hpp"
#include "widgets/divider.hpp"

struct SplitPane : public Widget {
  WidgetBase widgetBase;
  LayoutNode first;
  LayoutNode second;

  Rect paneRect{};
  Rect dividerRect{};
  Orientation orientation{Orientation::Horizontal};
  double splitRatio{0.5};
  std::size_t dividerThickness{1};
  ColourPair dividerColours{};
  double minRatio{0.1};
  double maxRatio{0.9};
  bool dividerVisible{true};
  bool dragging{false};
  std::size_t dragStart{0};
  double dragStartRatio{0.5};
  divider dividerWidget;

  SplitPane() = default;

  SplitPane(std::unique_ptr<Widget> firstChild, std::unique_ptr<Widget> secondChild,
            Orientation orient = Orientation::Horizontal) {
    orientation = orient;
    first.widget = std::move(firstChild);
    second.widget = std::move(secondChild);
    dividerWidget.orientation = (orient == Orientation::Horizontal) ? Orientation::Vertical : Orientation::Horizontal;
    dividerWidget.dividerGlyph = dividerStyle::vertical::doubleBorder;
  }

  SplitPane& withOrientation(Orientation orient) {
    orientation = orient;
    dividerWidget.withOrientation(
        (orient == Orientation::Horizontal) ? Orientation::Vertical : Orientation::Horizontal);
    return *this;
  }

  SplitPane& withSplitRatio(double ratio) {
    splitRatio = std::clamp(ratio, minRatio, maxRatio);
    return *this;
  }

  SplitPane& withMinRatio(double r) {
    minRatio = r;
    return *this;
  }

  SplitPane& withMaxRatio(double r) {
    maxRatio = r;
    return *this;
  }

  SplitPane& withDividerThickness(std::size_t t) {
    dividerThickness = t;
    return *this;
  }

  SplitPane& withDividerGlyph(char32_t g) {
    dividerWidget.withGlyph(g);
    return *this;
  }

  SplitPane& withDividerColours(ColourPair c) {
    dividerColours = c;
    dividerWidget.withColours(c);
    return *this;
  }

  SplitPane& withDividerVisible(bool v) {
    dividerVisible = v;
    return *this;
  }

  SplitPane& withFirstChild(std::unique_ptr<Widget> w) {
    first.widget = std::move(w);
    return *this;
  }

  SplitPane& withSecondChild(std::unique_ptr<Widget> w) {
    second.widget = std::move(w);
    return *this;
  }

  SplitPane& withPadding(Insets p) {
    widgetBase.padding = p;
    return *this;
  }

  Size measure(const SizeConstraints& constraints) override {
    const auto& padding = widgetBase.padding;
    std::size_t padW = padding.left + padding.right;
    std::size_t padH = padding.top + padding.bottom;
    std::size_t divSize = dividerVisible ? dividerThickness : 0;

    std::size_t maxW = constraints.getMaxWidth() > padW ? constraints.getMaxWidth() - padW : 0;
    std::size_t maxH = constraints.getMaxHeight() > padH ? constraints.getMaxHeight() - padH : 0;

    bool horizontal = (orientation == Orientation::Horizontal);
    std::size_t dividerSpace = divSize;

    std::size_t childMaxW = horizontal ? (maxW > dividerSpace ? maxW - dividerSpace : 0) : maxW;
    std::size_t childMaxH = horizontal ? maxH : (maxH > dividerSpace ? maxH - dividerSpace : 0);

    SizeConstraints childConstraints{Size{0, 0}, Size{childMaxH, childMaxW}};

    if (first.widget) {
      first.constraints = childConstraints;
      first.measured = first.widget->measure(childConstraints);
    } else {
      first.measured = Size{0, 0};
    }

    if (second.widget) {
      second.constraints = childConstraints;
      second.measured = second.widget->measure(childConstraints);
    } else {
      second.measured = Size{0, 0};
    }

    std::size_t totalW;
    std::size_t totalH;

    if (horizontal) {
      totalW = first.measured.width + second.measured.width + dividerSpace;
      totalH = std::max(first.measured.height, second.measured.height);
    } else {
      totalH = first.measured.height + second.measured.height + dividerSpace;
      totalW = std::max(first.measured.width, second.measured.width);
    }

    totalW = std::clamp(totalW + padW, constraints.getMinWidth(), constraints.getMaxWidth());
    totalH = std::clamp(totalH + padH, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{totalH, totalW};
  }

  void layout(const Rect& rect) override {
    paneRect = rect;
    const auto& padding = widgetBase.padding;
    std::size_t divSize = dividerVisible ? dividerThickness : 0;
    bool horizontal = (orientation == Orientation::Horizontal);

    std::size_t startX = rect.x + padding.left;
    std::size_t startY = rect.y + padding.top;
    std::size_t usableW =
        rect.width > (padding.left + padding.right) ? rect.width - padding.left - padding.right : 0;
    std::size_t usableH =
        rect.height > (padding.top + padding.bottom) ? rect.height - padding.top - padding.bottom : 0;

    if (usableW == 0 || usableH == 0) return;

    double ratio = std::clamp(splitRatio, minRatio, maxRatio);

    if (horizontal) {
      std::size_t splitSpace = usableW > divSize ? usableW - divSize : 0;
      std::size_t firstW = static_cast<std::size_t>(static_cast<double>(splitSpace) * ratio);
      std::size_t secondW = splitSpace > firstW ? splitSpace - firstW : 0;

      first.rect = Rect{startX, startY, usableH, firstW};
      second.rect = Rect{startX + firstW + divSize, startY, usableH, secondW};
      dividerRect = Rect{startX + firstW, startY, usableH, divSize};
    } else {
      std::size_t splitSpace = usableH > divSize ? usableH - divSize : 0;
      std::size_t firstH = static_cast<std::size_t>(static_cast<double>(splitSpace) * ratio);
      std::size_t secondH = splitSpace > firstH ? splitSpace - firstH : 0;

      first.rect = Rect{startX, startY, firstH, usableW};
      second.rect = Rect{startX, startY + firstH + divSize, secondH, usableW};
      dividerRect = Rect{startX, startY + firstH, divSize, usableW};
    }

    if (first.widget) {
      first.widget->layout(first.rect);
    }
    if (second.widget) {
      second.widget->layout(second.rect);
    }
  }

  void render(RenderContext& rendercontext) override {
    if (first.widget) {
      rendercontext.setRect(first.rect);
      first.widget->render(rendercontext);
    }

    if (second.widget) {
      rendercontext.setRect(second.rect);
      second.widget->render(rendercontext);
    }

    if (dividerVisible && dividerThickness > 0) {
      rendercontext.setRect(dividerRect);
      dividerWidget.render(rendercontext);
    }
  }

  bool handleEvent(const Event& event) override {
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      if (dragging) {
        if (mouse->action == MouseAction::Release) {
          dragging = false;
          return true;
        }
        if (mouse->action == MouseAction::Drag) {
          updateSplitFromDrag(mouse->x, mouse->y);
          return true;
        }
      }

      if (mouse->action == MouseAction::Press && dividerVisible && paneRect.contains(mouse->x, mouse->y)) {
        dragging = true;
        dragStartRatio = splitRatio;
        dragStart = (orientation == Orientation::Horizontal) ? mouse->x : mouse->y;
        updateSplitFromDrag(mouse->x, mouse->y);
        return true;
      }

      if (second.widget && second.rect.contains(mouse->x, mouse->y)) {
        if (second.widget->handleEvent(event)) return true;
      }
      if (first.widget && first.rect.contains(mouse->x, mouse->y)) {
        if (first.widget->handleEvent(event)) return true;
      }
    }
    return false;
  }

 private:
  void updateSplitFromDrag(std::size_t mx, std::size_t my) {
    bool horizontal = (orientation == Orientation::Horizontal);
    std::size_t divSize = dividerVisible ? dividerThickness : 0;

    if (horizontal) {
      std::size_t startX = first.rect.x;
      std::size_t endX = second.rect.x + second.rect.width;
      std::size_t totalSpan = endX > startX ? endX - startX : 0;
      std::size_t effectiveSpan = totalSpan > divSize ? totalSpan - divSize : 0;
      if (effectiveSpan == 0) return;

      std::size_t pos = mx > startX ? mx - startX : 0;
      if (pos > effectiveSpan) pos = effectiveSpan;

      splitRatio = std::clamp(static_cast<double>(pos) / static_cast<double>(effectiveSpan),
                              minRatio, maxRatio);
    } else {
      std::size_t startY = first.rect.y;
      std::size_t endY = second.rect.y + second.rect.height;
      std::size_t totalSpan = endY > startY ? endY - startY : 0;
      std::size_t effectiveSpan = totalSpan > divSize ? totalSpan - divSize : 0;
      if (effectiveSpan == 0) return;

      std::size_t pos = my > startY ? my - startY : 0;
      if (pos > effectiveSpan) pos = effectiveSpan;

      splitRatio = std::clamp(static_cast<double>(pos) / static_cast<double>(effectiveSpan),
                              minRatio, maxRatio);
    }
  }
};

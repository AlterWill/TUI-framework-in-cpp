#pragma once

#include <algorithm>
#include <string>

#include "core/singleChildWidget.hpp"
#include "utilities/alignment.hpp"

struct boxOutlineDetails {
  char32_t horizontal;
  char32_t vertical;
  char32_t topLeft;
  char32_t topRight;
  char32_t bottomLeft;
  char32_t bottomRight;
};

// clang-format off
namespace boxStyle {
inline constexpr boxOutlineDetails light        = {U'─', U'│', U'┌', U'┐', U'└', U'┘'};
inline constexpr boxOutlineDetails heavy        = {U'━', U'┃', U'┏', U'┓', U'┗', U'┛'};
inline constexpr boxOutlineDetails doubleBorder = {U'═', U'║', U'╔', U'╗', U'╚', U'╝'};
inline constexpr boxOutlineDetails rounded      = {U'─', U'│', U'╭', U'╮', U'╰', U'╯'};
inline constexpr boxOutlineDetails dashed       = {U'╌', U'╎', U'┌', U'┐', U'└', U'┘'};
inline constexpr boxOutlineDetails block        = {U'█', U'█', U'█', U'█', U'█', U'█'};
inline constexpr boxOutlineDetails ascii        = {U'-', U'|', U'+', U'+', U'+', U'+'};
}
// clang-format on

struct BoxBase {
  std::string title;
  HorizontalAlignment alignment{HorizontalAlignment::Left};
  bool titleTop{true};
  boxOutlineDetails outline{boxStyle::light};
  std::size_t borderSize{1};
  ColourPair colours;
};

struct Box : public SingleChildWidget {
  BoxBase boxBase;

  Box() = default;
  explicit Box(std::unique_ptr<Widget> childWidget) : SingleChildWidget(std::move(childWidget)) {}
  explicit Box(LayoutNode childNode) : SingleChildWidget(std::move(childNode)) {}

  // Builder methods
  Box& withTitle(std::string t, HorizontalAlignment a = HorizontalAlignment::Left, bool top = true) {
    boxBase.title = std::move(t);
    boxBase.alignment = a;
    boxBase.titleTop = top;
    return *this;
  }

  Box& withOutline(boxOutlineDetails o) {
    boxBase.outline = o;
    return *this;
  }

  Box& withColours(ColourPair c) {
    boxBase.colours = c;
    return *this;
  }

  Box& withBorderSize(std::size_t b) {
    boxBase.borderSize = b;
    return *this;
  }

  Box& withPadding(Insets p) {
    base.widgetBase.padding = p;
    return *this;
  }

  Box& withChildMargin(Insets m) {
    base.child.margin = m;
    return *this;
  }

  Size measure(const SizeConstraints& constraints) override {
    const auto& padding = base.widgetBase.padding;
    const auto& child = base.child;

    std::size_t extraWidth = (2 * boxBase.borderSize) + padding.left + padding.right +
                             (child.widget ? (child.margin.left + child.margin.right) : 0);
    std::size_t extraHeight = (2 * boxBase.borderSize) + padding.top + padding.bottom +
                              (child.widget ? (child.margin.top + child.margin.bottom) : 0);

    if (constraints.getMaxWidth() < extraWidth || constraints.getMaxHeight() < extraHeight) {
      return Size{0, 0};
    }

    std::size_t contentWidth = 0;
    std::size_t contentHeight = 0;

    if (child.widget) {
      SizeConstraints childConstraints;
      childConstraints.setMinHeight(
          constraints.getMinHeight() > extraHeight ? constraints.getMinHeight() - extraHeight : 0
      );
      childConstraints.setMaxHeight(constraints.getMaxHeight() - extraHeight);
      childConstraints.setMinWidth(
          constraints.getMinWidth() > extraWidth ? constraints.getMinWidth() - extraWidth : 0
      );
      childConstraints.setMaxWidth(constraints.getMaxWidth() - extraWidth);

      Size childSize = child.widget->measure(childConstraints);
      if (childSize.width == 0 || childSize.height == 0) {
        return Size{0, 0};
      }

      contentWidth = childSize.width;
      contentHeight = childSize.height;
    }

    std::size_t totalWidth =
        std::clamp(contentWidth + extraWidth, constraints.getMinWidth(), constraints.getMaxWidth());
    std::size_t totalHeight =
        std::clamp(contentHeight + extraHeight, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{totalHeight, totalWidth};
  }

  void drawBorder(RenderContext& rendercontext) {
    const Rect& rect = rendercontext.getRect();
    const auto& padding = base.widgetBase.padding;
    const auto& child = base.child;

    std::size_t extraW = (2 * boxBase.borderSize) + padding.left + padding.right +
                         (child.widget ? (child.margin.left + child.margin.right) : 0);
    std::size_t extraH = (2 * boxBase.borderSize) + padding.top + padding.bottom +
                         (child.widget ? (child.margin.top + child.margin.bottom) : 0);

    if (rect.width < extraW || rect.height < extraH) {
      return;
    }

    std::size_t left = rect.x;
    std::size_t top = rect.y;
    std::size_t right = rect.x + rect.width - boxBase.borderSize;
    std::size_t bottom = rect.y + rect.height - boxBase.borderSize;

    Cell borderCell;
    borderCell.setColour(boxBase.colours);

    borderCell.setGlyph(boxBase.outline.topLeft);
    rendercontext.setCell(left, top, borderCell);

    borderCell.setGlyph(boxBase.outline.topRight);
    rendercontext.setCell(right, top, borderCell);

    borderCell.setGlyph(boxBase.outline.bottomLeft);
    rendercontext.setCell(left, bottom, borderCell);

    borderCell.setGlyph(boxBase.outline.bottomRight);
    rendercontext.setCell(right, bottom, borderCell);

    borderCell.setGlyph(boxBase.outline.horizontal);
    for (std::size_t x = left + 1; x < right; x++) {
      rendercontext.setCell(x, top, borderCell);
      rendercontext.setCell(x, bottom, borderCell);
    }

    if (boxBase.title.size() <= rect.width) {
      std::size_t availableWidth =
          (rect.width > 2 * boxBase.borderSize) ? (rect.width - 2 * boxBase.borderSize) : 0;
      std::size_t startingX =
          left + 1 + alignOffset(availableWidth, boxBase.title.size(), boxBase.alignment);
      std::size_t startingY = boxBase.titleTop ? top : bottom;

      for (std::size_t x = startingX; x < startingX + boxBase.title.size() && x < right; x++) {
        rendercontext.setGlyph(x, startingY, static_cast<char32_t>(boxBase.title[x - startingX]));
      }
    }
    borderCell.setGlyph(boxBase.outline.vertical);
    for (std::size_t y = top + 1; y < bottom; y++) {
      rendercontext.setCell(left, y, borderCell);
      rendercontext.setCell(right, y, borderCell);
    }

    Cell bgCell;
    bgCell.setBackgroundColour(boxBase.colours.getBackgroundColour());
    for (std::size_t x = left + boxBase.borderSize; x < right; x++) {
      for (std::size_t y = top + boxBase.borderSize; y < bottom; y++) {
        rendercontext.setStyle(x, y, bgCell.getStyle());
      }
    }
  }

  void setRectForChild(const Rect& rect) override {
    if (!base.child.widget) return;

    std::size_t extraW = (2 * boxBase.borderSize) + base.widgetBase.padding.left + base.widgetBase.padding.right +
                         base.child.margin.left + base.child.margin.right;
    std::size_t extraH = (2 * boxBase.borderSize) + base.widgetBase.padding.top + base.widgetBase.padding.bottom +
                         base.child.margin.top + base.child.margin.bottom;

    base.child.rect = Rect{
        rect.x + boxBase.borderSize + base.widgetBase.padding.left + base.child.margin.left,
        rect.y + boxBase.borderSize + base.widgetBase.padding.top + base.child.margin.top,
        (rect.height > extraH) ? (rect.height - extraH) : 0,
        (rect.width > extraW) ? (rect.width - extraW) : 0
    };
  }

  void render(RenderContext& rendercontext) override {
    drawBorder(rendercontext);
    SingleChildWidget::render(rendercontext);
  }
};

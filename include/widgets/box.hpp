#pragma once

#include <algorithm>

#include "utilities/alignment.hpp"
#include "core/singleChildWidget.hpp"

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
//            horizontal; vertical; topLeft; topRight; bottomLeft; bottomRight;
inline constexpr boxOutlineDetails light = {U'─', U'│', U'┌', U'┐', U'└', U'┘'};
inline constexpr boxOutlineDetails heavy = {U'━', U'┃', U'┏', U'┓', U'┗', U'┛'};
inline constexpr boxOutlineDetails doubleBorder = {U'═', U'║', U'╔', U'╗', U'╚', U'╝'};
inline constexpr boxOutlineDetails rounded = {U'─', U'│', U'╭', U'╮', U'╰', U'╯'};
inline constexpr boxOutlineDetails dashed = {U'╌', U'╎', U'┌', U'┐', U'└', U'┘'};
inline constexpr boxOutlineDetails block = {U'█', U'█', U'█', U'█', U'█', U'█'};
inline constexpr boxOutlineDetails ascii = {U'-', U'|', U'+', U'+', U'+', U'+'};
}
// clang-format on

class Box : public SingleChildWidget {
 public:
  std::string title;
  Alignment alignment;
  bool titleTop{true};
  boxOutlineDetails outline;
  std::size_t borderSize{1};
  ColourPair colours;

  Box(std::unique_ptr<Widget> mainChild,
      boxOutlineDetails o = boxStyle::light,
      ColourPair c = ColourPair{},
      std::size_t b = 1)
      : SingleChildWidget(std::move(mainChild)), outline(o), borderSize(b), colours(c) {}

  Box(std::unique_ptr<Widget> mainChild,
      const std::string& t,
      Alignment a = Alignment::left,
      bool tt = true,
      boxOutlineDetails o = boxStyle::light,
      ColourPair c = ColourPair{},
      std::size_t b = 1)
      : SingleChildWidget(std::move(mainChild)),
        title(std::move(t)),
        alignment(a),
        titleTop(tt),
        outline(o),
        borderSize(b),
        colours(c) {}

  Size intrinsicSize() override {
    if (!child) return Size{0, 0};
    Size result{child->intrinsicSize()};
    result.setHeight(
        result.getHeight() + padding.getTop() + padding.getBottom() + child->margin.getTop() +
        child->margin.getBottom() + (2 * borderSize)
    );
    result.setWidth(
        result.getWidth() + padding.getLeft() + padding.getRight() + child->margin.getLeft() +
        child->margin.getRight() + (2 * borderSize)
    );
    return result;
  }

  // possible to write the function in a better way by moving the if child higher
  Size measure(const SizeConstraints& constraints) override {
    std::size_t extraWidth =
        (2 * borderSize) + padding.left + padding.right + child->margin.getLeft() + child->margin.getRight();
    std::size_t extraHeight =
        (2 * borderSize) + padding.top + padding.bottom + child->margin.getTop() + child->margin.getBottom();

    // A box must be at least large enough to draw its borders (min 2x2 if borderSize=1)
    if (constraints.getMaxWidth() < extraWidth || constraints.getMaxHeight() < extraHeight) {
      return Size{0, 0};
    }

    if (child) {
      SizeConstraints childConstraints;
      childConstraints.setMinWidth(constraints.getMinWidth() > extraWidth ? constraints.getMinWidth() - extraWidth : 0);
      childConstraints.setMinHeight(
          constraints.getMinHeight() > extraHeight ? constraints.getMinHeight() - extraHeight : 0
      );
      childConstraints.setMaxWidth(constraints.getMaxWidth() - extraWidth);
      childConstraints.setMaxHeight(constraints.getMaxHeight() - extraHeight);

      Size childSize = child->measure(childConstraints);

      // If the child requires 0 width or 0 height, it cannot be displayed.
      if (childSize.getWidth() == 0 || childSize.getHeight() == 0) {
        return Size{0, 0};
      }

      std::size_t width =
          std::clamp(childSize.getWidth() + extraWidth, constraints.getMinWidth(), constraints.getMaxWidth());
      std::size_t height =
          std::clamp(childSize.getHeight() + extraHeight, constraints.getMinHeight(), constraints.getMaxHeight());
      return Size{height, width};
    }

    // Empty box returns just the border/padding size if it fits, clamped to constraints
    return Size{0, 0};
  }

  void drawBorder(RenderContext& rendercontext) {
    if (rect.width < child->margin.getLeft() + child->margin.getRight() + padding.getLeft() + padding.getRight() +
                         (2 * borderSize) ||
        rect.height < child->margin.getTop() + child->margin.getBottom() + padding.getTop() + padding.getBottom() +
                          (2 * borderSize))
      return;

    std::size_t left = rect.x;
    std::size_t top = rect.y;
    std::size_t right = rect.x + rect.width - borderSize;
    std::size_t bottom = rect.y + rect.height - borderSize;

    Cell borderCell;
    borderCell.setColour(colours);

    borderCell.setGlyph(outline.topLeft);
    rendercontext.setCell(left, top, borderCell);

    borderCell.setGlyph(outline.topRight);
    rendercontext.setCell(right, top, borderCell);

    borderCell.setGlyph(outline.bottomLeft);
    rendercontext.setCell(left, bottom, borderCell);

    borderCell.setGlyph(outline.bottomRight);
    rendercontext.setCell(right, bottom, borderCell);

    borderCell.setGlyph(outline.horizontal);
    for (std::size_t x = left + 1; x < right; x++) {
      rendercontext.setCell(x, top, borderCell);
      rendercontext.setCell(x, bottom, borderCell);
    }

    if (title.size() <= rect.width) {
      std::size_t remainingSpace =
          std::max(static_cast<std::size_t>(0), rect.getWidth() - (2 * borderSize) - title.size());
      std::size_t startingX = left + 1;
      std::size_t startingY = top;

      if (alignment == Alignment::center) {
        startingX += remainingSpace / 2;
      } else if (alignment == Alignment::right) {
        startingX += remainingSpace;
      }
      if (!titleTop) {
        startingY = bottom;
      }

      for (std::size_t x = startingX; x < startingX + title.size() && x < right; x++) {
        rendercontext.setGlyph(x, startingY, static_cast<char32_t>(title[x - startingX]));
      }
    }
    borderCell.setGlyph(outline.vertical);
    for (std::size_t y = top + 1; y < bottom; y++) {
      rendercontext.setCell(left, y, borderCell);
      rendercontext.setCell(right, y, borderCell);
    }

    // not sure if box should do set the style for it children or not
    Cell bgCell;
    bgCell.setBackgroundColour(colours.getBackgroundColour());
    for (std::size_t x = left + borderSize; x < right; x++) {
      for (std::size_t y = top + borderSize; y < bottom; y++) {
        rendercontext.setStyle(x, y, bgCell.getStyle());
      }
    }
  }

  void render(RenderContext& rendercontext) override {
    if (!child) return;
    drawBorder(rendercontext);
    setRectForChild();
    child->render(rendercontext);
  }

  void setRectForChild() override {
    if (!child) return;

    std::size_t extraW =
        (2 * borderSize) + padding.getLeft() + padding.getRight() + child->margin.getLeft() + child->margin.getRight();
    std::size_t extraH =
        (2 * borderSize) + padding.getTop() + padding.getBottom() + child->margin.getTop() + child->margin.getBottom();
    child->setRect(
        rect.x + borderSize + padding.left + child->margin.left,
        rect.y + borderSize + padding.top + child->margin.top,
        (rect.height > extraH) ? (rect.height - extraH) : 0,
        (rect.width > extraW) ? (rect.width - extraW) : 0
    );
  }
};

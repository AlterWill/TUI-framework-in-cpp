#pragma once

#include <algorithm>

#include "singleChildWidget.hpp"

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
  boxOutlineDetails outline;
  int borderSize{1};
  // no idea what to do with Background Colour
  ColourPair colours;

  Box(std::unique_ptr<Widget> mainChild, boxOutlineDetails o = boxStyle::light, ColourPair c = ColourPair{}, int b = 1)
      : SingleChildWidget(std::move(mainChild)), outline(o), borderSize(b), colours(c) {}

  Size measure(const SizeConstraints& constraints) override {
    std::size_t extraWidth = (2 * borderSize) + padding.left + padding.right;
    std::size_t extraHeight = (2 * borderSize) + padding.top + padding.bottom;

    // A box must be at least large enough to draw its borders (min 2x2 if borderSize=1)
    if (constraints.getMaxWidth() < extraWidth || constraints.getMaxHeight() < extraHeight) {
      return Size{0, 0};
    }

    if (child) {
      SizeConstraints childConstraints;
      childConstraints.setMinWidth(constraints.getMinWidth() > extraWidth ? constraints.getMinWidth() - extraWidth : 0);
      childConstraints.setMinHeight(constraints.getMinHeight() > extraHeight ? constraints.getMinHeight() - extraHeight : 0);
      childConstraints.setMaxWidth(constraints.getMaxWidth() - extraWidth);
      childConstraints.setMaxHeight(constraints.getMaxHeight() - extraHeight);

      Size childSize = child->measure(childConstraints);

      // If the child requires 0 width or 0 height, it cannot be displayed.
      if (childSize.getWidth() == 0 || childSize.getHeight() == 0) {
        return Size{0, 0};
      }

      std::size_t width = std::clamp(childSize.getWidth() + extraWidth, constraints.getMinWidth(), constraints.getMaxWidth());
      std::size_t height = std::clamp(childSize.getHeight() + extraHeight, constraints.getMinHeight(), constraints.getMaxHeight());
      return Size{height, width};
    }

    // Empty box returns just the border/padding size if it fits, clamped to constraints
    return Size{0,0};
  }

  void drawBorder(RenderContext& rendercontext) {
    if (rect.width < 2 || rect.height < 2) return;

    std::size_t right = rect.x + rect.width - 1;
    std::size_t bottom = rect.y + rect.height - 1;

    Cell borderCell;
    borderCell.setColour(colours);

    borderCell.setGlyph(outline.topLeft);
    rendercontext.fb.setCell(rect.x, rect.y, borderCell, rendercontext.clip);

    borderCell.setGlyph(outline.topRight);
    rendercontext.fb.setCell(right, rect.y, borderCell, rendercontext.clip);

    borderCell.setGlyph(outline.bottomLeft);
    rendercontext.fb.setCell(rect.x, bottom, borderCell, rendercontext.clip);

    borderCell.setGlyph(outline.bottomRight);
    rendercontext.fb.setCell(right, bottom, borderCell, rendercontext.clip);

    borderCell.setGlyph(outline.horizontal);
    for (std::size_t x = rect.x + 1; x < right; x++) {
      rendercontext.fb.setCell(x, rect.y, borderCell, rendercontext.clip);
      rendercontext.fb.setCell(x, bottom, borderCell, rendercontext.clip);
    }

    borderCell.setGlyph(outline.vertical);
    for (std::size_t y = rect.y + 1; y < bottom; y++) {
      rendercontext.fb.setCell(rect.x, y, borderCell, rendercontext.clip);
      rendercontext.fb.setCell(right, y, borderCell, rendercontext.clip);
    }

    Cell bgCell;
    bgCell.setBackgroundColour(colours.getBackgroundColour());
    for (std::size_t x = rect.x + 1; x < right; x++) {
      for (std::size_t y = rect.y + 1; y < bottom; y++) {
        rendercontext.fb.setCell(x, y, bgCell, rendercontext.clip);
      }
    }
  }

  void render(RenderContext& rendercontext) override {
    drawBorder(rendercontext);
    if (child) {
      child->render(rendercontext);
    }
  }

  void setRectForChild() override {
    if (child) {
      child->setRect(
          rect.x + borderSize + padding.left + child->margin.left,
          rect.y + borderSize + padding.top + child->margin.top,
          std::max(
              0,
              static_cast<int>(
                  rect.height - (2 * borderSize) - padding.top - padding.bottom - child->margin.top -
                  child->margin.bottom
              )
          ),
          std::max(
              0,
              static_cast<int>(
                  rect.width - (2 * borderSize) - padding.left - padding.right - child->margin.left -
                  child->margin.right
              )
          )
      );
    }
  }
};

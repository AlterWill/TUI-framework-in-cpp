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

  void drawBorder(frameBuffer& fb) {
    if (rect.width < 2 || rect.height < 2) return;
    
    size_t right = rect.x + rect.width - 1;
    size_t bottom = rect.y + rect.height - 1;
    
    Cell borderCell;
    borderCell.setColour(colours);
    
    borderCell.setGlyph(outline.topLeft);
    fb.setCell(rect.x, rect.y, borderCell, clip);
    
    borderCell.setGlyph(outline.topRight);
    fb.setCell(right, rect.y, borderCell, clip);
    
    borderCell.setGlyph(outline.bottomLeft);
    fb.setCell(rect.x, bottom, borderCell, clip);
    
    borderCell.setGlyph(outline.bottomRight);
    fb.setCell(right, bottom, borderCell, clip);

    borderCell.setGlyph(outline.horizontal);
    for (size_t x = rect.x + 1; x < right; x++) {
      fb.setCell(x, rect.y, borderCell, clip);
      fb.setCell(x, bottom, borderCell, clip);
    }
    
    borderCell.setGlyph(outline.vertical);
    for (size_t y = rect.y + 1; y < bottom; y++) {
      fb.setCell(rect.x, y, borderCell, clip);
      fb.setCell(right, y, borderCell, clip);
    }

    Cell bgCell;
    bgCell.setBackgroundColour(colours.getBackgroundColour());
    for (size_t x = rect.x + 1; x < right; x++) {
      for (size_t y = rect.y + 1; y < bottom; y++) {
        fb.setCell(x, y, bgCell, clip);
      }
    }
  }

  void render(frameBuffer& fb) override {
    drawBorder(fb);
    if (child) {
      child->render(fb);
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

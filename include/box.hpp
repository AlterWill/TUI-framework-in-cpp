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
  std::vector<Cell> buffer;
  boxOutlineDetails outline;
  int borderSize{1};
  // no idea what to do with Background Colour
  ColourPair colours;

  Box(std::unique_ptr<Widget> mainChild, boxOutlineDetails o = boxStyle::light, ColourPair c = ColourPair{}, int b = 1)
      : SingleChildWidget(std::move(mainChild)), outline(o), borderSize(b), colours(c) {}

  void drawBorder() {
    if (rect.width < 2 || rect.height < 2) return;
    if (buffer.size() != rect.height * rect.width) {
      buffer.resize(rect.height * rect.width);
    }
    size_t right = rect.width - 1;
    size_t bottom = rect.width * (rect.height - 1);
    Cell borderCell;
    borderCell.setColour(colours);
    borderCell.setGlyph(outline.topLeft);
    buffer[0].setCell(borderCell);
    borderCell.setGlyph(outline.topRight);
    buffer[right].setCell(borderCell);
    borderCell.setGlyph(outline.bottomLeft);
    buffer[bottom].setCell(borderCell);
    borderCell.setGlyph(outline.bottomRight);
    buffer[bottom + right].setCell(borderCell);

    borderCell.setGlyph(outline.horizontal);
    for (size_t x = 1; x < right; x++) {
      buffer[x].setCell(borderCell);
      buffer[bottom + x].setCell(borderCell);
    }
    borderCell.setGlyph(outline.vertical);
    for (size_t y = 1; y < rect.height - 1; y++) {
      buffer[y * rect.width].setCell(borderCell);

      buffer[(y * rect.width) + right].setCell(borderCell);
    }

    Cell bgCell;
    bgCell.setBackgroundColour(colours.getBackgroundColour());
    for (size_t x = 1; x < right; x++) {
      for (size_t y = 1; y < rect.height - 1; y++) {
        buffer[(y * rect.width) + x].setCell(bgCell);
      }
    }
  }

  void passBuffer(frameBuffer& fb) {
    for (size_t x = rect.x; x < rect.width + rect.x; x++) {
      for (size_t y = rect.y; y < rect.height + rect.y; y++) {
        fb.setCell(x, y, buffer[((y - rect.y) * rect.width) + (x - rect.x)]);
      }
    }
  }

  void render(frameBuffer& fb) override {
    drawBorder();
    passBuffer(fb);
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

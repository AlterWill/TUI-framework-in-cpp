#pragma once

#include <algorithm>

#include "frameBuffer.hpp"
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
  Colour colour;

  Box(std::unique_ptr<Widget> mainChild, boxOutlineDetails o = boxStyle::light, Colour c = {}, int b = 1)
      : SingleChildWidget(std::move(mainChild)), outline(o), borderSize(b), colour(c) {}

  void drawBorder(frameBuffer& fb) {
    if (rect.width < 2 || rect.height < 2) return;
    int right = rect.x + rect.width - 1;
    int bottom = rect.y + rect.height - 1;
    fb.setGlyph(rect.x, rect.y, outline.topLeft);
    fb.setForegroundColor(rect.x, rect.y, colour.fg);
    fb.setGlyph(right, rect.y, outline.topRight);
    fb.setForegroundColor(right, rect.y, colour.fg);
    fb.setGlyph(rect.x, bottom, outline.bottomLeft);
    fb.setForegroundColor(rect.x, bottom, colour.fg);
    fb.setGlyph(right, bottom, outline.bottomRight);
    fb.setForegroundColor(right, bottom, colour.fg);

    for (int x = rect.x + 1; x < right; x++) {
      fb.setGlyph(x, rect.y, outline.horizontal);
      fb.setForegroundColor(x, rect.y, colour.fg);
      fb.setGlyph(x, bottom, outline.horizontal);
      fb.setForegroundColor(x, bottom, colour.fg);
    }
    for (int y = rect.y + 1; y < bottom; y++) {
      fb.setGlyph(rect.x, y, outline.vertical);
      fb.setForegroundColor(rect.x, y, colour.fg);
      fb.setGlyph(right, y, outline.vertical);
      fb.setForegroundColor(right, y, colour.fg);
    }

    for (int x = rect.x + 1; x < right; x++) {
      for (int y = rect.y + 1; y < bottom; y++) {
        fb.setBackgroundColor(x, y, colour.bg);
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
              rect.height - (2 * borderSize) - padding.top - padding.bottom - child->margin.top - child->margin.bottom
          ),
          std::max(
              0, rect.width - (2 * borderSize) - padding.left - padding.right - child->margin.left - child->margin.right
          )
      );
    }
  }
};

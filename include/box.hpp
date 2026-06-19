#pragma once

#include "frameBuffer.hpp"
#include "node.hpp"

struct boxOutlineDetails {
  char32_t horizontal;
  char32_t vertical;
  char32_t topLeft;
  char32_t topRight;
  char32_t bottomLeft;
  char32_t bottomRight;
};

namespace boxStyle {
inline constexpr boxOutlineDetails light = {U'─', U'│', U'┌', U'┐', U'└', U'┘'};
// Heavy / Thick line border
inline constexpr boxOutlineDetails heavy = {U'━', U'┃', U'┏', U'┓', U'┗', U'┛'};
// Double line border
inline constexpr boxOutlineDetails doubleBorder = {U'═', U'║', U'╔', U'╗', U'╚', U'╝'};
// Light line border with rounded corners
inline constexpr boxOutlineDetails rounded = {U'─', U'│', U'╭', U'╮', U'╰', U'╯'};
// Light dashed border
inline constexpr boxOutlineDetails dashed = {U'╌', U'╎', U'┌', U'┐', U'└', U'┘'};
// Solid block border
inline constexpr boxOutlineDetails block = {U'█', U'█', U'█', U'█', U'█', U'█'};
// Standard ASCII fallback
inline constexpr boxOutlineDetails ascii = {U'-', U'|', U'+', U'+', U'+', U'+'};
} // namespace boxStyle

class Box : public Widget {
public:
  boxOutlineDetails outline;

  Box(Rect r, boxOutlineDetails o = boxStyle::light) {
    rect = r;
    outline = o;
  }

  void render(frameBuffer &fb) override {
    if (rect.width <= 0 || rect.height <= 0) {
      return;
    }
    int right = rect.x + rect.width - 1;
    int bottom = rect.y + rect.height - 1;
    fb.setGlyph(rect.x, rect.y, outline.topLeft);
    fb.setGlyph(right, rect.y, outline.bottomLeft);
    fb.setGlyph(rect.x, bottom, outline.topLeft);
    fb.setGlyph(right, bottom, outline.topLeft);

    for (int x = rect.x + 1; x < right; x++) {
      fb.setGlyph(x, rect.y, outline.horizontal);
      fb.setGlyph(x, bottom, outline.horizontal);
    }
    for (int y = rect.y + 1; y < right; y++) {
      fb.setGlyph(rect.x, y, outline.horizontal);
      fb.setGlyph(right, y, outline.horizontal);
    }
  }
};

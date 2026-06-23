#pragma once

#include "container.hpp"
#include "frameBuffer.hpp"

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

class Box : public Widget {
public:
  boxOutlineDetails outline;
  std::unique_ptr<Container> layoutType;

  Box(std::unique_ptr<Container> main,
      std::vector<std::unique_ptr<Widget>> ContainerChildren,
      boxOutlineDetails o = boxStyle::light)
      : outline(o), layoutType(std::move(main)) {
    if (layoutType) {
      layoutType->children = std::move(ContainerChildren);
    }
  }

  void drawBorder(frameBuffer &fb) {
    if (rect.width < 2 || rect.height < 2) {
      return;
    }
    int right = rect.x + rect.width - 1;
    int bottom = rect.y + rect.height - 1;
    fb.setGlyph(rect.x, rect.y, outline.topLeft);
    fb.setGlyph(right, rect.y, outline.topRight);
    fb.setGlyph(rect.x, bottom, outline.bottomLeft);
    fb.setGlyph(right, bottom, outline.bottomRight);

    for (int x = rect.x + 1; x < right; x++) {
      fb.setGlyph(x, rect.y, outline.horizontal);
      fb.setGlyph(x, bottom, outline.horizontal);
    }
    for (int y = rect.y + 1; y < bottom; y++) {
      fb.setGlyph(rect.x, y, outline.vertical);
      fb.setGlyph(right, y, outline.vertical);
    }
  }

  void render(frameBuffer &fb) {
    drawBorder(fb);
    if (layoutType) {
      layoutType->setRect(rect.x + 1, rect.y + 1, rect.height - 2,
                          rect.width - 2);
      layoutType->setRectForChildren();
      for (auto &child : layoutType->children) {
        child->render(fb);
      }
    }
  }
};

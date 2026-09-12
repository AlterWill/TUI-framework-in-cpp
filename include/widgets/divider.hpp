#pragma once

#include "core/widget.hpp"
#include "utilities/Orientation.hpp"
#include "utilities/dividerStyle.hpp"

struct divider : Widget {
  Orientation orientation;
  char32_t dividerGlyph{dividerStyle::horizontal::light};
  ColourPair colours{};

  divider& withOrientation(Orientation o) {
    orientation = o;
    return *this;
  }

  divider& withGlyph(char32_t g) {
    dividerGlyph = g;
    return *this;
  }

  divider& withColours(ColourPair c) {
    colours = c;
    return *this;
  }

  Size measure(const SizeConstraints& constraints) override {
    if (orientation == Orientation::Horizontal) {
      return Size{std::max(static_cast<std::size_t>(1), constraints.getMinHeight()), constraints.getMaxWidth()};
    } else {
      return Size{constraints.getMaxHeight(), std::max(static_cast<std::size_t>(1), constraints.getMinWidth())};
    }
  };

  void render(RenderContext& rendercontext) override {
    std::size_t avaiableWidth = padding.horizontal();
    std::size_t avaiableHeight = padding.vertical();
    if (rendercontext.getRect().getHeight() <= avaiableHeight || rendercontext.getRect().getWidth() <= avaiableWidth)
      return;

    avaiableHeight = rendercontext.getRect().getHeight() - avaiableHeight;
    avaiableWidth = rendercontext.getRect().getWidth() - avaiableWidth;

    Cell cell{dividerGlyph, Style{}};
    cell.setColour(colours);
    if (orientation == Orientation::Horizontal) {
      for (std::size_t i = rendercontext.getRect().getX() + padding.getLeft();
          i < rendercontext.getRect().getX() + avaiableWidth + padding.getLeft();
          i++) {
        rendercontext.setCell(i, rendercontext.getRect().getY() + padding.getTop() + (avaiableHeight / 2), cell);
      }
    } else {
      for (std::size_t i = rendercontext.getRect().getY() + padding.top;
          i < rendercontext.getRect().getY() + avaiableHeight + padding.top;
          i++) {
        rendercontext.setCell(rendercontext.getRect().getX() + padding.getLeft() + (avaiableWidth / 2),i, cell);
      }
    }
  };
};

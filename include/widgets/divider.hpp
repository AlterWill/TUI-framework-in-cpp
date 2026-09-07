#pragma once

#include "core/widget.hpp"
#include "utilities/Orientation.hpp"
#include "utilities/dividerStyle.hpp"

struct divider : Widget {
  WidgetBase base;
  Orientation orientation;
  char32_t dividerGlyph{dividerStyle::horizontal::light};

  Size measure(const SizeConstraints& constraints) override {
    if (orientation == Orientation::Horizontal) {
      return Size{std::max(static_cast<std::size_t>(1), constraints.getMinHeight()), constraints.getMaxWidth()};
    } else {
      return Size{constraints.getMaxHeight(), std::max(static_cast<std::size_t>(1), constraints.getMinWidth())};
    }
  };

  void render(RenderContext& rendercontext) override {
    std::size_t avaiableWidth = base.padding.left + base.padding.right;
    std::size_t avaiableHeight = base.padding.top + base.padding.bottom;
    if (rendercontext.getRect().getHeight() <= avaiableHeight || rendercontext.getRect().getWidth() <= avaiableWidth)
      return;

    avaiableHeight = rendercontext.getRect().getHeight() - avaiableHeight;
    avaiableWidth = rendercontext.getRect().getWidth() - avaiableWidth;

    Cell cell{dividerGlyph, Style{}};
    if (orientation == Orientation::Horizontal) {
      for (std::size_t i = rendercontext.getRect().getX() + base.padding.getLeft();
          i < rendercontext.getRect().getX() + avaiableWidth + base.padding.getLeft();
          i++) {
        rendercontext.setCell(i, rendercontext.getRect().getY() + base.padding.getTop() + (avaiableHeight / 2), cell);
      }
    } else {
      for (std::size_t i = rendercontext.getRect().getY() + base.padding.top;
          i < rendercontext.getRect().getY() + avaiableHeight + base.padding.top;
          i++) {
        rendercontext.setCell(rendercontext.getRect().getX() + base.padding.getLeft() + (avaiableWidth / 2),i, cell);
      }
    }
  };
};

#pragma once

#include <algorithm>
#include <string>

#include "core/multiChildWidget.hpp"
#include "core/singleChildWidget.hpp"
#include "utilities/boxStyle.hpp"
#include "widgets/box.hpp"

struct AdvancedBoxBase {
  std::string title;
  HorizontalAlignment alignment{HorizontalAlignment::Left};
  bool titleTop{true};
  boxOutlineDetails outline{boxStyle::light};
  std::size_t borderSize{1};
  ColourPair colours;
  ConnectionInfo connections{};
};

struct AdvancedBox : public SingleChildWidget {
  AdvancedBoxBase boxBase;

  AdvancedBox() = default;
  explicit AdvancedBox(std::unique_ptr<Widget> childWidget) : SingleChildWidget(std::move(childWidget)) {}
  explicit AdvancedBox(LayoutNode childNode) : SingleChildWidget(std::move(childNode)) {}

  AdvancedBox& withTitle(std::string t, HorizontalAlignment a = HorizontalAlignment::Left, bool top = true) {
    boxBase.title = std::move(t);
    boxBase.alignment = a;
    boxBase.titleTop = top;
    return *this;
  }

  AdvancedBox& withOutline(boxOutlineDetails o) {
    boxBase.outline = o;
    return *this;
  }

  AdvancedBox& withColours(ColourPair c) {
    boxBase.colours = c;
    return *this;
  }

  AdvancedBox& withBorderSize(std::size_t b) {
    boxBase.borderSize = b;
    return *this;
  }

  AdvancedBox& withPadding(Insets p) {
    padding = p;
    return *this;
  }

  AdvancedBox& withChildMargin(Insets m) {
    base.child.margin = m;
    return *this;
  }

  AdvancedBox& withConnections(const ConnectionInfo& conn) {
    boxBase.connections = conn;
    return *this;
  }

  Size measure(const SizeConstraints& constraints) override {
    const auto& child = base.child;

    std::size_t extraWidth = (2 * boxBase.borderSize) + padding.horizontal() +
                             (child.widget ? (child.margin.horizontal()) : 0);
    std::size_t extraHeight = (2 * boxBase.borderSize) + padding.vertical() +
                              (child.widget ? (child.margin.vertical()) : 0);

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
    const auto& child = base.child;

    std::size_t extraW = (2 * boxBase.borderSize) + padding.horizontal() +
                         (child.widget ? (child.margin.horizontal()) : 0);
    std::size_t extraH = (2 * boxBase.borderSize) + padding.vertical() +
                         (child.widget ? (child.margin.vertical()) : 0);

    if (rect.width < extraW || rect.height < extraH) {
      return;
    }

    std::size_t left = rect.x;
    std::size_t top = rect.y;
    std::size_t right = rect.x + rect.width - boxBase.borderSize;
    std::size_t bottom = rect.y + rect.height - boxBase.borderSize;

    const auto& outline = boxBase.outline;
    Cell borderCell;
    borderCell.setColour(boxBase.colours);

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
    borderCell.setGlyph(outline.vertical);
    for (std::size_t y = top + 1; y < bottom; y++) {
      rendercontext.setCell(left, y, borderCell);
      rendercontext.setCell(right, y, borderCell);
    }

    for (const auto& sep : boxBase.connections.separators) {
      const Rect& sr = sep.rect;
      if (sr.width == 0 || sr.height == 0) continue;

      if (sep.isHorizontal) {
        for (std::size_t x = sr.x; x < sr.x + sr.width && x <= right; x++) {
          if (x == left) {
            borderCell.setGlyph(outline.tLeft);
          } else if (x == right) {
            borderCell.setGlyph(outline.tRight);
          } else if (sr.y == top) {
            borderCell.setGlyph(outline.tTop);
          } else if (sr.y + sr.height - 1 == bottom) {
            borderCell.setGlyph(outline.tBottom);
          } else {
            borderCell.setGlyph(outline.cross);
          }
          rendercontext.setCell(x, sr.y, borderCell);
        }
      } else {
        for (std::size_t y = sr.y; y < sr.y + sr.height && y <= bottom; y++) {
          if (y == top) {
            borderCell.setGlyph(outline.tTop);
          } else if (y == bottom) {
            borderCell.setGlyph(outline.tBottom);
          } else if (sr.x == left) {
            borderCell.setGlyph(outline.tLeft);
          } else if (sr.x + sr.width - 1 == right) {
            borderCell.setGlyph(outline.tRight);
          } else {
            borderCell.setGlyph(outline.cross);
          }
          rendercontext.setCell(sr.x, y, borderCell);
        }
      }
    }

    for (const auto& childConn : boxBase.connections.childConnections) {
      drawConnectionsRecursive(rendercontext, childConn, boxBase.outline, boxBase.colours, boxBase.borderSize);
    }

    Cell bgCell;
    bgCell.setBackgroundColour(boxBase.colours.getBackgroundColour());
    for (std::size_t x = left + boxBase.borderSize; x < right; x++) {
      for (std::size_t y = top + boxBase.borderSize; y < bottom; y++) {
        rendercontext.setCell(x, y, bgCell);
      }
    }
  }

  void drawConnectionsRecursive(RenderContext& rendercontext, const ConnectionInfo& conn,
                              const boxOutlineDetails& outline, const ColourPair& colours,
                              std::size_t borderSize) {
    const Rect& rect = rendercontext.getRect();
    std::size_t left = rect.x;
    std::size_t top = rect.y;
    std::size_t right = rect.x + rect.width - borderSize;
    std::size_t bottom = rect.y + rect.height - borderSize;

    Cell borderCell;
    borderCell.setColour(colours);

    for (const auto& sep : conn.separators) {
      const Rect& sr = sep.rect;
      if (sr.width == 0 || sr.height == 0) continue;

      if (sep.isHorizontal) {
        for (std::size_t x = sr.x; x < sr.x + sr.width && x <= right; x++) {
          if (x == left) {
            borderCell.setGlyph(outline.tLeft);
          } else if (x == right) {
            borderCell.setGlyph(outline.tRight);
          } else if (sr.y == top) {
            borderCell.setGlyph(outline.tTop);
          } else if (sr.y + sr.height - 1 == bottom) {
            borderCell.setGlyph(outline.tBottom);
          } else {
            borderCell.setGlyph(outline.cross);
          }
          rendercontext.setCell(x, sr.y, borderCell);
        }
      } else {
        for (std::size_t y = sr.y; y < sr.y + sr.height && y <= bottom; y++) {
          if (y == top) {
            borderCell.setGlyph(outline.tTop);
          } else if (y == bottom) {
            borderCell.setGlyph(outline.tBottom);
          } else if (sr.x == left) {
            borderCell.setGlyph(outline.tLeft);
          } else if (sr.x + sr.width - 1 == right) {
            borderCell.setGlyph(outline.tRight);
          } else {
            borderCell.setGlyph(outline.cross);
          }
          rendercontext.setCell(sr.x, y, borderCell);
        }
      }
    }

    for (const auto& childConn : conn.childConnections) {
      drawConnectionsRecursive(rendercontext, childConn, outline, colours, borderSize);
    }
  }

  void setRectForChild(const Rect& rect) override {
    if (!base.child.widget) return;

    std::size_t extraW = (2 * boxBase.borderSize) + padding.horizontal() +
                         base.child.margin.horizontal();
    std::size_t extraH = (2 * boxBase.borderSize) + padding.vertical() +
                         base.child.margin.vertical();

    base.child.rect = Rect{
        rect.x + boxBase.borderSize + padding.left + base.child.margin.left,
        rect.y + boxBase.borderSize + padding.top + base.child.margin.top,
        (rect.height > extraH) ? (rect.height - extraH) : 0,
        (rect.width > extraW) ? (rect.width - extraW) : 0
    };
  }

  void render(RenderContext& rendercontext) override {
    drawBorder(rendercontext);
    SingleChildWidget::render(rendercontext);
  }
};
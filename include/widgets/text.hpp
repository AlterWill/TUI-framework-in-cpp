#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "core/widget.hpp"
#include "utilities/alignment.hpp"
#include "utilities/splitParagraphs.hpp"

struct TextBase {
  WidgetBase widgetBase;
  std::vector<std::u32string>& lines;
  Style style;
  HorizontalAlignment alignment{HorizontalAlignment::Left};
};

struct Text : public Widget {
  TextBase base;

  explicit Text(std::vector<std::u32string>& l)
      : base{.widgetBase = {}, .lines = l, .style = {}, .alignment = HorizontalAlignment::Left} {}

  Text(std::vector<std::u32string>& l,
       Style s,
       HorizontalAlignment a = HorizontalAlignment::Left)
      : base{.widgetBase = {}, .lines = l, .style = std::move(s), .alignment = a} {}

  // Builder methods
  Text& withStyle(Style s) {
    base.style = std::move(s);
    return *this;
  }

  Text& withColours(ColourPair c) {
    base.style.colours = std::move(c);
    return *this;
  }

  Text& withAlignment(HorizontalAlignment a) {
    base.alignment = a;
    return *this;
  }

  Text& withPadding(Insets p) {
    base.widgetBase.padding = p;
    return *this;
  }

  void layout(const Rect&) override {}

  Size measure(const SizeConstraints& constraints) override {
    const auto& padding = base.widgetBase.padding;
    std::size_t totalPadH = padding.top + padding.bottom;
    std::size_t totalPadW = padding.left + padding.right;

    std::size_t maxLineWidth = 0;
    for (const auto& line : base.lines) {
      maxLineWidth = std::max(maxLineWidth, line.size());
    }
    std::size_t intrinsicWidth = maxLineWidth + totalPadW;

    std::size_t clampedWidth = std::clamp(
        intrinsicWidth,
        constraints.getMinWidth(),
        constraints.getMaxWidth()
    );

    std::size_t availableWidth = clampedWidth > totalPadW ? clampedWidth - totalPadW : 0;
    std::size_t heightNeeded = totalPadH;
    if (availableWidth > 0) {
      for (const auto& line : base.lines) {
        heightNeeded += findHeightForParagraph(line, availableWidth);
      }
    }

    std::size_t clampedHeight = std::clamp(
        heightNeeded,
        constraints.getMinHeight(),
        constraints.getMaxHeight()
    );

    return Size{clampedHeight, clampedWidth};
  }

  void render(RenderContext& rendercontext) override {
    const Rect& rect = rendercontext.getRect();
    const auto& padding = base.widgetBase.padding;
    std::size_t totalPadH = padding.top + padding.bottom;
    std::size_t totalPadW = padding.left + padding.right;

    if (rect.height <= totalPadH || rect.width <= totalPadW) {
      return;
    }

    std::size_t contentWidth = rect.width - totalPadW;
    std::size_t startX = rect.x + padding.left;
    Point writePoint{startX, rect.y + padding.top};
    std::size_t maxY = rect.y + rect.height - padding.bottom;

    for (const auto& line : base.lines) {
      auto sentences = convertStringToParagraph(line, contentWidth);
      for (const auto& sentence : sentences) {
        if (writePoint.y >= maxY) return;

        writePoint.x = startX + alignOffset(contentWidth, sentence.size(), base.alignment);

        for (auto ch : sentence) {
          rendercontext.setCell(writePoint, Cell{ch, base.style});
          writePoint.x++;
        }
        writePoint.y++;
        writePoint.x = startX;
      }
    }
  }
};

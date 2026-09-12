#pragma once

#include <functional>
#include <string>

#include "core/widget.hpp"
#include "utilities/alignment.hpp"
#include "utilities/splitParagraphs.hpp"

struct button : Widget {
  std::u32string text;
  Style style;
  HorizontalAlignment hAlignment;
  VerticalAlignment vAlignment;
  std::function<void()> onClick;

 public:
  button() { setFocusable(); }

  bool handleEvent(const Event& event) override {
    if (std::holds_alternative<MouseEvent>(event)) {
      const MouseEvent& me = std::get<MouseEvent>(event);
      if (me.getAction() == MouseAction::Press && me.getButton() == MouseButton::Left) {
        if (onClick) onClick();
        return true;
      }
    } else if (std::holds_alternative<keyEvent>(event)) {
      const keyEvent& ke = std::get<keyEvent>(event);
      if ((ke.getKey() == '\n' || ke.getKey() == ' ') && isFocused()) {
        if (onClick) onClick();
        return true;
      }
    }
    return false;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t horizontalPadding = padding.horizontal();
    std::size_t verticalPadding = padding.vertical();
    Size result{};

    if (constraints.getMaxWidth() <= horizontalPadding || constraints.getMaxHeight() <= verticalPadding) return result;

    if (text.empty()) {
      result.setHeight(verticalPadding);
      result.setWidth(horizontalPadding);
      return result;
    }

    if (constraints.getMaxWidth() >= horizontalPadding + text.size()) {
      result.setHeight(verticalPadding + 1);
      result.setWidth(text.size() + horizontalPadding);
      return result;
    }

    std::vector<std::u32string> split = convertStringToParagraph(text, constraints.getMaxWidth());
    result.setHeight(verticalPadding + split.size());
    result.setWidth(constraints.getMaxWidth());
    return result;
  }

  void render(RenderContext& rendercontext) override {
    const Rect& rect = rendercontext.getRect();
    std::size_t horizontalPadding = padding.horizontal();
    std::size_t verticalPadding = padding.vertical();

    if (rect.width <= horizontalPadding || rect.height <= verticalPadding) return;

    std::size_t contentWidth = rect.width - horizontalPadding;
    std::size_t contentHeight = rect.height - verticalPadding;

    Cell cell{' ', style};

    for (std::size_t y = 0; y < rect.height; ++y) {
      for (std::size_t x = 0; x < rect.width; ++x) {
        rendercontext.setCell(rect.getX() + x, rect.getY() + y, cell);
      }
    }

    if (text.empty()) return;

    std::vector<std::u32string> lines = convertStringToParagraph(text, contentWidth);
    if (lines.empty()) return;

    std::size_t startY = rect.getY() + padding.top + alignOffset(contentHeight, lines.size(), vAlignment);

    for (std::size_t lineIdx = 0; lineIdx < lines.size() && lineIdx < contentHeight; ++lineIdx) {
      const std::u32string& line = lines[lineIdx];
      std::size_t startX = rect.getX() + padding.left + alignOffset(contentWidth, line.size(), hAlignment);

      for (std::size_t x = 0; x < line.size() && x < contentWidth; ++x) {
        cell.setGlyph(line[x]);
        rendercontext.setCell(startX + x, startY + lineIdx, cell);
      }
    }
  }
};

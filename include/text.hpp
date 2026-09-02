#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "alignment.hpp"
#include "splitParagraphs.hpp"
#include "widget.hpp"

class Text : public Widget {
 public:
  std::vector<std::u32string>& lines;
  Style style;
  HorizontalAlignment alignment{HorizontalAlignment::Left};

  Text(std::vector<std::u32string>& l, Style s = Style(), HorizontalAlignment a = HorizontalAlignment::Left)
      : lines(l), style(std::move(s)), alignment(a) {}

  void setText(const std::u32string& t) { lines = splitStringByChar(t, '\n'); }
  void appendText(const std::u32string& t) {
    auto requestLines = splitStringByChar(t, '\n');
    for (const auto& line : requestLines) {
      lines.push_back(line);
    }
  }
  void addNewLine() { lines.push_back(U""); }

  void setStyle(Style s) { style = std::move(s); }
  void setColourPair(ColourPair c) { style.colours = std::move(c); }

  void setAlignment(HorizontalAlignment a) { alignment = a; }
  HorizontalAlignment getAlignment() const { return alignment; }

  void layout() override {}

  Size intrinsicSize() override {
    Size result{lines.size() + padding.getBottom() + padding.getTop(), 0};
    for (auto line : lines) {
      result.setWidth(std::max(result.getWidth(), line.size()));
    }
    result.setWidth(result.getWidth() + padding.getLeft() + padding.getRight());
    return result;
  }

  std::size_t getHeightForWidth(std::size_t width) {
    std::size_t totalPadH = padding.getTop() + padding.getBottom();
    std::size_t totalPadW = padding.getLeft() + padding.getRight();
    std::size_t availableWidth = width > totalPadW ? width - totalPadW : 0;
    if (availableWidth == 0) {
      return totalPadH;
    }
    std::size_t height = totalPadH;
    for (const auto& line : lines) {
      height += findHeightForParagraph(line, availableWidth);
    }
    return height;
  }

  // max Size -> each line is can be displayed as one line unless there is \n
  // min Size -> longest word as width and height of all the lines as one word
  Size measure(const SizeConstraints& constraints) override {
    Size intrinsic = intrinsicSize();

    std::size_t clampedWidth = std::clamp(
        intrinsic.getWidth(),
        constraints.getMinWidth(),
        constraints.getMaxWidth()
    );
    std::size_t heightNeeded = getHeightForWidth(clampedWidth);
    std::size_t clampedHeight = std::clamp(
        heightNeeded,
        constraints.getMinHeight(),
        constraints.getMaxHeight()
    );

    return Size{clampedHeight, clampedWidth};
  }

  // wrap each line that does not fit
  void render(RenderContext& rendercontext) override {
    const Rect& rect = rendercontext.getRect();
    std::size_t totalPadH = padding.getTop() + padding.getBottom();
    std::size_t totalPadW = padding.getLeft() + padding.getRight();

    if (rect.getHeight() <= totalPadH || rect.getWidth() <= totalPadW) {
      return;
    }

    std::size_t contentWidth = rect.getWidth() - totalPadW;
    std::size_t startX = rect.getX() + padding.getLeft();
    Point writePoint{startX, rect.getY() + padding.getTop()};
    std::size_t maxY = rect.getY() + rect.getHeight() - padding.getBottom();

    for (const auto& line : lines) {
      auto sentences = convertStringToParagraph(line, contentWidth);
      for (const auto& sentence : sentences) {
        if (writePoint.getY() >= maxY) return;

        std::size_t remainingSpace = (contentWidth > sentence.size()) ? (contentWidth - sentence.size()) : 0;
        if (HorizontalAlignment::Center == alignment) {
          writePoint.setX(startX + (remainingSpace / 2));
        } else if (HorizontalAlignment::Right == alignment) {
          writePoint.setX(startX + remainingSpace);
        } else {
          writePoint.setX(startX);
        }

        for (auto ch : sentence) {
          rendercontext.setCell(writePoint, Cell{ch, style});
          writePoint.incrementX();
        }
        writePoint.incrementY();
        writePoint.setX(startX);
      }
    }
  }
};

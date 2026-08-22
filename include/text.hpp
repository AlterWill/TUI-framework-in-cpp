#pragma once

#include <string>
#include <vector>

#include "splitParagraphs.hpp"
#include "widget.hpp"

enum class Alignment {
  left,
  right,
  center,
};

class Text : public Widget {
 public:
  std::vector<std::string>& lines;
  Style style;
  Alignment alignment;

  Text(std::vector<std::string>& l, Style s = Style(), Alignment a = Alignment::left)
      : lines(l), style(std::move(s)), alignment(a) {}

  void setText(std::string& t) { lines = splitStringByChar(t, '\n'); }
  void appendText(const std::string& t) {
    auto Requestlines = splitStringByChar(t, '\n');
    for (auto line : Requestlines) {
      lines.push_back(line);
    }
  }
  void addNewLine() { lines.push_back(""); }

  void setStyle(Style s) { style = std::move(s); }

  void setColourPair(ColourPair c) { style.colours = std::move(c); }

  void layout() override {};

  Size intrinsicSize() override {
    Size result{lines.size(), 0};
    for (auto line : lines) {
      result.setWidth(std::max(result.getWidth(), line.size()));
    }
    return result;
  }

  std::size_t getHeightForWidth(std::size_t width) {
    std::size_t height{};
    for (auto line : lines) {
      height += findHeightForParagraph(line, width);
    }
    return height;
  }

  // max Size -> each line is can be displayed as one line unles there is \n
  // min Size -> longest word as width and height of all the lines as one word
  Size measure(const SizeConstraints& constraints) override {
    Size result{intrinsicSize()};

    // clang-format off
    result.setWidth(
      std::clamp(
        result.getWidth(),
        constraints.getMinWidth(),
        constraints.getMaxWidth()
      )
    );
    result.setHeight(
      std::clamp(
        getHeightForWidth(result.getWidth()),
        constraints.getMinHeight(),
        constraints.getMaxHeight()
      )
    );
    // clang-format on

    return result;
  }

  // wrap each line that does not fit
  void render(RenderContext& rendercontext) override {
    if (rect.getHeight() == 0 || rect.getWidth() == 0) return;
    Point writePoint{rect.getX(), rect.getY()};

    for (auto line : lines) {
      auto sentences = convertStringToParagraph(line, rect.getWidth());
      for (auto sentence : sentences) {
        size_t remainingSpace = std::max(static_cast<size_t>(0), rect.getWidth() - sentence.size());
        if (Alignment::center == alignment) {
          writePoint.setX(writePoint.getX() + (remainingSpace / 2));
        } else if (Alignment::right == alignment) {
          writePoint.setX(writePoint.getX() + remainingSpace);
        }
        for (auto ch : sentence) {
          rendercontext.setCell(writePoint, Cell{ch, style});
          writePoint.incrementX();
        }
        writePoint.incrementY();
        writePoint.setX(rect.getX());
      }
    }
  };
};

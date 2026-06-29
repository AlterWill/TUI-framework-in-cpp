#pragma once

#include <string>
#include <vector>

#include "frameBuffer.hpp"
#include "splitParagraphs.hpp"
#include "widget.hpp"

enum class Alignment {
  left,
  right,
  center,
};

class Text : public Widget {
public:
  std::string text;
  Sytle style;
  Alignment alignment;

  Text(std::string t, Sytle s = Sytle(), Alignment a = Alignment::left)
      : text(t), style(s), alignment(a) {}

  void setText(std::string t) { text = std::move(t); }

  void setStyle(Sytle s) { style = std::move(s); }

  void setAlignment(Alignment a) { alignment = a; }

  void layout() override {};

  void render(frameBuffer &fb) override {
    std::vector<std::string> lines = convertStringToParagraph(text, rect.width);

    for (int i = 0; i < rect.height && i < static_cast<int>(lines.size());
         i++) {
      const std::string &line = lines[i];
      int lineLen = line.length();

      int xOffset = rect.x;
      if (alignment == Alignment::right) {
        xOffset = rect.x + rect.width - lineLen;
      } else if (alignment == Alignment::center) {
        xOffset = rect.x + (rect.width - lineLen) / 2;
      }
      for (int j = 0; j < rect.width && j < lineLen; j++) {
        fb.setGlyph(xOffset + j, rect.y + i, line[j]);
        fb.setStyle(xOffset + j, rect.y + i, style);
      }
    }
  };
};

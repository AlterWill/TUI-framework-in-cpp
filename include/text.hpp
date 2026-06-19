#pragma once

#include "frameBuffer.hpp"
#include "node.hpp"
#include "splitParagraphs.hpp"
#include <string>
#include <vector>

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

  Text(Rect r, std::string t, Sytle s = Sytle(), Alignment a = Alignment::left) {
    rect = r;
    text = t;
    style = s;
    alignment = a;
  }

  void render(frameBuffer &fb) override {
    std::vector<std::string> lines = convertStringToParagraph(text, rect.width);

    for (int i = 0; i < rect.height && i < static_cast<int>(lines.size()); i++) {
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

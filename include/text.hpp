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
  std::string text;
  Sytle style;
  Alignment alignment;

  void render(frameBuffer &fb) {
    int n = text.size();
    switch (alignment) {
    case Alignment::left: {
      if (n > rect.width) {
        std::vector<std::string> lines = convertStringToParagraph(text, rect.width);
        for (int i = 0; i < rect.height; i++) {
          for (int j = 0; j < rect.width; j++) {
            fb.setGlyph(rect.x + j, rect.y + i, lines[i + rect.x][j + rect.y]);
          }
        }
      }
    }
    case Alignment::right: {

      break;
    }
    case Alignment::center: {

      break;
    }
    default: {

      break;
    }
    }
  };
};

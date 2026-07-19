#include "style.hpp"

struct Cell {
  char32_t glyph = U' ';
  Style style;

  void setGlyph(char32_t g){
    glyph = g;
  }
  void setTextStyle(TextStyle t) { style.textStyle |= static_cast<int>(t); }
  void removeTextStyle(TextStyle t) { style.textStyle &= ~static_cast<int>(t); }
  bool hasTextStyle(TextStyle t) { return (style.textStyle & static_cast<int>(t)) != 0; }
  void setColour(NamedColour fg, NamedColour bg) {
    style.colours.fg.setRGBValue(fg);
    style.colours.bg.setRGBValue(bg);
  }
  void setColour(ColourPair c) {
    style.colours.fg.setRGBValue(c.fg.colour);
    style.colours.bg.setRGBValue(c.bg.colour);
  }

  void setForegroundColour(NamedColour fg) { style.colours.fg.setRGBValue(fg); }
  void setBackgroundColour(NamedColour bg) { style.colours.bg.setRGBValue(bg); }
  void setForegroundColour(uint32_t fg) { style.colours.fg.setRGBValue(fg); }
  void setBackgroundColour(uint32_t bg) { style.colours.bg.setRGBValue(bg); }
};

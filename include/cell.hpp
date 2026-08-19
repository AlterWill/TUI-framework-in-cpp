#pragma once

#include "style.hpp"

struct Cell {
  char32_t glyph = U' ';
  Style style;

  // Getters
  char32_t getGlyph() const { return glyph; }
  Style getStyle() const { return style; }

  // Setters
  void setCell(Cell c) {
    glyph = c.glyph;
    style = c.style;
  }
  void setGlyph(char32_t g) { glyph = g; }
  void setStyle(Style s) { style = s; }
  void setTextStyle(TextStyle t) { style.setTextStyle(t); }
  void removeTextStyle(TextStyle t) { style.removeTextStyle(t); }
  bool hasTextStyle(TextStyle t) const { return style.hasTextStyle(t); }
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

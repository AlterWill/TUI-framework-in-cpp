#pragma once

#include "colour.hpp"

struct ColourPair {
  Colour fg{};
  Colour bg{};

  // Getters
  uint32_t getForegroundColour() const { return fg.colour; }
  uint32_t getBackgroundColour() const { return bg.colour; }
  Colour getFg() const { return fg; }
  Colour getBg() const { return bg; }

  // Setters
  void setFg(Colour val) { fg = val; }
  void setBg(Colour val) { bg = val; }
  void setForegroundColour(uint32_t val) { fg.setRGBValue(val); }
  void setBackgroundColour(uint32_t val) { bg.setRGBValue(val); }
};

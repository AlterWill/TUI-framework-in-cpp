
#include "colour.hpp"

struct ColourPair {
  Colour fg{};
  Colour bg{};

  uint32_t getBackgroundColour(){
    return bg.colour;
  }

  uint32_t getForegroundColour(){
    return fg.colour;
  }
};

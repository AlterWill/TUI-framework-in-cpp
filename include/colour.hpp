
#pragma once

#include <cstdint>

#include "named_colour.hpp"

// alpha has 0 mean no colour ( transparent )

class Colour {
 public:
  uint32_t colour{};
  Colour() = default;
  Colour(uint32_t c) : colour(c) {}
  Colour(NamedColour c) : colour(0xFF000000 | static_cast<uint32_t>(c)) {}

  // clang-format off
  uint8_t getAlpha() {      return (colour >> 24) & 0xFF; }
  uint8_t getRedValue() {   return (colour >> 16) & 0xFF; }
  uint8_t getGreenValue() { return (colour >> 8) & 0xFF; }
  uint8_t getBlueValue() {  return colour & 0xFF; }

  void setAlphaValue(uint8_t a) {  colour = (colour & 0x00FFFFFF) | static_cast<uint32_t>(a) << 24; }
  void setRedValue(uint8_t r) {    colour = (colour & 0xFF00FFFF) | static_cast<uint32_t>(r) << 16; }
  void setGreenValue(uint8_t g) {  colour = (colour & 0xFFFF00FF) | static_cast<uint32_t>(g) << 8; }
  void setBlueValue(uint8_t b) {   colour = (colour & 0xFFFFFF00) | static_cast<uint32_t>(b); }
  // clang-format on

  void setRGBValue(uint8_t red, uint8_t green, uint8_t blue) {
    colour = (colour & 0xFF000000) + (static_cast<uint32_t>(red) << 16) + (static_cast<uint32_t>(green) << 8) + blue;
  }

  void setRGBValue(NamedColour c) {
    if (getAlpha() == 0) {
      colour = 0xFF000000 | static_cast<uint32_t>(c);
    } else {
      colour = (colour & 0xFF000000) | static_cast<uint32_t>(c);
    }
  }

  void setRGBValue(uint32_t c) { colour = c; }

  uint32_t getRaw(){ return colour;}
};

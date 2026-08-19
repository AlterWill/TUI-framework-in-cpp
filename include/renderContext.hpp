#pragma once

#include "buffer.hpp"

struct RenderContext {
  Buffer& fb;
  Rect& clip;
  Point offset{};

  // Getters
  std::size_t getXOffset() const { return offset.getX(); }
  std::size_t getYOffset() const { return offset.getY(); }

  // Setters
  void setXOffset(std::size_t val) { offset.setX(val); }
  void setYOffset(std::size_t val) { offset.setY(val); }
};

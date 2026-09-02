#pragma once

#include <cstddef>

struct Size {
  std::size_t height{};
  std::size_t width{};

  Size() = default;
  Size(std::size_t h, std::size_t w) : height(h), width(w) {}

  // Getters
  std::size_t getWidth() const { return width; }
  std::size_t getHeight() const { return height; }

  // Setters
  void setWidth(std::size_t val) { width = val; }
  void setHeight(std::size_t val) { height = val; }
};

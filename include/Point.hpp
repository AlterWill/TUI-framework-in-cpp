#pragma once

#include <cstddef>

struct Point {
  std::size_t x{};
  std::size_t y{};

  Point() = default;
  Point(std::size_t x, std::size_t y) : x(x), y(y) {}

  // Getters
  std::size_t getX() const { return x; }
  std::size_t getY() const { return y; }

  // Setters
  void setX(std::size_t val) { x = val; }
  void setY(std::size_t val) { y = val; }
};

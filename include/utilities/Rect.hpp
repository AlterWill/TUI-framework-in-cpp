#pragma once

#include "utilities/Point.hpp"
#include "layout/Size.hpp"

struct Rect {
  std::size_t x{};
  std::size_t y{};
  std::size_t width{};
  std::size_t height{};

  Rect() = default;
  Rect(std::size_t x, std::size_t y, std::size_t h, std::size_t w)
      : x(x), y(y), width(w), height(h) {}

  // Getters
  std::size_t getX() const { return x; }
  std::size_t getY() const { return y; }
  std::size_t getWidth() const { return width; }
  std::size_t getHeight() const { return height; }

  // Setters
  void setX(std::size_t val) { x = val; }
  void setY(std::size_t val) { y = val; }
  void setWidth(std::size_t val) { width = val; }
  void setHeight(std::size_t val) { height = val; }

  // Composite: get/set as Point and Size
  Point getPoint() const { return Point{x, y}; }
  Size getSize() const { return Size{height, width}; }
  void setPoint(Point p) { x = p.getX(); y = p.getY(); }
  void setSize(Size s) { height = s.getHeight(); width = s.getWidth(); }
};

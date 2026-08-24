#pragma once

#include <cstddef>

struct Insets {
  std::size_t top{};
  std::size_t bottom{};
  std::size_t right{};
  std::size_t left{};

  static constexpr Insets all(std::size_t value) {
    return {value, value, value, value};
  }

  static constexpr Insets symmetric(std::size_t x, std::size_t y) { return {y, y, x, x}; }

  // Getters
  std::size_t getTop() const { return top; }
  std::size_t getBottom() const { return bottom; }
  std::size_t getRight() const { return right; }
  std::size_t getLeft() const { return left; }

  // Setters
  void setTop(std::size_t val) { top = val; }
  void setBottom(std::size_t val) { bottom = val; }
  void setRight(std::size_t val) { right = val; }
  void setLeft(std::size_t val) { left = val; }
};

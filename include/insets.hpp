#pragma once

struct Insets {
  int top{};
  int bottom{};
  int right{};
  int left{};

  static constexpr Insets all(int value) {
    return {value, value, value, value};
  }

  static constexpr Insets symmetric(int x, int y) { return {y, y, x, x}; }

  // Getters
  int getTop() const { return top; }
  int getBottom() const { return bottom; }
  int getRight() const { return right; }
  int getLeft() const { return left; }

  // Setters
  void setTop(int val) { top = val; }
  void setBottom(int val) { bottom = val; }
  void setRight(int val) { right = val; }
  void setLeft(int val) { left = val; }
};

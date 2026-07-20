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
};

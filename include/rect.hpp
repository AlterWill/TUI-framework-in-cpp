#pragma once

#include <optional>

struct Rect {
  std::size_t x{};
  std::size_t y{};
  std::size_t height{};
  std::size_t width{};
};

struct LayoutProperies {
  int flex{};

  std::size_t minWidth{};
  std::size_t minHeight{};

  // size t is unsigned, Therefore no negative values
  // Thus -1 will result in the maximum possible value for std::size_t 
  std::optional<std::size_t> maxWidth = -1;
  std::optional<std::size_t> maxHeight = -1;
};

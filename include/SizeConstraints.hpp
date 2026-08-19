#pragma once

#include "Size.hpp"

struct SizeConstraints {
  Size minSize{0, 0};
  Size maxSize{static_cast<std::size_t>(-1), static_cast<std::size_t>(-1)};

  // Getters
  std::size_t getMaxWidth() const { return maxSize.getWidth(); }
  std::size_t getMaxHeight() const { return maxSize.getHeight(); }
  std::size_t getMinWidth() const { return minSize.getWidth(); }
  std::size_t getMinHeight() const { return minSize.getHeight(); }

  // Setters
  void setMaxWidth(std::size_t val) { maxSize.setWidth(val); }
  void setMaxHeight(std::size_t val) { maxSize.setHeight(val); }
  void setMinWidth(std::size_t val) { minSize.setWidth(val); }
  void setMinHeight(std::size_t val) { minSize.setHeight(val); }

  // Full size getters/setters
  Size getMinSize() const { return minSize; }
  Size getMaxSize() const { return maxSize; }
  void setMinSize(Size s) { minSize = s; }
  void setMaxSize(Size s) { maxSize = s; }
};

struct LayoutProperties {
  int flex{};
  SizeConstraints constraints{};
};

#pragma once

#include <algorithm>
#include <cstddef>

#include "utilities/Orientation.hpp"
#include "utilities/Rect.hpp"

inline double sliderValueFromMouse(
    std::size_t mx, std::size_t my,
    const Rect& rect,
    Orientation orientation,
    double minValue, double maxValue,
    std::size_t padLeft, std::size_t padTop,
    std::size_t padRight, std::size_t padBottom) {

  double range = maxValue - minValue;
  if (range <= 0.0) return minValue;

  if (orientation == Orientation::Horizontal) {
    std::size_t contentW = rect.width > padLeft + padRight ? rect.width - padLeft - padRight : 0;
    if (contentW < 3) return minValue;

    std::size_t startX = rect.x + padLeft;
    double norm = static_cast<double>(mx > startX ? mx - startX : 0) /
                  static_cast<double>(contentW > 1 ? contentW - 1 : 1);
    norm = std::clamp(norm, 0.0, 1.0);
    return minValue + norm * range;
  } else {
    std::size_t contentH = rect.height > padTop + padBottom ? rect.height - padTop - padBottom : 0;
    if (contentH < 3) return minValue;

    std::size_t startY = rect.y + padTop;
    double norm = static_cast<double>(my > startY ? my - startY : 0) /
                  static_cast<double>(contentH > 1 ? contentH - 1 : 1);
    norm = std::clamp(norm, 0.0, 1.0);
    return minValue + norm * range;
  }
}

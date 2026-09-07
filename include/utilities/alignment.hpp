#pragma once

#include <cstddef>

enum class HorizontalAlignment {
  Left,
  Center,
  Right,
};

enum class VerticalAlignment {
  Top,
  Center,
  Bottom,
};

inline constexpr std::size_t alignOffset(std::size_t availableSpace,
                                         std::size_t itemSize,
                                         HorizontalAlignment align) noexcept {
  if (availableSpace <= itemSize) return 0;
  std::size_t extra = availableSpace - itemSize;
  switch (align) {
    case HorizontalAlignment::Center:
      return extra / 2;
    case HorizontalAlignment::Right:
      return extra;
    case HorizontalAlignment::Left:
    default:
      return 0;
  }
}

inline constexpr std::size_t alignOffset(std::size_t availableSpace,
                                         std::size_t itemSize,
                                         VerticalAlignment align) noexcept {
  if (availableSpace <= itemSize) return 0;
  std::size_t extra = availableSpace - itemSize;
  switch (align) {
    case VerticalAlignment::Center:
      return extra / 2;
    case VerticalAlignment::Bottom:
      return extra;
    case VerticalAlignment::Top:
    default:
      return 0;
  }
}

inline constexpr std::size_t alignCoordinate(std::size_t start,
                                             std::size_t availableSpace,
                                             std::size_t itemSize,
                                             std::size_t marginStart,
                                             std::size_t marginEnd,
                                             HorizontalAlignment align) noexcept {
  std::size_t totalItem = itemSize + marginStart + marginEnd;
  return start + marginStart + alignOffset(availableSpace, totalItem, align);
}

inline constexpr std::size_t alignCoordinate(std::size_t start,
                                             std::size_t availableSpace,
                                             std::size_t itemSize,
                                             std::size_t marginStart,
                                             std::size_t marginEnd,
                                             VerticalAlignment align) noexcept {
  std::size_t totalItem = itemSize + marginStart + marginEnd;
  return start + marginStart + alignOffset(availableSpace, totalItem, align);
}

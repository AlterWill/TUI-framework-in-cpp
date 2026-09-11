#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <span>
#include <vector>

#include "styling/colour.hpp"

// Linearly interpolate between two colours at position t (0 -> start, 1 -> end).
inline Colour lerpColour(const Colour& start, const Colour& end, float t) {
  t = std::clamp(t, 0.0f, 1.0f);

  const auto lerpChannel = [t](uint8_t u, uint8_t v) -> uint8_t {
    return static_cast<uint8_t>(
        static_cast<float>(u) +
        (static_cast<float>(v) - static_cast<float>(u)) * t);
  };

  Colour result;
  result.setRGBValue(
      lerpChannel(start.getRedValue(), end.getRedValue()),
      lerpChannel(start.getGreenValue(), end.getGreenValue()),
      lerpChannel(start.getBlueValue(), end.getBlueValue()));
  result.setAlphaValue(
      lerpChannel(start.getAlpha(), end.getAlpha()));

  return result;
}

// Linearly interpolate between two colours given a percentage (0..100).
inline Colour lerpColourPercent(const Colour& start, const Colour& end,
                                float pct) {
  return lerpColour(start, end, pct * 0.01f);
}

// Return `steps` colours forming a smooth gradient from `start` to `end` (inclusive).
inline std::vector<Colour> generateGradient(const Colour& start,
                                            const Colour& end,
                                            std::size_t steps) {
  std::vector<Colour> gradient;
  if (steps == 0) return gradient;

  gradient.reserve(steps);
  for (std::size_t i = 0; i < steps; ++i) {
    const float t = (steps == 1)
                        ? 0.0f
                        : static_cast<float>(i) / static_cast<float>(steps - 1);
    gradient.push_back(lerpColour(start, end, t));
  }
  return gradient;
}

// Return `steps` colours spanning every stop in order (multi-stop gradient).
inline std::vector<Colour> generateGradient(const std::span<const Colour> stops,
                                            std::size_t steps) {
  std::vector<Colour> gradient;
  if (stops.empty() || steps == 0) return gradient;

  const std::size_t segments = stops.size() - 1;
  if (segments == 0 || steps == 1) {
    return std::vector<Colour>(steps, stops.front());
  }

  gradient.reserve(steps);
  for (std::size_t i = 0; i < steps; ++i) {
    const float position =
        static_cast<float>(i) * static_cast<float>(segments) /
        static_cast<float>(steps - 1);
    const std::size_t segment =
        std::min(static_cast<std::size_t>(position), segments - 1);
    const float t = position - static_cast<float>(segment);
    gradient.push_back(lerpColour(stops[segment], stops[segment + 1], t));
  }
  return gradient;
}

// Convenience overload allowing brace lists: generateGradient({red, yellow, blue}, 50);
inline std::vector<Colour> generateGradient(
    const std::initializer_list<Colour> stops, std::size_t steps) {
  return generateGradient(
      std::span<const Colour>(stops.begin(), stops.end()), steps);
}

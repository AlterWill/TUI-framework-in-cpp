#pragma once

#include <algorithm>

#include "styling/style.hpp"
#include "utilities/sliderStyle.hpp"

struct slider {
  double value{0.5};
  double minValue{0.0};
  double maxValue{1.0};
  sliderStyleData style{sliderStyle::block};
  Style thumbStyle{};
  Style trackStyle{};

  slider() = default;

  explicit slider(double initialValue) : value(initialValue) {}

  slider& withValue(double v) {
    value = std::clamp(v, minValue, maxValue);
    return *this;
  }

  slider& withRange(double min, double max) {
    minValue = min;
    maxValue = max;
    value = std::clamp(value, minValue, maxValue);
    return *this;
  }

  slider& withStyle(sliderStyleData s) {
    style = s;
    return *this;
  }

  slider& withThumbStyle(Style s) {
    thumbStyle = std::move(s);
    return *this;
  }

  slider& withTrackStyle(Style s) {
    trackStyle = std::move(s);
    return *this;
  }

  slider& withThumbColours(ColourPair c) {
    thumbStyle.colours = c;
    return *this;
  }

  slider& withTrackColours(ColourPair c) {
    trackStyle.colours = c;
    return *this;
  }

  double getValue() const { return value; }

  double normalizedValue() const {
    double range = maxValue - minValue;
    return range > 0.0 ? (value - minValue) / range : 0.0;
  }
};

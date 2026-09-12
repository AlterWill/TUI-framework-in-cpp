#pragma once

#include <algorithm>
#include <cstddef>

#include "core/widget.hpp"
#include "rendering/renderContext.hpp"
#include "styling/style.hpp"
#include "utilities/Orientation.hpp"
#include "utilities/sliderStyle.hpp"

struct slider : public Widget {
  WidgetBase base;
  Orientation orientation{Orientation::Horizontal};
  double value{0.5};
  double minValue{0.0};
  double maxValue{1.0};
  sliderStyleData sliderStyle{sliderStyle::block};
  Style thumbStyle{};
  Style trackStyle{};

  slider() = default;

  explicit slider(double initialValue) : value(initialValue) {}

  slider& withOrientation(Orientation o) {
    orientation = o;
    return *this;
  }

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
    sliderStyle = s;
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

  slider& withPadding(Insets p) {
    base.padding = p;
    return *this;
  }

  double getValue() const { return value; }

  double normalizedValue() const {
    double range = maxValue - minValue;
    return range > 0.0 ? (value - minValue) / range : 0.0;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t padW = base.padding.left + base.padding.right;
    std::size_t padH = base.padding.top + base.padding.bottom;

    std::size_t w, h;
    if (orientation == Orientation::Horizontal) {
      w = padW + 3;
      h = padH + 1;
    } else {
      w = padW + 1;
      h = padH + 3;
    }

    return Size{
        std::clamp(h, constraints.getMinHeight(), constraints.getMaxHeight()),
        std::clamp(w, constraints.getMinWidth(), constraints.getMaxWidth())};
  }

  void render(RenderContext& rendercontext) override {
    const Rect& rect = rendercontext.getRect();
    std::size_t padL = base.padding.left;
    std::size_t padR = base.padding.right;
    std::size_t padT = base.padding.top;
    std::size_t padB = base.padding.bottom;

    if (rect.width <= padL + padR || rect.height <= padT + padB) return;

    std::size_t contentW = rect.width - padL - padR;
    std::size_t contentH = rect.height - padT - padB;

    std::size_t startX = rect.x + padL;
    std::size_t startY = rect.y + padT;

    double norm = normalizedValue();

    if (orientation == Orientation::Horizontal) {
      if (contentW < 3 || contentH < 1) return;

      std::size_t trackLen = contentW;
      std::size_t thumbPos = static_cast<std::size_t>(norm * static_cast<double>(trackLen - 1));
      std::size_t thumbY = startY + contentH / 2;

      for (std::size_t i = 0; i < trackLen; i++) {
        Cell cell{sliderStyle.track, trackStyle};
        rendercontext.setCell(startX + i, thumbY, cell);
      }

      Cell thumb{sliderStyle.thumb, thumbStyle};
      rendercontext.setCell(startX + thumbPos, thumbY, thumb);
    } else {
      if (contentH < 3 || contentW < 1) return;

      std::size_t trackLen = contentH;
      std::size_t thumbPos = static_cast<std::size_t>(norm * static_cast<double>(trackLen - 1));
      std::size_t thumbX = startX + contentW / 2;

      for (std::size_t i = 0; i < trackLen; i++) {
        Cell cell{sliderStyle.track, trackStyle};
        rendercontext.setCell(thumbX, startY + i, cell);
      }

      Cell thumb{sliderStyle.thumb, thumbStyle};
      rendercontext.setCell(thumbX, startY + thumbPos, thumb);
    }
  }
};

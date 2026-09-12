#pragma once

#include <cstddef>

#include "core/widget.hpp"
#include "rendering/renderContext.hpp"
#include "utilities/Orientation.hpp"
#include "widgets/slider.hpp"

struct interactiveSlider : public Widget {
  WidgetBase base;
  slider data;
  Orientation orientation{Orientation::Horizontal};

  interactiveSlider() = default;

  explicit interactiveSlider(double initialValue) : data(initialValue) {}

  interactiveSlider& withOrientation(Orientation o) {
    orientation = o;
    return *this;
  }

  interactiveSlider& withValue(double v) {
    data.withValue(v);
    return *this;
  }

  interactiveSlider& withRange(double min, double max) {
    data.withRange(min, max);
    return *this;
  }

  interactiveSlider& withStyle(sliderStyleData s) {
    data.withStyle(s);
    return *this;
  }

  interactiveSlider& withThumbColours(ColourPair c) {
    data.withThumbColours(c);
    return *this;
  }

  interactiveSlider& withTrackColours(ColourPair c) {
    data.withTrackColours(c);
    return *this;
  }

  interactiveSlider& withPadding(Insets p) {
    base.padding = p;
    return *this;
  }

  double getValue() const { return data.getValue(); }

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

    double norm = data.normalizedValue();

    if (orientation == Orientation::Horizontal) {
      if (contentW < 3 || contentH < 1) return;

      std::size_t trackLen = contentW;
      std::size_t thumbPos = static_cast<std::size_t>(norm * static_cast<double>(trackLen - 1));
      std::size_t thumbY = startY + contentH / 2;

      for (std::size_t i = 0; i < trackLen; i++) {
        Cell cell{data.style.track, data.trackStyle};
        rendercontext.setCell(startX + i, thumbY, cell);
      }

      Cell thumb{data.style.thumb, data.thumbStyle};
      rendercontext.setCell(startX + thumbPos, thumbY, thumb);
    } else {
      if (contentH < 3 || contentW < 1) return;

      std::size_t trackLen = contentH;
      std::size_t thumbPos = static_cast<std::size_t>(norm * static_cast<double>(trackLen - 1));
      std::size_t thumbX = startX + contentW / 2;

      for (std::size_t i = 0; i < trackLen; i++) {
        Cell cell{data.style.track, data.trackStyle};
        rendercontext.setCell(thumbX, startY + i, cell);
      }

      Cell thumb{data.style.thumb, data.thumbStyle};
      rendercontext.setCell(thumbX, startY + thumbPos, thumb);
    }
  }
};

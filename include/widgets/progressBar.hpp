#pragma once

#include "core/widget.hpp"
#include "utilities/gradientColorHelper.hpp"
#include "utilities/progressBarStyles.hpp"

struct progressBar : Widget {
  float& progress;
  progressBarData progressBarStyle;
  std::vector<Colour> colours;
  Colour background;

  progressBar(float& p, progressBarData pbd = progressBarStyle::block) : progress(p), progressBarStyle(pbd) {}

  Size measure(const SizeConstraints& constraints) override {
    std::size_t height = padding.vertical() + 1;
    return Size{std::clamp(height, constraints.getMinHeight(), constraints.getMaxHeight()), constraints.getMaxWidth()};
  };

  bool handleEvent(const Event&) override { return false; }

  void render(RenderContext& rendercontext) override {
    std::size_t height = padding.vertical();
    std::size_t width = padding.horizontal();

    height = (rendercontext.getRect().getHeight() <= height) ? 0 : rendercontext.getRect().getHeight() - height;
    width = (rendercontext.getRect().getWidth() <= width) ? 0 : rendercontext.getRect().getWidth() - width;

    if (height == 0 || width == 0) return;
    std::size_t pointerX = rendercontext.getRect().getX();
    std::size_t pointerY = rendercontext.getRect().getY();

    float percentage = std::clamp(progress, 0.0f, 100.0f);
    std::size_t head = static_cast<std::size_t>(static_cast<float>(width) * (percentage / 100.0f));

    std::vector<Colour> gradientColours = generateGradient(colours,width);

    for (std::size_t i = 0; i < width; i++) {
      for (std::size_t j = pointerY; j < pointerY + height; j++) {
        if (i < head) {
          rendercontext.setCell(pointerX + i, j, Cell{progressBarStyle.filled, Style{ColourPair{gradientColours[i],background},0}});
        } else if (i == head) {
          rendercontext.setCell(pointerX + i, j, Cell{progressBarStyle.head, Style{ColourPair{gradientColours[i],background},0}});
        } else {
          rendercontext.setCell(pointerX + i, j, Cell{progressBarStyle.empty, Style{ColourPair{background,background},0}});
        }
      }
    }
  };
};

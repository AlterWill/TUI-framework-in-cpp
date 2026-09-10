#pragma once

#include <algorithm>

#include "utilities/singleSpinnerStyle.hpp"
#include "widgets/animatebleWidget.hpp"

struct singleCharSpinner : animatebleWidget {
  WidgetBase base;
  singleSpinnerData spinnerStyle;
  std::size_t frameIndex{};
  std::chrono::steady_clock::time_point lastUpdate{};

  singleCharSpinner(singleSpinnerData data = singleSpinnerStyle::blocks)
      : base{}, spinnerStyle(data), lastUpdate(std::chrono::steady_clock::now()) {}

  void nextFrame(std::size_t i) override {
    if (spinnerStyle.frames.empty()) return;

    frameIndex = (frameIndex + i) % spinnerStyle.frames.size();
  }

  std::chrono::milliseconds animationInterval() const override { return spinnerStyle.interval; };

  void update(std::chrono::steady_clock::time_point now) override {
    auto elapsed = now - lastUpdate;
    auto frames = elapsed / animationInterval();

    if (frames > 0) {
      nextFrame(static_cast<std::size_t>(frames));
      lastUpdate += animationInterval() * frames;
    }
  }

  char32_t getFrame() const {
    if (spinnerStyle.frames.empty()) return U' ';
    return spinnerStyle.frames[frameIndex];
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t height = base.padding.getTop() + base.padding.getBottom() + 1;
    std::size_t width = base.padding.getLeft() + base.padding.getRight() + 1;
    return Size{std::clamp(height, constraints.getMinHeight(), constraints.getMaxHeight()),
                std::clamp(width, constraints.getMinWidth(), constraints.getMaxWidth())};
  }

  void render(RenderContext& rendercontext) override {
    if (rendercontext.getRect().getHeight() <= base.padding.getTop() + base.padding.getBottom() ||
        rendercontext.getRect().getWidth() <= base.padding.getLeft() + base.padding.getRight()) {
      return;
    }

    rendercontext.setCell(base.padding.getLeft(), base.padding.getTop(), Cell{getFrame(), Style{}});
  };

  void layout(const Rect&) override {}
  bool handleEvent(const Event&) override { return false; }
};

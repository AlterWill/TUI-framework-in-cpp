#pragma once

#include <algorithm>

#include "styling/style.hpp"
#include "utilities/singleSpinnerStyle.hpp"
#include "widgets/animatebleWidget.hpp"

struct singleCharSpinner : animatebleWidget {
  singleSpinnerData spinnerStyle;
  std::size_t frameIndex{};
  std::chrono::steady_clock::time_point lastUpdate{};
  Style style{};
  bool visible{true};

  singleCharSpinner& withVisibility(bool v) { visible = v; return *this; }

  singleCharSpinner(singleSpinnerData data = singleSpinnerStyle::braille)
      : spinnerStyle(data), lastUpdate(std::chrono::steady_clock::now()) {}

  singleCharSpinner& withStyle(Style s) {
    style = std::move(s);
    return *this;
  }
  singleCharSpinner& withColours(ColourPair c) {
    style.colours = std::move(c);
    return *this;
  }
  singleCharSpinner& withPadding(Insets p) {
    padding = p;
    return *this;
  }

  void nextFrame(std::size_t i) {
    if (spinnerStyle.frames.empty()) return;

    frameIndex = (frameIndex + i) % spinnerStyle.frames.size();
  }

  void reset() {
    frameIndex = 0;
    lastUpdate = std::chrono::steady_clock::now();
  }

  std::chrono::milliseconds animationInterval() const { return spinnerStyle.interval; };

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
    if(!visible) return Size{0,0};
    std::size_t height = padding.vertical() + 1;
    std::size_t width = padding.horizontal() + 1;
    return Size{std::clamp(height, constraints.getMinHeight(), constraints.getMaxHeight()),
        std::clamp(width, constraints.getMinWidth(), constraints.getMaxWidth())};
  }

  void render(RenderContext& rendercontext) override {
    if(!visible) return;
    if (rendercontext.getRect().getHeight() <= padding.vertical() ||
        rendercontext.getRect().getWidth() <= padding.horizontal()) {
      return;
    }

    rendercontext.setCell(padding.getLeft(), padding.getTop(), Cell{getFrame(), style});
  };

  void layout(const Rect&) override {}
  bool handleEvent(const Event&) override { return false; }
};

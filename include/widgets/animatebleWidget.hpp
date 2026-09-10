#pragma once

#include <chrono>
#include "core/widget.hpp"

struct animatebleWidget : Widget {
  virtual ~animatebleWidget() = default;

  virtual void nextFrame(std::size_t i) = 0;
  virtual std::chrono::milliseconds animationInterval() const = 0;
  virtual void update(std::chrono::steady_clock::time_point now) = 0;
};

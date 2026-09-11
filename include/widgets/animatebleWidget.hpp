#pragma once

#include <chrono>
#include "core/widget.hpp"

struct animatebleWidget : Widget {
  virtual ~animatebleWidget() = default;

  virtual void update(std::chrono::steady_clock::time_point now) = 0;
};

#pragma once

#include <vector>

#include "widgets/animatebleWidget.hpp"
#include "animation/animationEntry.hpp"

struct AnimationManager {
  std::vector<AnimationEntry> animations;

  AnimationManager() = default;

 public:
  void add(animatebleWidget* widget) { animations.push_back(AnimationEntry{widget}); }
  void remove(animatebleWidget* widget) {
    for (auto it = animations.begin(); it < animations.end(); it++) {
      if (it->widget == widget) {
        animations.erase(it);
        break;
      }
    }
  }

  void update(std::chrono::steady_clock::time_point now) {
    for (auto it = animations.begin(); it < animations.end(); it++) {
      it->widget->update(now);
    }
  }
};

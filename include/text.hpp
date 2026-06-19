#pragma once

#include "frameBuffer.hpp"
#include "node.hpp"

enum class alignment {
  left,
  right,
  center,
};

class Text : Widget {
  std::string text;
  Sytle style;

  void render(frameBuffer &fb) {}
};

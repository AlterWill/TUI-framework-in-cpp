#pragma once

#include "core/widget.hpp"

struct Spacer : Widget {
  Size measure(const SizeConstraints& constraints) override {
    return Size{
    constraints.getMinHeight(),
    constraints.getMinWidth()
    };
  };
};

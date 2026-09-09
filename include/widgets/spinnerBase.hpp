#pragma once

#include "core/widget.hpp"

struct SpinnerBase : Widget{
    virtual ~SpinnerBase() = default;

    virtual void nextFrame() = 0;
};

#pragma once

#include <memory>

#include "utilities/Rect.hpp"
#include "utilities/alignment.hpp"
#include "layout/sizeType.hpp"
#include "core/widget.hpp"

struct LayoutNode {
  bool dirty{false};

  std::unique_ptr<Widget> widget;

  SizeSpec width;
  SizeSpec height;

  SizeConstraints constraints;

  Size measured{};
  Rect rect{};

  Insets margin{};

  HorizontalAlignment horizontalAlignment{};
  VerticalAlignment verticalAlignment{};
};

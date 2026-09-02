#pragma once

#include <memory>

#include "Rect.hpp"
#include "alignment.hpp"
#include "sizeType.hpp"
#include "widget.hpp"

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

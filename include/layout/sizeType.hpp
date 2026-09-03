#pragma once

#include <cstddef>

enum class SizeType {
  Content,
  Fixed,
  Percentage,
  Flex,
  AspectRatio
};

struct SizeSpec {
  SizeType type{SizeType::Content};
  std::size_t value{};
};

/*
SIZE TYPES
==========

Content
-------
Size is determined by the widget's content.

Fixed
-----
Size is exactly the specified value.

If the requested size cannot be allocated by the parent,
the widget receives no space and does not render.

Percentage
----------
Size is the specified percentage of the parent's available
space in that dimension.

Valid range:
    0 .. 100

The sum of percentage allocations for siblings in the
relevant dimension must not exceed 100%.

Flex
----
Size is determined from the remaining space after the
other children have been allocated.

allocation =
    remaining_space * flex / total_flex


ASPECT RATIO
============

Aspect ratio is not a SizeType.

It defines a relationship:

    height : width

One dimension must determine the size and the other dimension
is derived from the ratio.

Example:

    width  = Fixed(160)
    ratio  = 9:16

    height = 90


LAYOUT ORDER
============

1. Allocate Fixed children.
2. Allocate Percentage children.
3. Measure and allocate Content children.
4. Resolve aspect-ratio dimensions.
5. Calculate remaining space.
6. Distribute remaining space among Flex children.
7. Distribute any unused space according to container
   justification/alignment.
8. Produce child Rects.


OVERFLOW
========

Fixed:
    Cannot fit -> child does not render.

Percentage:
    Individual value must be 0..100.
    Sum of percentage allocations must not exceed 100%.

Content:
    Content allocations must fit within the available space.

Flex:
    Receives remaining space.
    If no space remains, Flex children receive no space.

Widgets must never overlap because the layout calculation
must not allocate overlapping rectangles.


REMAINING SPACE
===============

Remaining space is handled by the multi-child container.

Examples:

    Start
    Center
    End
    SpaceBetween
    SpaceEvenly

This is separate from SizeSpec.
*/

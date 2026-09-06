#pragma once

#include <cstddef>
#include <optional>

enum class SizeType {
  Content,
  Fixed,
  Percentage,
  Flex,
};

struct SizeSpec {
  SizeType type{SizeType::Content};
  std::size_t value{};

  std::optional<float> aspectRatio;

};

/*

watch video on clay algorithm?

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

1. Fixed
2. Percentage
3. Measure ALL Content children
4. Sum Content preferred sizes
5. If they fit:
       allocate preferred sizes
   Else:
       shrink Content children toward their minimums
6. Calculate remaining space
7. Give remaining space to Flex
8. Apply justification/alignment


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


Measure
=================

- Fixed gets that value 
- percentage gets the percentage of remaining constraints after fixed
- Content gets there preferred size with the remaining constraints
- Flex just gets the remaining constraints
*/

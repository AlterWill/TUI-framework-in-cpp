#pragma once

struct boxOutlineDetails {
  char32_t horizontal;
  char32_t vertical;
  char32_t topLeft;
  char32_t topRight;
  char32_t bottomLeft;
  char32_t bottomRight;
  char32_t tLeft;
  char32_t tRight;
  char32_t tTop;
  char32_t tBottom;
  char32_t cross;
};

// clang-format off
namespace boxStyle {
inline constexpr boxOutlineDetails light        = {U'─', U'│', U'┌', U'┐', U'└', U'┘', U'├', U'┤', U'┬', U'┴', U'┼'};
inline constexpr boxOutlineDetails heavy        = {U'━', U'┃', U'┏', U'┓', U'┗', U'┛', U'┣', U'┫', U'┳', U'┻', U'╋'};
inline constexpr boxOutlineDetails doubleBorder = {U'═', U'║', U'╔', U'╗', U'╚', U'╝', U'╠', U'╣', U'╦', U'╩', U'╬'};
inline constexpr boxOutlineDetails rounded      = {U'─', U'│', U'╭', U'╮', U'╰', U'╯', U'├', U'┤', U'┬', U'┴', U'┼'};
inline constexpr boxOutlineDetails dashed       = {U'╌', U'╎', U'┌', U'┐', U'└', U'┘', U'├', U'┤', U'┬', U'┴', U'┼'};
inline constexpr boxOutlineDetails block        = {U'█', U'█', U'█', U'█', U'█', U'█', U'█', U'█', U'█', U'█', U'█'};
inline constexpr boxOutlineDetails ascii        = {U'-', U'|', U'+', U'+', U'+', U'+', U'+', U'+', U'+', U'+', U'+'};
}
// clang-format on
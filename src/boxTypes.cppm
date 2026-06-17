module;

export module boxTypes;

export struct Cell {
  char32_t glyph;
};

export struct boxOutlineDetails {
  char32_t horizontal;
  char32_t vertical;
  char32_t topLeft;
  char32_t topRight;
  char32_t bottomLeft;
  char32_t bottomRight;
};

export struct bounds {
  int x;
  int y;
  int height;
  int width;
};

export namespace boxStyle {
inline constexpr boxOutlineDetails light = {U'─', U'│', U'┌', U'┐', U'└', U'┘'};

// Heavy / Thick line border
inline constexpr boxOutlineDetails heavy = {U'━', U'┃', U'┏', U'┓', U'┗', U'┛'};

// Double line border
inline constexpr boxOutlineDetails doubleBorder = {U'═', U'║', U'╔',
                                                   U'╗', U'╚', U'╝'};

// Light line border with rounded corners
inline constexpr boxOutlineDetails rounded = {U'─', U'│', U'╭',
                                              U'╮', U'╰', U'╯'};

// Light dashed border
inline constexpr boxOutlineDetails dashed = {U'╌', U'╎', U'┌',
                                             U'┐', U'└', U'┘'};

// Solid block border
inline constexpr boxOutlineDetails block = {U'█', U'█', U'█', U'█', U'█', U'█'};

// Standard ASCII fallback
inline constexpr boxOutlineDetails ascii = {U'-', U'|', U'+', U'+', U'+', U'+'};
} // namespace boxStyle

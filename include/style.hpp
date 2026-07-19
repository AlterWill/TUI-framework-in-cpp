
#include "colourPair.hpp"

// clang-format off
enum class TextStyle : uint8_t {
  None          = 0,
  Bold          = 1 << 0,
  Dim           = 1 << 1,
  Italic        = 1 << 2,
  Underline     = 1 << 3,
  Blink         = 1 << 4,
  Reverse       = 1 << 5,
  Hidden        = 1 << 6,
  StrikeThrough = 1 << 7,
};
// clang-format on

struct Style {
  ColourPair colours{};
  uint8_t textStyle = static_cast<uint8_t>(TextStyle::None);
  void setTextStyle(TextStyle t) { textStyle |= static_cast<int>(t); }
  void removeTextStyle(TextStyle t) { textStyle &= ~static_cast<int>(t); }
  bool hasTextStyle(TextStyle t) { return (textStyle & static_cast<int>(t)) != 0; }
};

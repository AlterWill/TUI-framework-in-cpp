#pragma once

struct sliderStyleData {
  char32_t thumb{};
  char32_t track{};
};

namespace sliderStyle {

inline constexpr sliderStyleData block{
    U'\u2588',
    U'\u2500',
};

inline constexpr sliderStyleData heavy{
    U'\u2588',
    U'\u2501',
};

inline constexpr sliderStyleData light{
    U'O',
    U'\u2500',
};

inline constexpr sliderStyleData dots{
    U'\u25CF',
    U'\u2500',
};

inline constexpr sliderStyleData arrows{
    U'\u25B6',
    U'\u2500',
};

inline constexpr sliderStyleData braces{
    U'\u2588',
    U'\u2502',
};

inline constexpr sliderStyleData hash{
    U'#',
    U'-',
};

inline constexpr sliderStyleData pipe{
    U'\u2588',
    U'|',
};

inline constexpr sliderStyleData none{
    U'\u2588',
    U' ',
};

}  // namespace sliderStyle

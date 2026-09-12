#pragma once

struct checkboxData {
  char32_t checked{};
  char32_t unchecked{};
  char32_t checkedFocused{};
  char32_t uncheckedFocused{};
};

namespace checkboxStyle {

// ─────────────────────────────────────────────
// X / space (classic): [X] / [ ]
// ─────────────────────────────────────────────

inline constexpr checkboxData x{
    U'X',
    U' ',
    U'X',
    U' '
};

// ─────────────────────────────────────────────
// Light checkmark: [✓] / [ ]
// ─────────────────────────────────────────────

inline constexpr checkboxData check{
    U'✓',
    U' ',
    U'✗',
    U' '
};

// ─────────────────────────────────────────────
// Heavy checkmark: [✔] / [ ]
// ─────────────────────────────────────────────

inline constexpr checkboxData heavyCheck{
    U'✔',
    U' ',
    U'✘',
    U' '
};

// ─────────────────────────────────────────────
// Block: [█] / [ ]  (focus uses lighter shade)
// ─────────────────────────────────────────────

inline constexpr checkboxData block{
    U'█',
    U' ',
    U'▓',
    U'░'
};

// ─────────────────────────────────────────────
// Square: [■] / [□]
// ─────────────────────────────────────────────

inline constexpr checkboxData square{
    U'■',
    U'□',
    U'▪',
    U'▫'
};

// ─────────────────────────────────────────────
// Braille: [⣿] / [⣀]
// ─────────────────────────────────────────────

inline constexpr checkboxData braille{
    U'⣿',
    U'⣀',
    U'⣿',
    U'⠿'
};

// ─────────────────────────────────────────────
// Circle: [●] / [○]
// ─────────────────────────────────────────────

inline constexpr checkboxData circle{
    U'●',
    U'○',
    U'◉',
    U'◌'
};

}  // namespace checkboxStyle
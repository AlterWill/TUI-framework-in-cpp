#pragma once

#include <span>

struct progressBarData {
  char32_t filled{};
  char32_t empty{};
  char32_t head{};
};

// Shades are ordered from light (lowest fill) to dark (full).
struct progressBarGradientData {
  std::span<const char32_t> shades;
  char32_t empty{};
  char32_t head{};
};

namespace progressBarStyle {

// ─────────────────────────────────────────────
// Block (default)
// ─────────────────────────────────────────────

inline constexpr progressBarData block{
    U'█',
    U'░',
    U'▌'
};

// ─────────────────────────────────────────────
// Block shaded
// ─────────────────────────────────────────────

inline constexpr progressBarData shaded{
    U'█',
    U'▓',
    U'▒'
};

// ─────────────────────────────────────────────
// Braille dots
// ─────────────────────────────────────────────

inline constexpr progressBarData braille{
    U'⣿',
    U'⣀',
    U'⡇'
};

// ─────────────────────────────────────────────
// Circle
// ─────────────────────────────────────────────

inline constexpr progressBarData circle{
    U'●',
    U'○',
    U'⦿'
};

// ─────────────────────────────────────────────
// Arrow
// ─────────────────────────────────────────────

inline constexpr progressBarData arrow{
    U'▶',
    U'▷',
    U'▶'
};

// ─────────────────────────────────────────────
// Heavy line
// ─────────────────────────────────────────────

inline constexpr progressBarData heavy{
    U'━',
    U'─',
    U'►'
};

// ─────────────────────────────────────────────
// Dot
// ─────────────────────────────────────────────

inline constexpr progressBarData dot{
    U'•',
    U'·',
    U'◉'
};

// ─────────────────────────────────────────────
// Hash
// ─────────────────────────────────────────────

inline constexpr progressBarData hash{
    U'#',
    U'-',
    U'>'
};

// ─────────────────────────────────────────────
// Equal
// ─────────────────────────────────────────────

inline constexpr progressBarData equal{
    U'=',
    U' ',
    U'>'
};

// ─────────────────────────────────────────────
// Ramp (fill with head)
// ====>░░░░
// ─────────────────────────────────────────────

inline constexpr progressBarData ramp{
    U'=',
    U' ',
    U'>'
};

// ─────────────────────────────────────────────
// Play (fill with head)
// ────▶░░░░
// ─────────────────────────────────────────────

inline constexpr progressBarData play{
    U'─',
    U' ',
    U'▶'
};

// ─────────────────────────────────────────────
// Tracker (fill with head)
// ▬▬▬▬►░░░░
// ─────────────────────────────────────────────

inline constexpr progressBarData tracker{
    U'▬',
    U' ',
    U'►'
};

// ─────────────────────────────────────────────
// Layer (fill with head)
// █████▏░░░
// ─────────────────────────────────────────────

inline constexpr progressBarData layer{
    U'█',
    U'░',
    U'▏'
};

// ─────────────────────────────────────────────
// Gradient shades
// ░▒▓█ / ░░░▓█ / ░░▒▓█ ... smooth 4-level fill
// ─────────────────────────────────────────────

inline constexpr char32_t shadeLevels[] = {
    U'░', U'▒', U'▓', U'█'
};

inline constexpr progressBarGradientData gradient{
    shadeLevels,
    U' '
};

// ─────────────────────────────────────────────
// Eighth-block gradient
// ▏▎▍▌▋▊▉█ ... smooth 8-level fill
// ─────────────────────────────────────────────

inline constexpr char32_t eighthLevels[] = {
    U'▏', U'▎', U'▍', U'▌', U'▋', U'▊', U'▉', U'█'
};

inline constexpr progressBarGradientData gradient8{
    eighthLevels,
    U' '
};

// ─────────────────────────────────────────────
// Gradient with head
// ░░▒▓█>░░
// ─────────────────────────────────────────────

inline constexpr progressBarGradientData gradientRamp{
    shadeLevels,
    U' ',
    U'>'
};

// ─────────────────────────────────────────────
// Eighth-block gradient with head
// ▏▎▍▌▋▊▉█▶░
// ─────────────────────────────────────────────

inline constexpr progressBarGradientData gradient8Ramp{
    eighthLevels,
    U' ',
    U'▶'
};

}  // namespace progressBarStyle

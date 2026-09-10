#pragma once

#include <chrono>
#include <span>

struct singleSpinnerData {
  std::span<const char32_t> frames;
  std::chrono::milliseconds interval;
};

namespace singleSpinnerStyle {

using namespace std::chrono_literals;

// ─────────────────────────────────────────────
// Classic
// ─────────────────────────────────────────────

inline constexpr char32_t classicFrames[] = {
    U'|', U'/', U'-', U'\\'
};

inline constexpr singleSpinnerData classic{
    classicFrames,
    100ms
};

// ─────────────────────────────────────────────
// Braille
// ─────────────────────────────────────────────

inline constexpr char32_t brailleFrames[] = {
    U'⠋', U'⠙', U'⠹', U'⠸', U'⠼',
    U'⠴', U'⠦', U'⠧', U'⠇', U'⠏'
};

inline constexpr singleSpinnerData braille{
    brailleFrames,
    80ms
};

// ─────────────────────────────────────────────
// Braille dots
// ─────────────────────────────────────────────

inline constexpr char32_t dotsFrames[] = {
    U'⠋', U'⠙', U'⠚', U'⠒', U'⠂',
    U'⠂', U'⠒', U'⠲', U'⠴', U'⠦',
    U'⠖', U'⠒', U'⠐', U'⠐', U'⠒',
    U'⠓', U'⠋'
};

inline constexpr singleSpinnerData dots{
    dotsFrames,
    80ms
};

// ─────────────────────────────────────────────
// Circle
// ─────────────────────────────────────────────

inline constexpr char32_t circleFrames[] = {
    U'◐', U'◓', U'◑', U'◒'
};

inline constexpr singleSpinnerData circle{
    circleFrames,
    120ms
};

// ─────────────────────────────────────────────
// Arrows
// ─────────────────────────────────────────────

inline constexpr char32_t arrowsFrames[] = {
    U'←', U'↖', U'↑', U'↗',
    U'→', U'↘', U'↓', U'↙'
};

inline constexpr singleSpinnerData arrows{
    arrowsFrames,
    100ms
};

// ─────────────────────────────────────────────
// Box
// ─────────────────────────────────────────────

inline constexpr char32_t boxFrames[] = {
    U'◰', U'◳', U'◲', U'◱'
};

inline constexpr singleSpinnerData box{
    boxFrames,
    120ms
};

// ─────────────────────────────────────────────
// Blocks
// ─────────────────────────────────────────────

inline constexpr char32_t blocksFrames[] = {
    U'▖', U'▘', U'▝', U'▗'
};

inline constexpr singleSpinnerData blocks{
    blocksFrames,
    120ms
};

// ─────────────────────────────────────────────
// Growing dot
// ─────────────────────────────────────────────

inline constexpr char32_t growingDotFrames[] = {
    U'·', U'•', U'●', U'•'
};

inline constexpr singleSpinnerData growingDot{
    growingDotFrames,
    180ms
};

}  // namespace singleSpinnerStyle

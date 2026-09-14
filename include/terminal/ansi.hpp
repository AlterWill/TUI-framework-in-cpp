#pragma once

#include <cstddef>
#include <iostream>
#include <string>

// ──────────────────────────────────────────────────────────────────────────────
//  ansi.hpp — Centralised ANSI escape sequence helpers
//
//  All raw terminal escape codes live here. The `backend` layer (and anything
//  that needs low-level terminal control) should call these instead of
//  hand-writing "\x1b[..." strings.
// ──────────────────────────────────────────────────────────────────────────────

namespace ansi {

// ── Cursor visibility ────────────────────────────────────────────────────────
inline void hideCursor()         { std::cout << "\x1b[?25l"; }
inline void showCursor()         { std::cout << "\x1b[?25h" << std::flush; }

// ── Cursor movement ──────────────────────────────────────────────────────────
inline void cursorToRow(std::size_t col) {
  std::cout << "\x1b[" << col << "G";
}
inline void cursorUp(std::size_t amount) {
  std::cout << "\x1b[" << amount << "A";
}
inline void cursorDown(std::size_t amount) {
  std::cout << "\x1b[" << amount << "B";
}
inline void cursorRight(std::size_t amount) {
  std::cout << "\x1b[" << amount << "C";
}
inline void cursorLeft(std::size_t amount) {
  std::cout << "\x1b[" << amount << "D";
}
inline void cursorTo(std::size_t row, std::size_t col) {
  std::cout << "\x1b[" << row << ";" << col << "H";
}
inline void cursorToHome() { std::cout << "\x1b[H"; }

// ── Screen clearing ──────────────────────────────────────────────────────────
inline void clearScreen()        { std::cout << "\x1b[2J"; }
inline void clearCurrentLine()   { std::cout << "\x1b[2K"; }
inline void clearToEndOfLine()   { std::cout << "\x1b[0K"; }
inline void clearToStartOfLine() { std::cout << "\x1b[1K"; }
inline void clearToEndOfScreen() { std::cout << "\x1b[0J"; }

// ── Alternate screen buffer ──────────────────────────────────────────────────
inline void enterAlternateScreenBuffer()  { std::cout << "\x1b[?1049h"; }
inline void leaveAlternateScreenBuffer() {
  std::cout << "\x1b[?1049l" << std::flush;
}

// ── Style / reset ────────────────────────────────────────────────────────────
inline void resetStyle() { std::cout << "\x1b[0m"; }

}  // namespace ansi

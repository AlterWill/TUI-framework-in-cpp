#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <string>
#include <vector>

#include "unicode.hpp"

enum class AnsiColor : uint8_t {
  Black = 0,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  BrightBlack,
  BrightRed,
  BrightGreen,
  BrightYellow,
  BrightBlue,
  BrightMagenta,
  BrightCyan,
  BrightWhite
};

struct Sytle {
  AnsiColor fg = AnsiColor::White;
  AnsiColor bg = AnsiColor::Black;
  bool bold = false;
  bool underline = false;
  bool italic = false;
};

struct Cell {
  char32_t glyph = U' ';
  Sytle sytle;
};

class frameBuffer {
  winsize w;
  std::vector<Cell> buffer;
  std::vector<Cell> previousFrame;

public:
  int row;
  int col;
  std::string displayOutput;
  frameBuffer() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
    buffer.resize(row * col);
    previousFrame.resize(row * col, Cell{});
    displayOutput = "";
  }

  void clear() { std::fill(buffer.begin(), buffer.end(), Cell{}); }

  void resizeBuffer() {
    displayOutput.clear();
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
    buffer.resize(row * col);
    clear();
  }

  void setGlyph(int x, int y, char32_t glyph) {
    buffer[y * col + x].glyph = glyph;
  }

  void setStyle(int x, int y, Sytle sytle) {
    buffer[y * col + x].sytle = sytle;
  }

  void setColor(int x, int y, AnsiColor fg, AnsiColor bg) {
    buffer[y * col + x].sytle.fg = fg;
    buffer[y * col + x].sytle.bg = bg;
  }

  void setCell(int x, int y, const Cell &NewCell) {
    buffer[y * col + x] = NewCell;
  }

  void display() {
    displayOutput.clear();
    for (int i = 0; i < row - 1; i++) {
      for (int j = 0; j < col; j++) {
        displayOutput += unicode::toUtf8(buffer[i * col + j].glyph);
      }
      displayOutput += '\n';
    }
    for (int j = 0; j < col; j++) {
      displayOutput += unicode::toUtf8(buffer[(row - 1) * col + j].glyph);
    }
  }
};

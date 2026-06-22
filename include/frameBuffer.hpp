#pragma once

#include "unicode.hpp"
#include <cstdint>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

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
  int row;
  int col;
  winsize w;
  std::vector<Cell> buffer;
  std::vector<Cell> previousFrame;
  std::string displayOutput;

public:
  frameBuffer() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
    buffer.resize(row * col);
    previousFrame.resize(row * col);
    displayOutput = "";
  }
  void resizeBuffer() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
    buffer.resize(row * col);
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
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
        displayOutput += unicode::toUtf8(buffer[i * col + j].glyph);
      }
    }
  }
};

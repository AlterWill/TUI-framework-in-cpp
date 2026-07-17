#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "unicode.hpp"

enum class AnsiColor {
  Default,
  Black = 30,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  BrightBlack = 90,
  BrightRed,
  BrightGreen,
  BrightYellow,
  BrightBlue,
  BrightMagenta,
  BrightCyan,
  BrightWhite
};

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

struct Colour {
  AnsiColor fg = AnsiColor::Default;
  AnsiColor bg = AnsiColor::Default;
};

struct Style {
  Colour colour;
  uint8_t textStyle = static_cast<uint8_t>(TextStyle::None);
};

struct Cell {
  char32_t glyph = U' ';
  Style style;
};

class frameBuffer {
  winsize w;
  std::vector<Cell> currentBuffer;
  std::vector<Cell> previousBuffer;
  const std::string ESCAPE_SEQUENCE_ESC = "\x1b";
  std::string displayOutput;

 public:
  int row;
  int col;

  frameBuffer() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
    currentBuffer.resize(row * col, Cell{});
    previousBuffer.resize(row * col, Cell{});
    displayOutput = "";
  }

  void clear() { std::fill(currentBuffer.begin(), currentBuffer.end(), Cell{}); }

  void resizeBuffer() {
    displayOutput.clear();
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
    currentBuffer.resize(row * col);
    previousBuffer.resize(row * col);
    clear();
  }

  void setGlyph(int x, int y, char32_t glyph) { currentBuffer[y * col + x].glyph = glyph; }
  void setStyle(int x, int y, Style style) { currentBuffer[y * col + x].style = style; }
  void setColor(int x, int y, AnsiColor fg, AnsiColor bg) {
    currentBuffer[y * col + x].style.colour.fg = fg;
    currentBuffer[y * col + x].style.colour.bg = bg;
  }
  void setForegroundColor(int x, int y, AnsiColor fg) { currentBuffer[y * col + x].style.colour.fg = fg; }
  void setBackgroundColor(int x, int y, AnsiColor bg) { currentBuffer[y * col + x].style.colour.bg = bg; }
  void setCell(int x, int y, const Cell& NewCell) { currentBuffer[y * col + x] = NewCell; }
  void setTextStyle(int index, TextStyle t) { currentBuffer[index].style.textStyle |= static_cast<int>(t); }
  void removeTextStyle(int index, TextStyle t) { currentBuffer[index].style.textStyle &= ~static_cast<int>(t); }
  bool hasTextStyle(int index, TextStyle t) {
    return (currentBuffer[index].style.textStyle & static_cast<int>(t)) != 0;
  }

  void display() {
    displayOutput.clear();
    for (int i = 0; i < row - 1; i++) {
      for (int j = 0; j < col; j++) {
        displayOutput += displayBufferPixel(i, j);
      }
      displayOutput += '\n';
    }
    for (int j = 0; j < col; j++) {
      displayOutput += displayBufferPixel(row - 1, j);
    }
    std::cout << displayOutput << std::endl;
  }
  
  // fix the display increment 
  void incrementDisplay() {
    displayOutput.clear();
    bool previousDirty = false;
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
        if (!compareCell(i, j)) {
          previousDirty = false;
          continue;
        }

        if (!previousDirty) {
          displayOutput += ESCAPE_SEQUENCE_ESC + "[" + std::to_string(i + 1) + ";" + std::to_string(j + 1) + "H";
        }

        displayOutput += displayBufferPixel(i, j);
        previousDirty = true;
      }
    }
    previousBuffer = currentBuffer;
    std::cout << displayOutput << std::endl;
  }

 protected:
  bool compareCell(int i, int j) {
    int index = i * col + j;
    if (currentBuffer[index].glyph != previousBuffer[index].glyph) return true;
    if (currentBuffer[index].style.colour.fg != previousBuffer[index].style.colour.fg) return true;
    if (currentBuffer[index].style.colour.bg != previousBuffer[index].style.colour.bg) return true;
    if (currentBuffer[index].style.textStyle != previousBuffer[index].style.textStyle) return true;
    return false;
  }

  std::string displayBufferPixel(int i, int j) {
    int needReset = false;
    size_t index = i * col + j;
    if (index >= currentBuffer.size()) throw std::runtime_error("Out of Borders for the Display Buffer");
    std::string pixelOutput;
    std::vector<std::string> sgr;

    for (int i = 0; i < 8; i++) {
      if ((currentBuffer[index].style.textStyle & (1 << i)) != 0) {
        needReset = true;
        sgr.push_back(std::to_string(i + 1));
      }
    }

    if (currentBuffer[index].style.colour.bg != AnsiColor::Default) {
      needReset = true;
      sgr.push_back(std::to_string(static_cast<int>(currentBuffer[index].style.colour.bg) + 10));
    }
    if (currentBuffer[index].style.colour.fg != AnsiColor::Default) {
      needReset = true;
      sgr.push_back(std::to_string(static_cast<int>(currentBuffer[index].style.colour.fg)));
    }

    if (needReset) {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[";
      for (size_t i = 0; i < sgr.size(); i++) {
        pixelOutput += sgr[i];
        if (i != sgr.size() - 1) {
          pixelOutput += ";";
        }
      }
      pixelOutput += "m";
    }

    pixelOutput += unicode::toUtf8(currentBuffer[index].glyph);

    if (needReset) {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[0m";
    }
    return pixelOutput;
  }
};

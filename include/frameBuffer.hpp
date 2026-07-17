#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "tools.hpp"
#include "backend.hpp"
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
  std::vector<Cell> currentBuffer;
  std::vector<Cell> previousBuffer;
  const std::string ESCAPE_SEQUENCE_ESC = "\x1b";
  std::string displayOutput;

 public:
  backend terminalData;

  frameBuffer() {
    terminalData.findTerminalSize();
    
    currentBuffer.resize(terminalData.row*terminalData.col, Cell{});
    previousBuffer.resize(terminalData.row*terminalData.col, Cell{});
    displayOutput = "";
  }

  void clear() { std::fill(currentBuffer.begin(), currentBuffer.end(), Cell{}); }

  void resizeBuffer() {
    displayOutput.clear();
    terminalData.findTerminalSize();
    if (currentBuffer.size() != static_cast<size_t>(terminalData.row*terminalData.col)) {
      currentBuffer.resize(terminalData.row*terminalData.col);
      previousBuffer.resize(terminalData.row*terminalData.col);

      tools::clearScreen();
      std::fill(previousBuffer.begin(),previousBuffer.end(),Cell{});
    }
    clear();
  }

  void setGlyph(size_t x, size_t y, char32_t glyph) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x].glyph = glyph;
  }
  void setStyle(size_t x, size_t y, Style style) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x].style = style;
  }
  void setColor(size_t x, size_t y, AnsiColor fg, AnsiColor bg) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x].style.colour.fg = fg;
    currentBuffer[y * terminalData.col + x].style.colour.bg = bg;
  }
  void setForegroundColor(size_t x, size_t y, AnsiColor fg) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x].style.colour.fg = fg;
  }
  void setBackgroundColor(size_t x, size_t y, AnsiColor bg) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x].style.colour.bg = bg;
  }
  void setCell(size_t x, size_t y, const Cell& NewCell) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x] = NewCell;
  }
  void setTextStyle(size_t x, size_t y, TextStyle t) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x].style.textStyle |= static_cast<int>(t);
  }
  void removeTextStyle(size_t x, size_t y, TextStyle t) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x].style.textStyle &= ~static_cast<int>(t);
  }
  bool hasTextStyle(size_t x, size_t y, TextStyle t) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return false;
    return (currentBuffer[y * terminalData.col + x].style.textStyle & static_cast<int>(t)) != 0;
  }

  void display() {
    displayOutput.clear();
    for (size_t i = 0; i < terminalData.row - 1; i++) {
      for (size_t j = 0; j < terminalData.col; j++) {
        displayOutput += displayBufferPixel(i, j);
      }
      displayOutput += '\n';
    }
    for (size_t j = 0; j < terminalData.col; j++) {
      displayOutput += displayBufferPixel(terminalData.row - 1, j);
    }
    std::cout << displayOutput << std::flush;
  }

  void incrementDisplay() {
    displayOutput.clear();
    bool previousDirty = false;
    for (size_t i = 0; i < terminalData.row; i++) {
      previousDirty = false;
      for (size_t j = 0; j < terminalData.col; j++) {
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
    std::cout << displayOutput << std::flush;
  }

 protected:
  bool compareCell(size_t i, size_t j) {
    if (i < 0 || i >= terminalData.row || j < 0 || j >= terminalData.col) return false;
    int index = i * terminalData.col + j;
    if (currentBuffer[index].glyph != previousBuffer[index].glyph) return true;
    if (currentBuffer[index].style.colour.fg != previousBuffer[index].style.colour.fg) return true;
    if (currentBuffer[index].style.colour.bg != previousBuffer[index].style.colour.bg) return true;
    if (currentBuffer[index].style.textStyle != previousBuffer[index].style.textStyle) return true;
    return false;
  }

  std::string displayBufferPixel(int i, int j) {
    int needReset = false;
    size_t index = i * terminalData.col + j;
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

#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "backend.hpp"
#include "cell.hpp"
#include "tools.hpp"
#include "unicode.hpp"

class frameBuffer {
  std::vector<Cell> currentBuffer;
  std::vector<Cell> previousBuffer;
  const std::string ESCAPE_SEQUENCE_ESC = "\x1b";
  std::string displayOutput;

 public:
  backend terminalData;

  frameBuffer() {
    terminalData.findTerminalSize();
    currentBuffer.resize(terminalData.row * terminalData.col, Cell{});
    previousBuffer.resize(terminalData.row * terminalData.col, Cell{});
    displayOutput = "";
  }

  void clear() { std::fill(currentBuffer.begin(), currentBuffer.end(), Cell{}); }

  void resizeBuffer() {
    displayOutput.clear();
    terminalData.findTerminalSize();
    if (currentBuffer.size() != static_cast<size_t>(terminalData.row * terminalData.col)) {
      currentBuffer.resize(terminalData.row * terminalData.col);
      previousBuffer.resize(terminalData.row * terminalData.col);

      tools::clearScreen();
      std::fill(previousBuffer.begin(), previousBuffer.end(), Cell{});
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
  void setCell(size_t x, size_t y, const Cell& NewCell) {
    if (x < 0 || x >= terminalData.col || y < 0 || y >= terminalData.row) return;
    currentBuffer[y * terminalData.col + x] = NewCell;
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
  bool compareColour(Colour a, Colour b) { return static_cast<uint32_t>(a.colour) == static_cast<uint32_t>(b.colour); }

  bool compareCell(size_t i, size_t j) {
    if (i < 0 || i >= terminalData.row || j < 0 || j >= terminalData.col) return false;
    int index = i * terminalData.col + j;
    if (currentBuffer[index].glyph != previousBuffer[index].glyph) return true;
    if (compareColour(currentBuffer[index].style.colours.fg, previousBuffer[index].style.colours.fg)) return true;
    if (compareColour(currentBuffer[index].style.colours.bg, previousBuffer[index].style.colours.bg)) return true;
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

    // ESC[38;2;{r};{g};{b}m
    if (currentBuffer[index].style.colours.fg.getAlpha() > 0) {
      //clang-format off
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[38;2;" +
                     std::to_string(currentBuffer[index].style.colours.fg.getRedValue()) + ";" +
                     std::to_string(currentBuffer[index].style.colours.fg.getGreenValue()) + ";" +
                     std::to_string(currentBuffer[index].style.colours.fg.getBlueValue()) + "m";
    } else {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[39m";
    }

    // ESC[48;2;{r};{g};{b}m
    if (currentBuffer[index].style.colours.bg.getAlpha() > 0) {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[48;2;" +
                     std::to_string(currentBuffer[index].style.colours.bg.getRedValue()) + ";" +
                     std::to_string(currentBuffer[index].style.colours.bg.getGreenValue()) + ";" +
                     std::to_string(currentBuffer[index].style.colours.bg.getBlueValue()) + "m";
      //clang-format on
    } else {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[49m";
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

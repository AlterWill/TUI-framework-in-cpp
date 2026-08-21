#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "buffer.hpp"
#include "backend.hpp"
#include "tools.hpp"
#include "unicode.hpp"

class Surface {
  Buffer current;
  Buffer previous;

  const std::string ESCAPE_SEQUENCE_ESC = "\x1b";
  std::string displayOutput;

 public:
  backend& terminalData;

  Surface(backend& terminal) : terminalData(terminal) {
    terminalData.findTerminalSize();

    current.resize(terminalData.col, terminalData.row);
    previous.resize(terminalData.col, terminalData.row);

    displayOutput = "";
  }

  // Widgets write directly to the current frame buffer.
  Buffer& getBuffer() { return current; }

  void clear() {
    std::fill(current.cells.begin(), current.cells.end(), Cell{});
  }

  void resizeBuffer() {
    displayOutput.clear();

    terminalData.findTerminalSize();

    if (current.cells.size() !=
        static_cast<std::size_t>(terminalData.row * terminalData.col)) {

      current.resize(terminalData.col, terminalData.row);
      previous.resize(terminalData.col, terminalData.row);

      tools::clearScreen();

      std::fill(
          previous.cells.begin(),
          previous.cells.end(),
          Cell{}
      );
    }

    clear();
  }

  // Direct writes to the current buffer.
  // These are mostly convenience functions; widgets can also use getBuffer().
  void setGlyph(std::size_t x, std::size_t y, char32_t glyph) {
    if (x >= terminalData.col || y >= terminalData.row) return;

    current.cells[y * terminalData.col + x].glyph = glyph;
  }

  void setStyle(std::size_t x, std::size_t y, Style style) {
    if (x >= terminalData.col || y >= terminalData.row) return;

    current.cells[y * terminalData.col + x].style = style;
  }

  void setCell(std::size_t x, std::size_t y, const Cell& newCell) {
    if (x >= terminalData.col || y >= terminalData.row) return;

    current.cells[y * terminalData.col + x] = newCell;
  }

  void display() {
    if (terminalData.row == 0 || terminalData.col == 0) return;

    displayOutput.clear();

    for (std::size_t i = 0; i < terminalData.row - 1; i++) {
      for (std::size_t j = 0; j < terminalData.col; j++) {
        displayOutput += displayBufferPixel(i, j);
      }

      displayOutput += '\n';
    }

    for (std::size_t j = 0; j < terminalData.col; j++) {
      displayOutput += displayBufferPixel(
          terminalData.row - 1,
          j
      );
    }

    std::cout << displayOutput << std::flush;
  }

  void incrementDisplay() {
    displayOutput.clear();

    bool previousDirty = false;

    for (std::size_t i = 0; i < terminalData.row; i++) {
      previousDirty = false;

      for (std::size_t j = 0; j < terminalData.col; j++) {
        if (!isCellDirty(i, j)) {
          previousDirty = false;
          continue;
        }

        if (!previousDirty) {
          displayOutput +=
              ESCAPE_SEQUENCE_ESC +
              "[" +
              std::to_string(i + 1) +
              ";" +
              std::to_string(j + 1) +
              "H";
        }

        displayOutput += displayBufferPixel(i, j);

        previousDirty = true;
      }
    }

    previous.cells = current.cells;

    std::cout << displayOutput << std::flush;
  }

 protected:
  bool compareColour(Colour a, Colour b) {
    return static_cast<uint32_t>(a.colour) ==
           static_cast<uint32_t>(b.colour);
  }

  bool isCellDirty(std::size_t i, std::size_t j) {
    if (i >= terminalData.row || j >= terminalData.col) {
      return false;
    }

    std::size_t index = i * terminalData.col + j;

    if (current.cells[index].glyph != previous.cells[index].glyph) {
      return true;
    }

    if (!compareColour(
            current.cells[index].style.colours.fg,
            previous.cells[index].style.colours.fg)) {
      return true;
    }

    if (!compareColour(
            current.cells[index].style.colours.bg,
            previous.cells[index].style.colours.bg)) {
      return true;
    }

    if (current.cells[index].style.textStyle !=
        previous.cells[index].style.textStyle) {
      return true;
    }

    return false;
  }

  // Gives the pixel with ANSI code.
  std::string displayBufferPixel(std::size_t i, std::size_t j) {
    bool needReset = false;

    std::size_t index = i * terminalData.col + j;

    if (index >= current.cells.size()) {
      throw std::runtime_error(
          "Out of Borders for the Display Buffer"
      );
    }

    std::string pixelOutput;
    std::vector<std::string> sgr;

    for (int styleBit = 0; styleBit < 8; styleBit++) {
      if ((current.cells[index].style.textStyle &
           (1 << styleBit)) != 0) {

        needReset = true;
        sgr.push_back(std::to_string(styleBit + 1));
      }
    }

    // Foreground Color
    if (current.cells[index].style.colours.fg.getAlpha() > 0) {
      if (terminalData.supportsTrueColor) {
        // ESC[38;2;{r};{g};{b}m
        pixelOutput +=
            ESCAPE_SEQUENCE_ESC +
            "[38;2;" +
            std::to_string(
                current.cells[index].style.colours.fg.getRedValue()
            ) +
            ";" +
            std::to_string(
                current.cells[index].style.colours.fg.getGreenValue()
            ) +
            ";" +
            std::to_string(
                current.cells[index].style.colours.fg.getBlueValue()
            ) +
            "m";

      } else if (terminalData.supports256Color) {
        // ESC[38;5;{id}m
        pixelOutput +=
            ESCAPE_SEQUENCE_ESC +
            "[38;5;" +
            std::to_string(
                current.cells[index].style.colours.fg.to256Palette()
            ) +
            "m";

      } else {
        // 16-color fallback
        uint8_t colorIndex =
            current.cells[index].style.colours.fg.to16Palette();

        if (colorIndex < 8) {
          pixelOutput +=
              ESCAPE_SEQUENCE_ESC +
              "[" +
              std::to_string(30 + colorIndex) +
              "m";
        } else {
          pixelOutput +=
              ESCAPE_SEQUENCE_ESC +
              "[" +
              std::to_string(90 + (colorIndex - 8)) +
              "m";
        }
      }
    } else {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[39m";
    }

    // Background Color
    if (current.cells[index].style.colours.bg.getAlpha() > 0) {
      if (terminalData.supportsTrueColor) {
        // ESC[48;2;{r};{g};{b}m
        pixelOutput +=
            ESCAPE_SEQUENCE_ESC +
            "[48;2;" +
            std::to_string(
                current.cells[index].style.colours.bg.getRedValue()
            ) +
            ";" +
            std::to_string(
                current.cells[index].style.colours.bg.getGreenValue()
            ) +
            ";" +
            std::to_string(
                current.cells[index].style.colours.bg.getBlueValue()
            ) +
            "m";

      } else if (terminalData.supports256Color) {
        // ESC[48;5;{id}m
        pixelOutput +=
            ESCAPE_SEQUENCE_ESC +
            "[48;5;" +
            std::to_string(
                current.cells[index].style.colours.bg.to256Palette()
            ) +
            "m";

      } else {
        // 16-color fallback
        uint8_t colorIndex =
            current.cells[index].style.colours.bg.to16Palette();

        if (colorIndex < 8) {
          pixelOutput +=
              ESCAPE_SEQUENCE_ESC +
              "[" +
              std::to_string(40 + colorIndex) +
              "m";
        } else {
          pixelOutput +=
              ESCAPE_SEQUENCE_ESC +
              "[" +
              std::to_string(100 + (colorIndex - 8)) +
              "m";
        }
      }
    } else {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[49m";
    }

    // Text styles
    if (needReset) {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[";

      for (std::size_t i = 0; i < sgr.size(); i++) {
        pixelOutput += sgr[i];

        if (i != sgr.size() - 1) {
          pixelOutput += ";";
        }
      }

      pixelOutput += "m";
    }

    pixelOutput +=
        unicode::toUtf8(current.cells[index].glyph);

    if (needReset) {
      pixelOutput += ESCAPE_SEQUENCE_ESC + "[0m";
    }

    return pixelOutput;
  }
};

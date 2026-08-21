#pragma once

#include <algorithm>
#include <vector>

#include "Rect.hpp"
#include "cell.hpp"

class Buffer {
  std::size_t bufWidth{};
  std::size_t bufHeight{};

 public:
  std::vector<Cell> cells;

  Buffer() = default;
  Buffer(std::size_t width, std::size_t height) : bufWidth(width), bufHeight(height), cells(width * height, Cell{}) {}

  void resize(std::size_t width, std::size_t height) {
    bufWidth = width;
    bufHeight = height;
    cells.assign(width * height, Cell{});
  }

  void clear() { std::fill(cells.begin(), cells.end(), Cell{}); }

  std::size_t getWidth() const { return bufWidth; }
  std::size_t getHeight() const { return bufHeight; }

  void setCell(std::size_t x, std::size_t y, const Cell& cell) {
    if (x >= bufWidth || y >= bufHeight) return;
    cells[y * bufWidth + x] = cell;
  }

  void setGlyph(std::size_t x, std::size_t y, char32_t glyph) {
    if (x >= bufWidth || y >= bufHeight) return;
    cells[y * bufWidth + x].glyph = glyph;
  }

  void setStyle(std::size_t x, std::size_t y, Style style) {
    if (x >= bufWidth || y >= bufHeight) return;
    cells[y * bufWidth + x].style = style;
  }

  const Cell& at(std::size_t x, std::size_t y) const { return cells[y * bufWidth + x]; }

  void blitTo(Buffer& dst, const Rect& srcRegion, const Rect& dstRegion) const {
    for (std::size_t row = 0; row < srcRegion.height; ++row) {
      for (std::size_t col = 0; col < srcRegion.width; ++col) {
        std::size_t srcX = srcRegion.x + col;
        std::size_t srcY = srcRegion.y + row;
        std::size_t dstX = dstRegion.x + col;
        std::size_t dstY = dstRegion.y + row;
        if (srcX >= bufWidth || srcY >= bufHeight) continue;
        if (col >= dstRegion.width || row >= dstRegion.height) continue;
        if (dstX >= dst.getWidth() || dstY >= dst.getHeight()) continue;
        dst.setCell(dstX, dstY, at(srcX, srcY));
      }
    }
  }

};

#pragma once

#include "utilities/Rect.hpp"
#include "rendering/buffer.hpp"
#include "rendering/cell.hpp"
#include "styling/style.hpp"

struct RenderContext {
 private:
  Buffer& frameBuffer;
  Rect currentRect{};

 public:
  explicit RenderContext(Buffer& buffer, Rect r = {}) : frameBuffer(buffer), currentRect(r) {}

  void setRect(const Rect& r) { currentRect = r; }
  const Rect& getRect() const { return currentRect; }

  void setCell(std::size_t x, std::size_t y, const Cell& cell) { frameBuffer.setCell(x, y, cell); }

  void setCell(Point point, const Cell& cell) { frameBuffer.setCell(point, cell); }

  void setGlyph(std::size_t x, std::size_t y, char32_t glyph) { frameBuffer.setGlyph(x, y, glyph); }

  void setStyle(std::size_t x, std::size_t y, Style style) { frameBuffer.setStyle(x, y, style); }

  const Cell& at(std::size_t x, std::size_t y) const { return frameBuffer.at(x, y); }

  Buffer& getBuffer() { return frameBuffer; }

  const Buffer& getBuffer() const { return frameBuffer; }
};

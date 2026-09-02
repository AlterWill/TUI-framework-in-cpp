#pragma once

#include <cstddef>
#include <optional>
#include "input/event.hpp"

class backend {
 public:
  std::size_t row{0};
  std::size_t col{0};
  bool supportsTrueColor{false};
  bool supports256Color{false};

  virtual ~backend() = default;

  virtual void findTerminalSize() = 0;
  virtual void findSupportsTrueAnd256Colour() = 0;
  virtual std::size_t getTerminalSizeRow() = 0;
  virtual std::size_t getTerminalSizeCol() = 0;
  virtual int readKey() = 0;
  virtual std::optional<Event> readEvent() = 0;

  // Getters
  std::size_t getRow() const { return row; }
  std::size_t getCol() const { return col; }
  bool getSupportsTrueColor() const { return supportsTrueColor; }
  bool getSupports256Color() const { return supports256Color; }

  // Setters
  void setRow(std::size_t val) { row = val; }
  void setCol(std::size_t val) { col = val; }
  void setSupportsTrueColor(bool val) { supportsTrueColor = val; }
  void setSupports256Color(bool val) { supports256Color = val; }
};

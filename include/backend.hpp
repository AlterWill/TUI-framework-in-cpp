#pragma once

#include <cstddef>
#include <optional>
#include "event.hpp"

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
};

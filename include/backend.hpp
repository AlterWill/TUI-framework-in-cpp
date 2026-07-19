#pragma once

#include <cstddef>
#include <string>

class backend {
 public:
  size_t row = 0;
  size_t col = 0;
  bool supportsTrueColor = false;
  bool supports256Color = false;

  virtual ~backend() = default;

  virtual void findTerminalSize() = 0;
  virtual void findSupportsTrueAnd256Colour() = 0;
  virtual size_t getTerminalSizeRow() = 0;
  virtual size_t getTerminalSizeCol() = 0;
};

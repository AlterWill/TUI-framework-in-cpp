#pragma once

#include <cstddef>

enum KeyAction {
  ARROW_UP = 1000,
  ARROW_DOWN,
  ARROW_RIGHT,
  ARROW_LEFT,
  PAGE_UP,
  PAGE_DOWN,
  HOME_KEY,
  END_KEY,
  DEL_KEY
};

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
  virtual int readKey() = 0;
};

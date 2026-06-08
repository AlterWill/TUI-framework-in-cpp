#pragma once
#include <string>

const std::string ESC = "\x1b";

namespace tools {
  void visiableCursor();
  void invisiableCursor();
  void cursorHomePosition();
  void clearScreen();
  void setCursorPosition(int x, int y);
};

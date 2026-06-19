#pragma once
#include <string>

extern const std::string ESC;

namespace tools {
  void visiableCursor();
  void invisiableCursor();
  void cursorHomePosition();
  void clearScreen();
  void setCursorPosition(int x, int y);
}

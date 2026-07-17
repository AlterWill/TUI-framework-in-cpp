#include "tools.hpp"

#include <iostream>

namespace tools {
  const std::string ESC = "\x1b";
  void visiableCursor() { std::cout << ESC << "[?25h"; }
  void invisiableCursor() { std::cout << ESC << "[?25l"; }
  void cursorHomePosition() { std::cout << ESC << "[H"; }
  void clearScreen() { std::cout << ESC << "[2J"; }
  void setCursorPosition(int x, int y) { std::cout << ESC << "[" << y << ";" << x << "H"; }
}

#pragma once
#include <string>
#include <iostream>

namespace tools {
  inline const std::string ESC = "\x1b";
  inline void visiableCursor() { std::cout << ESC << "[?25h"; }
  inline void alternateScreenBuffer() { std::cout << ESC << "[?1049h"; }
  inline void invisiableCursor() { std::cout << ESC << "[?25l"; }
  inline void cursorHomePosition() { std::cout << ESC << "[H"; }
  inline void clearScreen() { std::cout << ESC << "[2J"; }
  inline void setCursorPosition(int x, int y) { std::cout << ESC << "[" << y << ";" << x << "H"; }
}

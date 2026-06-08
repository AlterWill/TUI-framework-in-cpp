#include "tools.hpp"
#include <iostream>

void tools::visiableCursor() { std::cout << ESC << "[?25h"; }
void tools::invisiableCursor() { std::cout << ESC << "[?25l"; }
void tools::cursorHomePosition() { std::cout << ESC << "[H"; }
void tools::clearScreen() { std::cout << ESC << "[2J"; }
void tools::setCursorPosition(int x, int y) {
  cursorHomePosition();
  std::cout << ESC << "[" << y << ";" << x << "H";
}

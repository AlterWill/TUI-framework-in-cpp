module;
#include <iostream>
#include <string>

export module tools;

export const std::string ESC = "\x1b";

export namespace tools {
  void visiableCursor() { std::cout << ESC << "[?25h"; }
  void invisiableCursor() { std::cout << ESC << "[?25l"; }
  void cursorHomePosition() { std::cout << ESC << "[H"; }
  void clearScreen() { std::cout << ESC << "[2J"; }
  void setCursorPosition(int x, int y) {
    cursorHomePosition();
    std::cout << ESC << "[" << y << ";" << x << "H";
  }
}

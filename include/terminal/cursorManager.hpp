#pragma once

#include <iostream>
#include <string>

#include "utilities/Point.hpp"

enum class CursorShape { Block, Beam, Underline };

struct CursorManager {
  Point cursorPosition{};
  bool visible{true};
  const std::string ESC = "\x1b";

  void setCursorPosition(int x, int y) { std::cout << ESC << "[" << y << ";" << x << "H"; }

  inline void visiableCursor() { std::cout << ESC << "[?25h"; }
  inline void alternateScreenBuffer() { std::cout << ESC << "[?1049h"; }
  inline void invisiableCursor() { std::cout << ESC << "[?25l"; }
  inline void cursorHomePosition() { std::cout << ESC << "[H"; }
  inline void clearScreen() { std::cout << ESC << "[2J"; }

  inline void setCursorShape(CursorShape shape) {
    switch (shape) {
      case CursorShape::Block:
        std::cout << ESC << "[2 q";
        break;
      case CursorShape::Beam:
        std::cout << ESC << "[6 q";
        break;
      case CursorShape::Underline:
        std::cout << ESC << "[4 q";
        break;
    }
  }
};

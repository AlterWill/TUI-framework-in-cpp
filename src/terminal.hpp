#pragma once
#include "boxTypes.hpp"
#include <string>
#include <sys/ioctl.h>
#include <vector>

class terminal {
public:
  int row;
  int col;
  struct winsize w;
  std::vector<std::vector<Cell>> buffer;
  void measurements();
  void createScreen();
  void drawUnicode(int x, int y, char32_t text);
  void drawChar(int x, int y, char text);
  void drawBox(boxOutlineDetails characters, bounds location);
  void drawText(int x, int y, std::string text);
  void display();
  void clearBuffer();
};

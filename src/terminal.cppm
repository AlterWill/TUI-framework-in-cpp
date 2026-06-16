module;
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>

export module terminal;
export import boxTypes;
import tools;
import unicode;

export class terminal {
public:
  int row;
  int col;
  struct winsize w;
  std::vector<std::vector<Cell>> buffer;

  void measurements() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
  }

  void createScreen() {
    buffer.resize(row);
    for (int i = 0; i < row; i++) {
      buffer[i].resize(col);
      for (int j = 0; j < col; j++) {
        buffer[i][j].glyph = ' ';
      }
    }
  }

  // draw Character at x,y
  // We use std::string because we want unicode letters as well
  void drawUnicode(int x, int y, char32_t text) {
    if (x < 0 || x >= col) {
      return;
    }
    if (y < 0 || y >= row) {
      return;
    }
    buffer[y][x].glyph = text;
  }

  void drawChar(int x, int y, char text) {
    if (x < 0 || x >= col) {
      return;
    }
    if (y < 0 || y >= row) {
      return;
    }
    buffer[y][x].glyph = text;
  }

  void drawBox(boxOutlineDetails characters, bounds location) {
    if (this->row == 0 || this->col == 0) {
      tools::clearScreen();
      std::cout << "Failed";
    }
    for (int i = location.x; i < location.x + location.width; i++) {
      for (int j = location.y; j < location.y + location.height; j++) {
        if (i < 0 || i > col || j < 0 || j > row) {
          continue;
        }
        if (j == location.y || j == location.y + location.height - 1) {
          buffer[j][i].glyph = characters.horizontal;
        }
        if (i == location.x || i == location.x + location.width - 1) {
          buffer[j][i].glyph = characters.vertical;
        }
        if (i == location.x && j == location.y) {
          buffer[j][i].glyph = characters.topLeft;
        }
        if (i == location.x && j == location.y + location.height - 1) {
          buffer[j][i].glyph = characters.bottomLeft;
        }
        if (i == location.x + location.width - 1 &&
            j == location.y + location.height - 1) {
          buffer[j][i].glyph = characters.bottomRight;
        }
        if (i == location.x + location.width - 1 && j == location.y) {
          buffer[j][i].glyph = characters.topRight;
        }
      }
    }
  }

  void drawText(int x, int y, std::string text) {
    for (int i = x; i < x + text.length(); i++) {
      drawChar(i, y, text[i - x]);
    }
  }

  // display buffer to terminal
  void display() {
    std::string terminalFrame;
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
        terminalFrame += unicode::toUtf8(buffer[i][j].glyph);
      }
      if (i != row - 1) {
        terminalFrame += '\n';
      }
    }
    std::cout << terminalFrame;
    std::cout.flush();
  }

  void clearBuffer() { buffer.clear(); }
};

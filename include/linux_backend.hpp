#pragma once

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <string>

#include "backend.hpp"

class linux_backend : public backend {
  winsize w;
  termios original;

 public:
  linux_backend() {
    findTerminalSize();
    findSupportsTrueAnd256Colour();

    tcgetattr(STDIN_FILENO, &original);
    termios raw = original;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  }

  ~linux_backend() override {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
  }

  void findSupportsTrueAnd256Colour() override {
    if (const char* colorterm = std::getenv("COLORTERM")) {
      std::string ct(colorterm);
      if (ct == "truecolor" || ct == "24bit") supportsTrueColor = true;
    }
    if (const char* term = std::getenv("TERM")) {
      std::string t(term);
      if (t.find("256color") != std::string::npos) supports256Color = true;
      if (t.find("-truecolor") != std::string::npos) supportsTrueColor = true;
    }
    if (supportsTrueColor) supports256Color = true;
  }

  void findTerminalSize() override {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
  }

  size_t getTerminalSizeRow() override { return w.ws_row; }

  size_t getTerminalSizeCol() override { return w.ws_col; }

  int readKey() override {
    int nread;
    char c;
    
    nread = read(STDIN_FILENO, &c, 1);
    if (nread != 1) return -1; 
    
    if (c == '\x1b') {
      char seq[3];
      if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
      if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

      if (seq[0] == '[') {
        if (seq[1] >= '0' && seq[1] <= '9') {
          if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
          if (seq[2] == '~') {
            switch (seq[1]) {
              case '1': return HOME_KEY;
              case '3': return DEL_KEY;
              case '4': return END_KEY;
              case '5': return PAGE_UP;
              case '6': return PAGE_DOWN;
              case '7': return HOME_KEY;
              case '8': return END_KEY;
            }
          }
        } else {
          switch (seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
          }
        }
      }
      return '\x1b';
    }
    return c;
  }
};

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
  bool keyboardSupport;
  bool mouseSupport;

 public:
  void setKeyboardSupport(bool a) { keyboardSupport = a; }
  void setMouseSupport(bool a) { mouseSupport = a; }

  linux_backend(bool keyboardSupport, bool mouseSupport)
      : keyboardSupport(keyboardSupport), mouseSupport(mouseSupport) {
    findTerminalSize();
    findSupportsTrueAnd256Colour();

    if (keyboardSupport) {
      tcgetattr(STDIN_FILENO, &original);
      termios raw = original;
      raw.c_lflag &= ~(ICANON | ECHO);
      raw.c_cc[VMIN] = 0;
      raw.c_cc[VTIME] = 1;
      tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }

    if (mouseSupport) {
      // Enable SGR mouse mode (recommended)
      write(STDOUT_FILENO, "\x1b[?1000h", 8);  // Button press/release
      write(STDOUT_FILENO, "\x1b[?1002h", 8);  // Drag events
      write(STDOUT_FILENO, "\x1b[?1006h", 8);  // SGR encoding
    }
  }

  ~linux_backend() override {
    if (keyboardSupport) {
      tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    }
    if (mouseSupport) {
      write(STDOUT_FILENO, "\x1b[?1006l", 8);
      write(STDOUT_FILENO, "\x1b[?1002l", 8);
      write(STDOUT_FILENO, "\x1b[?1000l", 8);
    }
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

  std::size_t getTerminalSizeRow() override { return w.ws_row; }

  std::size_t getTerminalSizeCol() override { return w.ws_col; }

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
              case '1':
                return HOME_KEY;
              case '3':
                return DEL_KEY;
              case '4':
                return END_KEY;
              case '5':
                return PAGE_UP;
              case '6':
                return PAGE_DOWN;
              case '7':
                return HOME_KEY;
              case '8':
                return END_KEY;
            }
          }
        } else {
          switch (seq[1]) {
            case 'A':
              return ARROW_UP;
            case 'B':
              return ARROW_DOWN;
            case 'C':
              return ARROW_RIGHT;
            case 'D':
              return ARROW_LEFT;
            case 'H':
              return HOME_KEY;
            case 'F':
              return END_KEY;
          }
        }
      }
      return '\x1b';
    }
    return c;
  }

  std::optional<Event> readEvent() override {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return std::nullopt;

    // Normal character
    if (c != '\x1b') {
      return keyEvent{.key = c};
    }

    // Read after ESC
    char next;
    if (read(STDIN_FILENO, &next, 1) != 1) return keyEvent{.key = '\x1b'};

    if (next != '[') return keyEvent{.key = '\x1b'};

    char third;
    if (read(STDIN_FILENO, &third, 1) != 1) return keyEvent{.key = '\x1b'};

    // ---------------- Mouse ----------------

    if (third == '<') {
      std::string seq = "[<";

      char ch;
      while (read(STDIN_FILENO, &ch, 1) == 1) {
        seq += ch;
        if (ch == 'M' || ch == 'm') break;
      }

      int code, x, y;
      char end;

      if (std::sscanf(seq.c_str(), "[<%d;%d;%d%c", &code, &x, &y, &end) == 4) {
        MouseEvent mouse{};

        mouse.x = x;
        mouse.y = y;

        if (code & 64) {
          mouse.button = MouseButton::None;
          mouse.action = (code & 1) ? MouseAction::ScrollDown : MouseAction::ScrollUp;

          return mouse;
        }

        switch (code & 3) {
          case 0:
            mouse.button = MouseButton::Left;
            break;
          case 1:
            mouse.button = MouseButton::Middle;
            break;
          case 2:
            mouse.button = MouseButton::Right;
            break;
          default:
            mouse.button = MouseButton::None;
        }

        if (code & 32)
          mouse.action = MouseAction::Drag;
        else if (end == 'm')
          mouse.action = MouseAction::Release;
        else
          mouse.action = MouseAction::Press;

        return mouse;
      }

      return std::nullopt;
    }

    // ---------------- Arrow/Home/etc ----------------

    if (third >= '0' && third <= '9') {
      char fourth;

      if (read(STDIN_FILENO, &fourth, 1) != 1) return keyEvent{.key = '\x1b'};

      if (fourth == '~') {
        switch (third) {
          case '1':
            return keyEvent{.key = HOME_KEY};
          case '3':
            return keyEvent{.key = DEL_KEY};
          case '4':
            return keyEvent{.key = END_KEY};
          case '5':
            return keyEvent{.key = PAGE_UP};
          case '6':
            return keyEvent{.key = PAGE_DOWN};
          case '7':
            return keyEvent{.key = HOME_KEY};
          case '8':
            return keyEvent{.key = END_KEY};
        }
      }

      return keyEvent{.key = '\x1b'};
    }

    switch (third) {
      case 'A':
        return keyEvent{.key = ARROW_UP};
      case 'B':
        return keyEvent{.key = ARROW_DOWN};
      case 'C':
        return keyEvent{.key = ARROW_RIGHT};
      case 'D':
        return keyEvent{.key = ARROW_LEFT};
      case 'H':
        return keyEvent{.key = HOME_KEY};
      case 'F':
        return keyEvent{.key = END_KEY};
    }

    return keyEvent{.key = '\x1b'};
  }
};

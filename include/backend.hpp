

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <string>

class backend {
  winsize w;

 public:
  size_t row;
  size_t col;
  bool supportsTrueColor = false;
  bool supports256Color = false;

  backend() {
    findTerminalSize();
    findSupportsTrueAnd256Colour();
  }

  void findSupportsTrueAnd256Colour(){
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

  void findTerminalSize() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
  }

  size_t getTerminalSizeRow() { return w.ws_row; }

  size_t getTerminalSizeCol() { return w.ws_col; }
};

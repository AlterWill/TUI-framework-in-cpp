

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstddef>

class backend {
  winsize w;

public:
  size_t row;
  size_t col;
  void findTerminalSize() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    row = w.ws_row;
    col = w.ws_col;
  }

  size_t getTerminalSizeRow(){
    return w.ws_row;
  }

  size_t getTerminalSizeCol(){
    return w.ws_col;
  }

};

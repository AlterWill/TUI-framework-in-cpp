#include "terminal.hpp"
#include "tools.hpp"

int main() {
  terminal Frame;
  tools::invisiableCursor();
  for (;;) {
    tools::cursorHomePosition();
    Frame.measurements();
    Frame.createScreen();
    Frame.drawBox(boxStyle::heavy, {1, 1, 10, 15});
    Frame.drawBox(boxStyle::light, {0, 0, Frame.row, Frame.col});
    Frame.drawText(2, 2, "Hello World");
    Frame.display();
  }
  return 0;
}

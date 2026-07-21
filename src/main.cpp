#include <unistd.h>

#include <iostream>
#include <memory>

#include "ColumnContainer.hpp"
#include "GridContainer.hpp"
#include "box.hpp"
#include "linux_backend.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widgetTree.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  linux_backend terminal(true, true);

  auto column = std::make_unique<Column>();

  column->addChild(
      std::make_unique<Text>(
          "TUI Library Demo",
          Style{.colours = {.fg = NamedColour::Cyan}, .textStyle = static_cast<uint8_t>(TextStyle::Bold)},
          Alignment::center
      )
  );

  column->addChild(std::make_unique<Text>("Keyboard + Mouse enabled", Style{}, Alignment::center));

  auto rootBox = std::make_unique<Box>(std::move(column), boxStyle::rounded, ColourPair{.fg = NamedColour::Green});

  tools::alternateScreenBuffer();
  tools::clearScreen();
  tools::invisiableCursor();

  WidgetTree tree(std::move(rootBox), terminal);

  bool running = true;

  while (running) {
    tools::cursorHomePosition();

    tree.fb.resizeBuffer();

    tree.layout({100, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});

    tree.render();

    tree.display();

    auto event = terminal.readEvent();

    if (event) {
      if (auto key = std::get_if<keyEvent>(&event.value())) {
        if (key->key == 'q') running = false;
      }

      if (auto mouse = std::get_if<MouseEvent>(&event.value())) {
        std::cout.flush();

        // temporary debug
        // remove later
        std::cerr << "mouse " << mouse->x << "," << mouse->y << "\n";
      }
    }

    usleep(50000);
  }

  tools::visiableCursor();
  tools::clearScreen();
  std::cout << "\x1b[?1049l" << std::flush;

  return 0;
}

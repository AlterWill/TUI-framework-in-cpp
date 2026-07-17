#include "react_dsl.hpp"

/*
 int main() {
  run_app(
        box(grid(
                rows = 2,
                cols = 2,
                box(column(
                        text("SYSTEM OVERVIEW", Style{.fg = AnsiColor::BrightGreen}, Alignment::center),
                        text("OS: Linux TUI-OS", Alignment::center),
                        text("Uptime: 2h 45m", Alignment::center),
                        text("Status: ONLINE", Style{.fg = AnsiColor::BrightGreen}, Alignment::center)
                    ),
                    boxStyle::rounded),
                box(column(
                        text("CPU MONITOR", Style{.fg = AnsiColor::BrightCyan}, Alignment::center),
                        text("Usage: [||||||||||          ] 50%", Alignment::center),
                        text("Cores: 8 Cores @ 3.6 GHz", Alignment::center),
                        text("Temp: 45 C (OK)", Alignment::center)
                    ),
                    boxStyle::heavy),
                box(column(
                        text("MEMORY & SWAP", Style{.fg = AnsiColor::BrightYellow}, Alignment::center),
                        text("RAM:  [|||||||||||         ] 55%", Alignment::center),
                        text("Used: 17.6 GB / 32.0 GB", Alignment::center),
                        text("Swap: 2.1 GB / 8.0 GB", Alignment::center)
                    ),
                    boxStyle::doubleBorder),
                box(column(
                        text("I/O & NETWORK", Style{.fg = AnsiColor::BrightMagenta}, Alignment::center),
                        text("Net Rx: 4.8 MB/s", Alignment::center),
                        text("Net Tx: 1.2 MB/s", Alignment::center),
                        text("Disk Write: 80 MB/s", Alignment::center)
                    ),
                    boxStyle::dashed)
            ),
            boxStyle::block)
    );
    return 0;
}
  */
/*
Example of how to construct and execute the same TUI application
using explicit widget instantiation and a manual run-loop without templates:

*/
int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);   
  // 1. Construct the widgets manually using constructors and addChild
  auto col1 = std::make_unique<Column>();
  col1->addChild(
      std::make_unique<Text>(
          "SYSTEM OVERVIEW",
          Style{
            .colour = {.fg = AnsiColor::BrightGreen},
            .textStyle = static_cast<uint8_t>(TextStyle::Bold) | static_cast<uint8_t>(TextStyle::Italic) |
                           static_cast<uint8_t>(TextStyle::Underline)
          },
          Alignment::center
      )
  );
  col1->addChild(std::make_unique<Text>("OS: Linux TUI-OS", Style{}, Alignment::center));
  col1->addChild(std::make_unique<Text>("Uptime: 2h 45m", Style{}, Alignment::center));
  col1->addChild(
      std::make_unique<Text>(
          "Status: ONLINE",
          Style{.colour = {.fg = AnsiColor::BrightGreen},
              .textStyle = static_cast<uint8_t>(TextStyle::Bold) | static_cast<uint8_t>(TextStyle::Italic)},
          Alignment::center
      )
  );

  Colour c{ .fg = AnsiColor::Blue};
  auto box1 = std::make_unique<Box>(std::move(col1), boxStyle::rounded, c );

  auto col2 = std::make_unique<Column>();
  col2->addChild(
      std::make_unique<Text>(
          "CPU MONITOR",
          Style{.colour = {.fg = AnsiColor::BrightCyan},
              .textStyle = static_cast<uint8_t>(TextStyle::Bold) | static_cast<uint8_t>(TextStyle::Italic) |
                           static_cast<uint8_t>(TextStyle::Underline)},
          Alignment::center
      )
  );
  col2->addChild(std::make_unique<Text>("Usage: [||||||||||          ] 50%", Style{}, Alignment::center));
  col2->addChild(std::make_unique<Text>("Cores: 8 Cores @ 3.6 GHz", Style{}, Alignment::center));
  col2->addChild(std::make_unique<Text>("Temp: 45 C (OK)", Style{}, Alignment::center));
  auto box2 = std::make_unique<Box>(std::move(col2), boxStyle::heavy);

  auto col3 = std::make_unique<Column>();
  col3->addChild(
      std::make_unique<Text>(
          "MEMORY & SWAP",
          Style{.colour = {.fg = AnsiColor::Blue},
              .textStyle = static_cast<uint8_t>(TextStyle::Reverse) | static_cast<uint8_t>(TextStyle::Italic) |
                           static_cast<uint8_t>(TextStyle::Underline)},
          Alignment::center
      )
  );
  col3->addChild(std::make_unique<Text>("RAM:  [|||||||||||         ] 55%", Style{}, Alignment::center));
  col3->addChild(std::make_unique<Text>("Used: 17.6 GB / 32.0 GB", Style{}, Alignment::center));
  col3->addChild(
      std::make_unique<Text>(
          "Swap: 2.1 GB / 8.0 GB", Style{.colour = {.fg = AnsiColor::Cyan, .bg = AnsiColor::Yellow}}, Alignment::center
      )
  );
  auto box3 = std::make_unique<Box>(std::move(col3), boxStyle::doubleBorder);

  auto col4 = std::make_unique<Column>();
  col4->addChild(
      std::make_unique<Text>(
          "I/O & NETWORK",
          Style{.colour = {.fg = AnsiColor::Cyan},
              .textStyle = static_cast<uint8_t>(TextStyle::Bold) | static_cast<uint8_t>(TextStyle::Italic) |
                           static_cast<uint8_t>(TextStyle::Underline)},
          Alignment::center
      )
  );
  col4->addChild(std::make_unique<Text>("Net Rx: 4.8 MB/s", Style{}, Alignment::center));
  col4->addChild(std::make_unique<Text>("Net Tx: 1.2 MB/s", Style{}, Alignment::center));
  col4->addChild(std::make_unique<Text>("Disk Write: 80 MB/s", Style{}, Alignment::center));
  auto box4 = std::make_unique<Box>(std::move(col4), boxStyle::dashed, Colour{ .fg = AnsiColor::Yellow, .bg = AnsiColor::Black});

  auto gridContainer = std::make_unique<Grid>(2, 2);
  gridContainer->addChild(std::move(box1));
  gridContainer->addChild(std::move(box2));
  gridContainer->addChild(std::move(box3));
  gridContainer->addChild(std::move(box4));

  auto rootBox = std::make_unique<Box>(std::move(gridContainer), boxStyle::block, Colour{ .fg = AnsiColor::Green});

  // 2. Initialize the application runner state manually
  tools::invisiableCursor();
  tools::clearScreen();

  // 3. Create the WidgetTree and run the event loop
  WidgetTree tree(std::move(rootBox));

  for (;;) {
    tools::alternateScreenBuffer();
    tools::cursorHomePosition();
    tree.fb.resizeBuffer();
    tree.layout(Rect{0, 0, tree.fb.row, tree.fb.col});
    tree.render();
    tree.display();
    usleep(100000);
  }

  tools::visiableCursor();
  return 0;
}

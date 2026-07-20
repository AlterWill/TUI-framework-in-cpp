#include <iostream>
#include <memory>
#include <unistd.h>

#include "box.hpp"
#include "ColumnContainer.hpp"
#include "GridContainer.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widgetTree.hpp"

/*
 * TUI Showcase Program
 * This program demonstrates all the core features of the TUI library:
 * - Layouts: Grid, Column
 * - Widgets: Box, Text
 * - Styling: Foreground & Background colours using ColourPair, Text styles (Bold, Italic, Reverse)
 * - Borders: Various box outlines
 * 
 * Note: Manual widget instantiation is used here because the react_dsl 
 * BoxBuilder currently does not handle ColourPair attributes.
 */
int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);

  // --- WIDGET 1: System Overview ---
  auto col1 = std::make_unique<Column>();
  col1->addChild(std::make_unique<Text>(
      "SYSTEM OVERVIEW",
      Style{
          .colours = {.fg = NamedColour::Cyan},
          .textStyle = static_cast<uint8_t>(TextStyle::Bold) | static_cast<uint8_t>(TextStyle::Underline)
      },
      Alignment::center
  ));
  col1->addChild(std::make_unique<Text>("OS: Linux TUI-OS", Style{}, Alignment::center));
  col1->addChild(std::make_unique<Text>("Uptime: 2h 45m", Style{}, Alignment::center));
  col1->addChild(std::make_unique<Text>(
      "Status: ONLINE",
      Style{
          .colours = {.fg = NamedColour::Green},
          .textStyle = static_cast<uint8_t>(TextStyle::Bold) | static_cast<uint8_t>(TextStyle::Blink)
      },
      Alignment::center
  ));
  // Box with rounded outline and Cyan border
  auto box1 = std::make_unique<Box>(std::move(col1), boxStyle::rounded, ColourPair{.fg = NamedColour::Cyan});


  // --- WIDGET 2: CPU Monitor ---
  auto col2 = std::make_unique<Column>();
  col2->addChild(std::make_unique<Text>(
      "CPU MONITOR",
      Style{
          .colours = {.fg = NamedColour::Magenta},
          .textStyle = static_cast<uint8_t>(TextStyle::Bold)
      },
      Alignment::center
  ));
  col2->addChild(std::make_unique<Text>("Usage: [||||||||||          ] 50%", Style{}, Alignment::center));
  col2->addChild(std::make_unique<Text>("Cores: 8 Cores @ 3.6 GHz", Style{}, Alignment::center));
  col2->addChild(std::make_unique<Text>("Temp: 45 C (OK)", Style{.colours = {.fg = NamedColour::Yellow}}, Alignment::center));
  
  // Box with heavy outline and Magenta border
  auto box2 = std::make_unique<Box>(std::move(col2), boxStyle::heavy, ColourPair{.fg = NamedColour::Magenta});


  // --- WIDGET 3: Memory & Swap ---
  auto col3 = std::make_unique<Column>();
  col3->addChild(std::make_unique<Text>(
      "MEMORY & SWAP",
      Style{
          .colours = {.fg = NamedColour::Black, .bg = NamedColour::Yellow},
          .textStyle = static_cast<uint8_t>(TextStyle::Bold)
      },
      Alignment::center
  ));
  col3->addChild(std::make_unique<Text>("RAM:  [|||||||||||         ] 55%", Style{}, Alignment::center));
  col3->addChild(std::make_unique<Text>("Used: 17.6 GB / 32.0 GB", Style{}, Alignment::center));
  col3->addChild(std::make_unique<Text>(
      "Swap: 2.1 GB / 8.0 GB", 
      Style{.colours = {.fg = NamedColour::Red}, .textStyle = static_cast<uint8_t>(TextStyle::Reverse)}, 
      Alignment::center
  ));
  
  // Box with double border and Yellow border colour
  auto box3 = std::make_unique<Box>(std::move(col3), boxStyle::doubleBorder, ColourPair{.fg = NamedColour::Yellow});


  // --- WIDGET 4: I/O & Network ---
  auto col4 = std::make_unique<Column>();
  col4->addChild(std::make_unique<Text>(
      "I/O & NETWORK",
      Style{
          .colours = {.fg = NamedColour::Blue},
          .textStyle = static_cast<uint8_t>(TextStyle::Bold) | static_cast<uint8_t>(TextStyle::Italic)
      },
      Alignment::center
  ));
  col4->addChild(std::make_unique<Text>("Net Rx: 4.8 MB/s", Style{}, Alignment::center));
  col4->addChild(std::make_unique<Text>("Net Tx: 1.2 MB/s", Style{}, Alignment::center));
  col4->addChild(std::make_unique<Text>("Disk Write: 80 MB/s", Style{}, Alignment::center));
  
  // Box with dashed outline and Blue border colour
  auto box4 = std::make_unique<Box>(std::move(col4), boxStyle::dashed, ColourPair{.fg = NamedColour::Blue});


  // --- ASSEMBLE GRID ---
  auto gridContainer = std::make_unique<Grid>(2, 2);
  gridContainer->addChild(std::move(box1));
  gridContainer->addChild(std::move(box2));
  gridContainer->addChild(std::move(box3));
  gridContainer->addChild(std::move(box4));

  // Root box holding everything, basic light border
  auto rootBox = std::make_unique<Box>(std::move(gridContainer), boxStyle::light, ColourPair{.fg = NamedColour::White});

  // --- RUN APP ---
  tools::alternateScreenBuffer();
  tools::invisiableCursor();
  tools::clearScreen();

  WidgetTree tree(std::move(rootBox));

  bool running = true;
  while (running) {
    tools::cursorHomePosition();
    tree.fb.resizeBuffer();
    
    // Layout and render
    tree.layout(Rect{0, 0, static_cast<size_t>(tree.fb.terminalData.row), static_cast<size_t>(tree.fb.terminalData.col)});
    tree.render();
    tree.display();
    
    int key = tree.fb.terminalData.readKey();
    if (key == 'q' || key == 'Q' || key == '\x1b') {
      running = false;
    }
  }

  tools::visiableCursor();
  tools::clearScreen();
  std::cout << "\x1b[?1049l" << std::flush;
  return 0;
}

#include <unistd.h>

#include <chrono>
#include <iostream>
#include <map>
#include <memory>

#include "ColumnContainer.hpp"
#include "linux_backend.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widgetTree.hpp"

struct AppData {
  std::string str{};
};

class Timer {
 public:
  using Clock = std::chrono::steady_clock;

  Timer() : start_(Clock::now()) {}

  [[nodiscard]]
  auto elapsed() const {
    return Clock::now() - start_;
  }

  [[nodiscard]]
  double elapsed_ms() const {
    return std::chrono::duration<double, std::milli>(elapsed()).count();
  }

 private:
  Clock::time_point start_;
};

std::vector<double> times;

int main() {
  Timer timer;
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  linux_backend terminal(true, true);

  // auto column = std::make_unique<Column>();

  /*column->addChild(
      std::make_unique<Text>(
          "TUI Library Demo",
          Style{.colours = {.fg = NamedColour::Cyan}, .textStyle = static_cast<uint8_t>(TextStyle::Bold)},
          Alignment::center
      )
  );

  column->addChild(std::make_unique<Text>("Keyboard + Mouse enabled", Style{}, Alignment::center));

  */
  // auto rootBox = std::make_unique<Box>(std::move(column), boxStyle::rounded, ColourPair{.fg = NamedColour::Green});

  tools::alternateScreenBuffer();
  tools::clearScreen();
  tools::invisiableCursor();

  AppData data{};
  auto textParagraph =
      std::make_unique<Text>(data.str, Style{.colours = {.fg = NamedColour::Orange}}, Alignment::center);

  WidgetTree tree(std::move(textParagraph), terminal);

  bool running = true;
  std::size_t limit = 5;
  std::size_t i = 0;

  times.push_back(timer.elapsed_ms());
  while (i < limit) {
    times.push_back(timer.elapsed_ms());
    data.str += "aaaa aaaaaaaa aaaaaaaaaaaa";
    tools::cursorHomePosition();
    times.push_back(timer.elapsed_ms());
    tree.fb.resizeBuffer();
    times.push_back(timer.elapsed_ms());
    tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});
    times.push_back(timer.elapsed_ms());
    tree.render();
    times.push_back(timer.elapsed_ms());
    tree.display();
    times.push_back(timer.elapsed_ms());

    auto event = terminal.readEvent();
    times.push_back(timer.elapsed_ms());

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

    times.push_back(timer.elapsed_ms());
    //usleep(10000);
    i++;
  }

  tools::visiableCursor();
  tools::clearScreen();
  std::cout << "\x1b[?1049l" << std::flush;

  tools::clearScreen();
  tools::cursorHomePosition();

  std::cout << "Before While :" << times[0] << '\n';
  for (i = 1; i < times.size(); i += 8) {
    std::cout << "After While " << (i - 1) / 6 << " : " << times[i] << '\n';
    std::cout << "Before Resize :" << times[i + 1] << '\n';
    std::cout << "After  Resize :" << times[i + 2] << '\n';
    std::cout << "layout        :" << times[i + 3] << '\n';
    std::cout << "render        :" << times[i + 4] << '\n';
    std::cout << "display       :" << times[i + 5] << '\n';
    std::cout << "event         :" << times[i + 6] << '\n';
    std::cout << "what event    :" << times[i + 7] << '\n';
  }
  return 0;
}

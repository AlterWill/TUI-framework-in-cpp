#include <unistd.h>

#include <chrono>
#include <iostream>
#include <memory>

#include "widgets/box.hpp"
#include "terminal/linux_backend.hpp"
#include "widgets/text.hpp"
#include "terminal/tools.hpp"
#include "core/widgetTree.hpp"

struct AppData {
  std::vector<std::u32string> lines{U"HHelloHelloHelloHelloHelloHelloHelloHelloHelloello"};
  std::string boxTitle{"what"};
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

  tools::alternateScreenBuffer();
  tools::clearScreen();
  tools::invisiableCursor();

  AppData data{};
  auto textParagraph = std::make_unique<Text>(data.lines, Style{.colours = {.fg = NamedColour::Aqua,.bg = NamedColour::RebeccaPurple}}, Alignment::left);
  textParagraph->setPadding(Insets{0,0,5,5});
  textParagraph->setMargin(Insets{5,5,0,0});

  auto box = std::make_unique<Box>(std::move(textParagraph),data.boxTitle,Alignment::right,false,boxStyle::heavy,ColourPair{.fg = NamedColour::Aqua,.bg = NamedColour::Grey});
  box->setPadding(Insets{0,5,0,0});
  box->setMargin(Insets{5,5,5,0});

  WidgetTree tree(std::move(box), terminal);

  bool running = true;
  times.push_back(timer.elapsed_ms());
  while (running) {
    times.push_back(timer.elapsed_ms());

    data.lines.push_back(U"HHelloHelloHelloHelloHelloHelloHelloHelloHelloello");
    tools::cursorHomePosition();
    times.push_back(timer.elapsed_ms());

    tree.fb.resizeBuffer();
    times.push_back(timer.elapsed_ms());

    tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});
    times.push_back(timer.elapsed_ms());

    tree.render();
    times.push_back(timer.elapsed_ms());

    //tree.display();
    tree.incrementDisplay();
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
  }

  tools::visiableCursor();
  tools::clearScreen();
  std::cout << "\x1b[?1049l" << std::flush;

  tools::clearScreen();
  tools::cursorHomePosition();

  std::cout << "Before While :" << times[0] << '\n';
  return 0;
}

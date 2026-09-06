#include <unistd.h>

#include <chrono>
#include <iostream>
#include <memory>

#include "widgets/box.hpp"
#include "terminal/linux_backend.hpp"
#include "widgets/text.hpp"
#include "terminal/tools.hpp"
#include "core/widgetTree.hpp"
#include "layout/RowContainer.hpp"

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

  // ─── Row Layout Test ───────────────────────────────────────────
  // Child 1: Fixed width=20, label "FIXED"
  std::vector<std::u32string> fixedLines{U"[ FIXED w=20 ]"};
  auto fixedText = std::make_unique<Text>(fixedLines);
  fixedText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Aqua}});

  // Child 2: Percentage width=25%, label "PCT 25%"
  std::vector<std::u32string> pctLines{U"[ PCT 25% ]"};
  auto pctText = std::make_unique<Text>(pctLines);
  pctText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Yellow}});

  // Child 3: Content (natural text width), label "CONTENT"
  std::vector<std::u32string> contentLines{U"[ CONTENT ]"};
  auto contentText = std::make_unique<Text>(contentLines);
  contentText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Green}});

  // Child 4: Flex=1 (takes remaining space), label "FLEX fills rest"
  std::vector<std::u32string> flexLines{U"[ FLEX: fills remaining space ]"};
  auto flexText = std::make_unique<Text>(flexLines);
  flexText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::RebeccaPurple}});

  auto row = std::make_unique<Row>();
  row->withGap(1)
     .withPadding(Insets{1, 1, 1, 1})
     .addChild(std::move(fixedText),   SizeSpec{SizeType::Fixed,      20, {}}, SizeSpec{})
     .addChild(std::move(pctText),     SizeSpec{SizeType::Percentage, 25, {}}, SizeSpec{})
     .addChild(std::move(contentText), SizeSpec{},                             SizeSpec{})
     .addChild(std::move(flexText),    SizeSpec{SizeType::Flex,        1, {}}, SizeSpec{});

  WidgetTree tree(std::move(row), terminal);

  bool running = true;
  times.push_back(timer.elapsed_ms());
  while (running) {
    times.push_back(timer.elapsed_ms());

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

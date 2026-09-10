#include <chrono>
#include <iostream>
#include <memory>

#include "animation/animationManager.hpp"
#include "core/widgetTree.hpp"
#include "layout/ColumnContainer.hpp"
#include "layout/GridContainer.hpp"
#include "layout/Overlay.hpp"
#include "layout/RowContainer.hpp"
#include "layout/ScrollContainer.hpp"
#include "layout/SplitPane.hpp"
#include "layout/StackContainer.hpp"
#include "terminal/linux_backend.hpp"
#include "terminal/tools.hpp"
#include "widgets/box.hpp"
#include "widgets/singleCharSpinner.hpp"
#include "widgets/text.hpp"

// ──────────────────────────────────────────────────────────────────────────────
// App state: owns all string data persistent for the lifetime of UI
// ──────────────────────────────────────────────────────────────────────────────

struct AppState {
  std::vector<std::u32string> titleLines{U"TUI Library Demo"};
  std::vector<std::u32string> subtitleLines{U"SplitPane / Overlay / Row / Column / Box / Grid / Stack / Scroll"};

  std::vector<std::u32string> aLine1{U"Widget A-1"};
  std::vector<std::u32string> aLine2{U"Widget A-2"};
  std::vector<std::u32string> aLine3{U"Widget A-3"};

  std::vector<std::u32string> g00{U"Grid[0,0]"};
  std::vector<std::u32string> g01{U"Grid[0,1]"};
  std::vector<std::u32string> g10{U"Grid[1,0]"};
  std::vector<std::u32string> g11{U"Grid[1,1]"};

  std::vector<std::u32string> bgLines{U"                    "};
  std::vector<std::u32string> overlayLines{U"Stack overlay!"};

  std::vector<std::u32string> splitLeftLines{U"Left pane"};
  std::vector<std::u32string> splitRightLines{U"Right pane"};

  std::vector<std::u32string> overlayMsgLines{U"=== MODAL POPUP ==="};
  std::vector<std::u32string> overlayHintLines{U"Click outside to dismiss"};

  std::vector<std::u32string> scrollLines;
  std::vector<std::u32string> footerLines{U"Press 'q' to quit | Scroll: mouse wheel | Drag divider to resize"};

  AppState() {
    for (unsigned int i = 1; i <= 4000; ++i) {
      scrollLines.push_back(U"  Line " + std::u32string(1, static_cast<char32_t>(U'0' + (i / 10u % 10u))) +
                            std::u32string(1, static_cast<char32_t>(U'0' + (i % 10u))) + U": Hello from the scroll view!");
    }
  }
};

// ──────────────────────────────────────────────────────────────────────────────
// Build UI
// ──────────────────────────────────────────────────────────────────────────────

// Creates a spinner registered with the animation manager so it can be
// placed in any column/row and still animate.
std::unique_ptr<singleCharSpinner> makeSpinner(AnimationManager& animations,
                                               singleSpinnerData style = singleSpinnerStyle::braille) {
  auto spinner = std::make_unique<singleCharSpinner>(style);
  spinner->style.setColours(ColourPair{
    .fg = NamedColour::DarkRed,
    .bg = NamedColour::DarkMagenta
  });
  animations.add(spinner.get());
  return spinner;
}

std::unique_ptr<Widget> buildUI(AppState& state, AnimationManager& animations) {

  // ── Section 1: Header row with three Fixed/Percentage/Flex columns ──────────

  auto title = std::make_unique<Text>(state.titleLines);
  title->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
      .withAlignment(HorizontalAlignment::Center)
      .withPadding(Insets{0, 0, 1, 1});

  auto subtitle = std::make_unique<Text>(state.subtitleLines);
  subtitle->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Navy}})
      .withAlignment(HorizontalAlignment::Center)
      .withPadding(Insets{0, 0, 1, 1});

  auto headerCol = std::make_unique<Column>();
  headerCol
      ->withPadding(Insets{0, 0, 0, 0})
      .addChild(std::move(title),    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}})
      .addChild(std::move(subtitle), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});

  auto headerBox = std::make_unique<Box>(std::move(headerCol));
  headerBox->withOutline(boxStyle::heavy)
      .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Navy});

  // ── Section 2: Main body — Row with three panels ────────────────────────────

  // Panel A: Column of boxed text labels
  auto ta1 = std::make_unique<Text>(state.aLine1);
  ta1->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Green}})
     .withPadding(Insets{0, 0, 1, 1});

  auto ta2 = std::make_unique<Text>(state.aLine2);
  ta2->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Yellow}})
     .withPadding(Insets{0, 0, 1, 1});

  auto ta3 = std::make_unique<Text>(state.aLine3);
  ta3->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Orange}})
     .withPadding(Insets{0, 0, 1, 1});

  auto colA = std::make_unique<Column>();
  colA->withGap(1)
      .withPadding(Insets{1, 1, 1, 1})
      .addChild(std::move(ta1), SizeSpec{SizeType::Content, 0, {}})
      .addChild(std::move(ta2), SizeSpec{SizeType::Content, 0, {}})
      .addChild(std::move(ta3), SizeSpec{SizeType::Content, 0, {}});

  auto boxA = std::make_unique<Box>(std::move(colA));
  boxA->withTitle("Column", HorizontalAlignment::Left)
      .withOutline(boxStyle::rounded)
      .withColours(ColourPair{.fg = NamedColour::Green, .bg = NamedColour::Black});

  // Panel B: 2x2 Grid
  auto tg00 = std::make_unique<Text>(state.g00);
  tg00->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::DarkRed}})
      .withAlignment(HorizontalAlignment::Center)
      .withPadding(Insets{0, 0, 1, 1});

  auto tg01 = std::make_unique<Text>(state.g01);
  tg01->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::DarkGreen}})
      .withAlignment(HorizontalAlignment::Center)
      .withPadding(Insets{0, 0, 1, 1});

  auto tg10 = std::make_unique<Text>(state.g10);
  tg10->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::DarkRed}})
      .withAlignment(HorizontalAlignment::Center)
      .withPadding(Insets{0, 0, 1, 1});

  auto tg11 = std::make_unique<Text>(state.g11);
  tg11->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::DarkRed}})
      .withAlignment(HorizontalAlignment::Center)
      .withPadding(Insets{0, 0, 1, 1});

  auto grid = std::make_unique<Grid<2, 2>>();
  grid->withGaps(1, 1)
      .withPadding(Insets{1, 1, 1, 1})
      .setCell(0, 0, std::move(tg00), HorizontalAlignment::Center, VerticalAlignment::Center)
      .setCell(0, 1, std::move(tg01), HorizontalAlignment::Center, VerticalAlignment::Center)
      .setCell(1, 0, std::move(tg10), HorizontalAlignment::Center, VerticalAlignment::Center)
      .setCell(1, 1, std::move(tg11), HorizontalAlignment::Center, VerticalAlignment::Center);

  auto boxB = std::make_unique<Box>(std::move(grid));
  boxB->withTitle("Grid<2,2>", HorizontalAlignment::Left)
      .withOutline(boxStyle::rounded)
      .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Black});

  // Panel C: Stack (label on top of a background)
  auto bgText = std::make_unique<Text>(state.bgLines);
  bgText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::DarkBlue}});

  auto overlayText = std::make_unique<Text>(state.overlayLines);
  overlayText->withStyle(Style{.colours = {.fg = NamedColour::Yellow, .bg = NamedColour::DarkBlue}})
             .withAlignment(HorizontalAlignment::Center);

  auto stackC = std::make_unique<Stack>();
  stackC->withPadding(Insets{1, 1, 1, 1})
        .addChild(std::move(bgText), HorizontalAlignment::Left, VerticalAlignment::Top)
        .addChild(std::move(overlayText), HorizontalAlignment::Center, VerticalAlignment::Center);

  auto boxC = std::make_unique<Box>(std::move(stackC));
  boxC->withTitle("Stack", HorizontalAlignment::Left)
      .withOutline(boxStyle::rounded)
      .withColours(ColourPair{.fg = NamedColour::Yellow, .bg = NamedColour::Black});

  // Main body row: Column(Fixed=30%) | Grid(Percentage=35%) | Stack(Flex=1)
  auto mainRow = std::make_unique<Row>();
  mainRow->withGap(1)
      .addChild(std::move(boxA), SizeSpec{SizeType::Percentage, 28, {}}, SizeSpec{SizeType::Flex, 1, {}})
      .addChild(std::move(boxB), SizeSpec{SizeType::Percentage, 35, {}}, SizeSpec{SizeType::Flex, 1, {}})
      .addChild(std::move(boxC), SizeSpec{SizeType::Flex,       1, {}},  SizeSpec{SizeType::Flex, 1, {}});

  auto mainRowBox = std::make_unique<Box>(std::move(mainRow));
  mainRowBox->withTitle("Row (Column | Grid | Stack)")
      .withOutline(boxStyle::light)
      .withColours(ColourPair{.fg = NamedColour::Silver, .bg = NamedColour::Black});

  // ── Section 2b: SplitPane demo ──────────────────────────────────────────────

  auto splitLeftText = std::make_unique<Text>(state.splitLeftLines);
  splitLeftText->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::DarkCyan}})
                .withAlignment(HorizontalAlignment::Center)
                .withPadding(Insets{0, 0, 1, 1});

  auto splitRightText = std::make_unique<Text>(state.splitRightLines);
  splitRightText->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::DarkMagenta}})
                  .withAlignment(HorizontalAlignment::Center)
                  .withPadding(Insets{0, 0, 1, 1});

  auto splitPane = std::make_unique<SplitPane>(std::move(splitLeftText), std::move(splitRightText),
                                               Orientation::Horizontal);
  splitPane->withSplitRatio(0.5)
            .withDividerGlyph(dividerStyle::vertical::doubleBorder)
            .withDividerColours(ColourPair{.fg = NamedColour::White, .bg = NamedColour::Black})
            .withPadding(Insets{1, 1, 1, 1});

  auto splitBox = std::make_unique<Box>(std::move(splitPane));
  splitBox->withTitle("SplitPane (drag divider)")
      .withOutline(boxStyle::heavy)
      .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Black});

  // ── Section 3: Scroll container with a long text list ───────────────────────

  auto scrollContent = std::make_unique<Text>(state.scrollLines);
  scrollContent->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Grey}})
               .withPadding(Insets{0, 0, 1, 1});

  auto scroll = std::make_unique<Scroll>(std::move(scrollContent), ScrollDirection::Vertical);

  auto scrollBox = std::make_unique<Box>(std::move(scroll));
  scrollBox->withTitle("Scroll (mouse wheel to scroll)", HorizontalAlignment::Left)
      .withOutline(boxStyle::light)
      .withColours(ColourPair{.fg = NamedColour::Silver, .bg = NamedColour::Black});

  // ── Section 4: Footer status row ────────────────────────────────────────────

  auto footerText = std::make_unique<Text>(state.footerLines);
  footerText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Silver}})
            .withAlignment(HorizontalAlignment::Center);

  // spinner (owned by the tree, animated by the manager)
  auto spinner = makeSpinner(animations);

  // ── Root Column: header | body row | split pane | scroll | spinner | footer ──

  auto root = std::make_unique<Column>();
  root->withGap(0)
      .addChild(std::move(headerBox), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}})
      .addChild(std::move(mainRowBox),   SizeSpec{SizeType::Fixed,   10, {}}, SizeSpec{SizeType::Flex, 1, {}})
      .addChild(std::move(splitBox), SizeSpec{SizeType::Fixed, 6, {}},  SizeSpec{SizeType::Flex, 1, {}})
      .addChild(std::move(scrollBox), SizeSpec{SizeType::Flex,    1, {}},  SizeSpec{SizeType::Flex, 1, {}})
      .addChild(std::move(spinner), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}})
      .addChild(std::move(footerText),SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});

  // ── Wrap in Overlay with a modal popup ──────────────────────────────────────

  auto overlayMsg = std::make_unique<Text>(state.overlayMsgLines);
  overlayMsg->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Red}})
             .withAlignment(HorizontalAlignment::Center)
             .withPadding(Insets{1, 1, 1, 1});

  auto overlayHint = std::make_unique<Text>(state.overlayHintLines);
  overlayHint->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::DarkRed}})
              .withAlignment(HorizontalAlignment::Center)
              .withPadding(Insets{0, 0, 1, 1});

  auto overlayCol = std::make_unique<Column>();
  overlayCol->withGap(0)
             .addChild(std::move(overlayMsg), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}})
             .addChild(std::move(overlayHint), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});

  auto overlayBox = std::make_unique<Box>(std::move(overlayCol));
  overlayBox->withTitle("Modal Overlay")
      .withOutline(boxStyle::doubleBorder)
      .withColours(ColourPair{.fg = NamedColour::White, .bg = NamedColour::Red});

  auto overlay = std::make_unique<Overlay>();
  overlay->withPadding(Insets{0, 0, 0, 0})
          .setBaseLayer(std::move(root))
          .withTransparent(false)
          .addOverlay(std::move(overlayBox), true, true);

  return overlay;
}

// ──────────────────────────────────────────────────────────────────────────────
// Main loop
// ──────────────────────────────────────────────────────────────────────────────

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  linux_backend terminal(true, true);

  tools::alternateScreenBuffer();
  tools::clearScreen();
  tools::invisiableCursor();

  AppState state;
  AnimationManager animationManager;

  WidgetTree tree(buildUI(state, animationManager), terminal);

  bool running = true;
  while (running) {
    tools::cursorHomePosition();

    tree.fb.resizeBuffer();
    tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});
    animationManager.update(std::chrono::steady_clock::now());
    tree.render();
    tree.incrementDisplay();

    auto event = terminal.readEvent();
    if (event) {
      if (auto key = std::get_if<keyEvent>(&event.value())) {
        if (key->key == 'q') running = false;
      }

      // Forward mouse scroll events to the tree root (scroll widget handles it)
      if (auto mouse = std::get_if<MouseEvent>(&event.value())) {
        tree.root.widget->handleEvent(event.value());
        (void)mouse;
      }
    }
  }

  tools::visiableCursor();
  tools::clearScreen();
  std::cout << "\x1b[?1049l" << std::flush;

  return 0;
}

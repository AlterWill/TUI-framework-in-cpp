#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <unistd.h>

#include "animation/animationManager.hpp"
#include "core/widgetTree.hpp"
#include "input/eventHandler.hpp"
#include "layout/ColumnContainer.hpp"
#include "layout/RowContainer.hpp"
#include "terminal/linux_backend.hpp"
#include "utilities/boxStyle.hpp"
#include "utilities/checkboxStyle.hpp"
#include "utilities/dividerStyle.hpp"
#include "utilities/progressBarStyles.hpp"
#include "utilities/singleSpinnerStyle.hpp"
#include "utilities/sliderDrag.hpp"
#include "utilities/sliderStyle.hpp"
#include "utilities/validators.hpp"
#include "widgets/box.hpp"
#include "widgets/button.hpp"
#include "widgets/checkbox.hpp"
#include "widgets/divider.hpp"
#include "widgets/interactiveSlider.hpp"
#include "widgets/progressBar.hpp"
#include "widgets/radioGroup.hpp"
#include "widgets/singleCharSpinner.hpp"
#include "widgets/spacer.hpp"
#include "widgets/text.hpp"
#include "widgets/textInput.hpp"
#include "widgets/toggle.hpp"

// ──────────────────────────────────────────────────────────────────────────────
// Helpers for String Formatting (UTF-32 for Text widgets)
// ──────────────────────────────────────────────────────────────────────────────

std::u32string formatDouble(double value) {
  char buffer[32];
  std::snprintf(buffer, sizeof buffer, "%.2f", value);
  std::u32string result;
  for (char c : buffer) {
    if (c == '\0') break;
    result.push_back(static_cast<char32_t>(static_cast<unsigned char>(c)));
  }
  return result;
}

std::u32string u32Bool(bool value) { return value ? U"ON" : U"OFF"; }

std::u32string formatUnsigned(unsigned long value) {
  char buffer[32];
  std::snprintf(buffer, sizeof buffer, "%lu", value);
  std::u32string result;
  for (char c : buffer) {
    if (c == '\0') break;
    result.push_back(static_cast<char32_t>(static_cast<unsigned char>(c)));
  }
  return result;
}

// Custom non-abstract Spacer implementing layout measurement and no-op render
struct CustomSpacer : Widget {
  Size measure(const SizeConstraints& constraints) override {
    return Size{constraints.getMinHeight(), constraints.getMinWidth()};
  }
  void render(RenderContext&) override {}
};

// ──────────────────────────────────────────────────────────────────────────────
// Interactive Slider with keyboard and mouse hit-testing / drag support
// ──────────────────────────────────────────────────────────────────────────────

struct DemoSlider : interactiveSlider {
  Rect myRect{};
  bool dragging{false};

  DemoSlider() = default;
  explicit DemoSlider(double initialValue) : interactiveSlider(initialValue) {}

  void layout(const Rect& rect) override {
    myRect = rect;
  }

  bool handleEvent(const Event& event) override {
    if (const auto* key = std::get_if<keyEvent>(&event)) {
      if (isFocused()) {
        const double step = (data.maxValue - data.minValue) / 20.0;
        if (key->key == ARROW_RIGHT || key->key == ARROW_UP) {
          data.value = std::clamp(data.value + step, data.minValue, data.maxValue);
          return true;
        }
        if (key->key == ARROW_LEFT || key->key == ARROW_DOWN) {
          data.value = std::clamp(data.value - step, data.minValue, data.maxValue);
          return true;
        }
      }
      return false;
    }

    if (const auto* mouse = std::get_if<MouseEvent>(&event)) {
      if (mouse->action == MouseAction::Press || mouse->action == MouseAction::Drag) {
        if (dragging || myRect.contains(mouse->x, mouse->y)) {
          dragging = (mouse->action == MouseAction::Press);
          data.value = sliderValueFromMouse(mouse->x, mouse->y, myRect, orientation,
                                           data.minValue, data.maxValue,
                                           padding.left, padding.top,
                                           padding.right, padding.bottom);
          return true;
        }
      }
      if (mouse->action == MouseAction::Release) {
        dragging = false;
      }
    }
    return false;
  }
};

// ──────────────────────────────────────────────────────────────────────────────
// Application State (owns all strings and persistent data)
// ──────────────────────────────────────────────────────────────────────────────

struct AppState {
  std::vector<std::u32string> titleLines{U"TUI Framework — Interactive Showcase & Demo"};

  // Labels for basic & interactive widgets
  std::vector<std::u32string> spinnerLabel{U"Spinners:"};
  std::vector<std::u32string> progressLabel{U"Progress:"};
  std::vector<std::u32string> checkSquareLabel{U"Opt A"};
  std::vector<std::u32string> checkCheckLabel{U"Opt B"};
  std::vector<std::u32string> toggleLabel{U"Toggle:"};
  std::vector<std::u32string> radioTitle{U"Radio:"};
  std::vector<std::u32string> radioOpt0Label{U"Low"};
  std::vector<std::u32string> radioOpt1Label{U"Med"};
  std::vector<std::u32string> radioOpt2Label{U"High"};
  std::vector<std::u32string> sliderLabel{U"Slider:"};

  // Labels for form input widgets
  std::vector<std::u32string> userLabel{U"User:"};
  std::vector<std::u32string> passLabel{U"Pass:"};
  std::vector<std::u32string> ageLabel{U"Age: "};
  std::vector<std::u32string> bioLabel{U"Bio:"};

  // Form input values
  std::string username{"developer"};
  std::string password{"secret123"};
  std::string age{"25"};
  std::string bio{"C++20 Terminal Framework\nRetained-mode layout\nKeyboard & Mouse support"};

  // Controls state
  bool checkSquare{true};
  bool checkCheckmark{false};
  bool toggleSwitchState{true};
  bool radio0{false};
  bool radio1{true};
  bool radio2{false};

  float progress{35.0f};
  unsigned int clickCount{0};

  // Live status and help text
  std::vector<std::u32string> statusLines{
      U"Clicks: 0 | Radio: 1 | Checkboxes: [ON, OFF] | Toggle: ON | Slider: 0.35",
      U"Input Validation: [User: ✓, Pass: ✓, Age: ✓] | Bio: 3 lines"};

  std::vector<std::u32string> footerLines{
      U"[Tab / Shift+Tab] Focus   [Enter / Space] Toggle   [← / → / Drag] Slider   [Esc] Unfocus   [q] Quit"};
};

// ──────────────────────────────────────────────────────────────────────────────
// UI Tree Construction
// ──────────────────────────────────────────────────────────────────────────────

std::unique_ptr<singleCharSpinner> makeSpinner(AnimationManager& animations,
                                               singleSpinnerData style = singleSpinnerStyle::braille,
                                               Colour fg = NamedColour::Cyan) {
  auto spinner = std::make_unique<singleCharSpinner>(style);
  spinner->withColours(ColourPair{.fg = fg, .bg = NamedColour::Black});
  animations.add(spinner.get());
  return spinner;
}

std::unique_ptr<Widget> buildUI(AppState& state, AnimationManager& animations,
                                DemoSlider*& outSlider, radioGroup<Row>*& outRadio,
                                TextInput*& outUserInput, TextInput*& outAgeInput) {
  // ── Header Box ─────────────────────────────────────────────────────────────
  auto title = std::make_unique<Text>(state.titleLines);
  title->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
        .withAlignment(HorizontalAlignment::Center);

  auto headerBox = std::make_unique<Box>(std::move(title));
  headerBox->withTitle(" TUI Library Showcase ", HorizontalAlignment::Center)
           .withOutline(boxStyle::heavy)
           .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Navy});

  // ── Left Column: Basic & Interactive Controls (Phase 5 & 6) ────────────────
  // 1. Spinners Row
  auto spinLbl = std::make_unique<Text>(state.spinnerLabel);
  spinLbl->withStyle(Style{.colours = {.fg = NamedColour::Yellow, .bg = NamedColour::Black}});

  auto spinRow = std::make_unique<Row>();
  spinRow->withGap(2);
  spinRow->addChild(std::move(spinLbl), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  spinRow->addChild(makeSpinner(animations, singleSpinnerStyle::braille, NamedColour::Cyan),
                    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  spinRow->addChild(makeSpinner(animations, singleSpinnerStyle::circle, NamedColour::Green),
                    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  spinRow->addChild(makeSpinner(animations, singleSpinnerStyle::arrows, NamedColour::Yellow),
                    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  spinRow->addChild(makeSpinner(animations, singleSpinnerStyle::dots, NamedColour::Magenta),
                    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  spinRow->addChild(makeSpinner(animations, singleSpinnerStyle::blocks, NamedColour::White),
                    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 2. Progress Bar Row
  auto progLbl = std::make_unique<Text>(state.progressLabel);
  progLbl->withStyle(Style{.colours = {.fg = NamedColour::Yellow, .bg = NamedColour::Black}});

  auto bar = std::make_unique<progressBar>(state.progress);
  bar->colours = {NamedColour::Red, NamedColour::Yellow, NamedColour::Green};
  bar->background = NamedColour::Black;
  bar->progressBarStyle = progressBarStyle::block;

  auto progRow = std::make_unique<Row>();
  progRow->withGap(1);
  progRow->addChild(std::move(progLbl), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  progRow->addChild(std::move(bar), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 3. Buttons Row
  auto incBtn = std::make_unique<button>();
  incBtn->text = U" [ Click +1 ] ";
  incBtn->style = Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Green}};
  incBtn->hAlignment = HorizontalAlignment::Center;
  incBtn->vAlignment = VerticalAlignment::Center;
  incBtn->onClick = [&state]() {
    ++state.clickCount;
  };

  auto resetBtn = std::make_unique<button>();
  resetBtn->text = U" [ Reset ] ";
  resetBtn->style = Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::DarkRed}};
  resetBtn->hAlignment = HorizontalAlignment::Center;
  resetBtn->vAlignment = VerticalAlignment::Center;
  resetBtn->onClick = [&state]() {
    state.clickCount = 0;
  };

  auto btnRow = std::make_unique<Row>();
  btnRow->withGap(2);
  btnRow->addChild(std::move(incBtn), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  btnRow->addChild(std::move(resetBtn), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 4. Checkboxes & Toggle Row
  auto cb1 = std::make_unique<checkbox>(checkboxStyle::square);
  cb1->withState(state.checkSquare)
     .withPrefix("[")
     .withSuffix("]")
     .withLabel(" Opt A")
     .withChecked(true)
     .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Black});

  auto cb2 = std::make_unique<checkbox>(checkboxStyle::check);
  cb2->withState(state.checkCheckmark)
     .withPrefix("[")
     .withSuffix("]")
     .withLabel(" Opt B")
     .withChecked(false)
     .withColours(ColourPair{.fg = NamedColour::Green, .bg = NamedColour::Black});

  auto tog = std::make_unique<toggle>(state.toggleSwitchState);
  tog->withPrefix("Toggle: [")
     .withSuffix("]")
     .withColours(ColourPair{.fg = NamedColour::Yellow, .bg = NamedColour::Black});

  auto checkRow = std::make_unique<Row>();
  checkRow->withGap(2);
  checkRow->addChild(std::move(cb1), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  checkRow->addChild(std::move(cb2), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  checkRow->addChild(std::move(tog), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 5. Radio Group Row
  auto radTitle = std::make_unique<Text>(state.radioTitle);
  radTitle->withStyle(Style{.colours = {.fg = NamedColour::Yellow, .bg = NamedColour::Black}});

  auto radio = std::make_unique<radioGroup<Row>>();
  auto makeRadioOpt = [](bool& bound, std::string_view label) {
    auto opt = std::make_unique<checkbox>(checkboxStyle::circle);
    opt->withState(bound)
       .withPrefix("(")
       .withSuffix(")")
       .withLabel(label)
       .withColours(ColourPair{.fg = NamedColour::Cyan, .bg = NamedColour::Black});
    return opt;
  };
  radio->withGap(2);
  radio->addChild(makeRadioOpt(state.radio0, " Low"), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  radio->addChild(makeRadioOpt(state.radio1, " Med"), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  radio->addChild(makeRadioOpt(state.radio2, " High"), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  radio->withSelected(1);
  outRadio = radio.get();

  auto radRow = std::make_unique<Row>();
  radRow->withGap(1);
  radRow->addChild(std::move(radTitle), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  radRow->addChild(std::move(radio), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 6. Slider Row
  auto slidLbl = std::make_unique<Text>(state.sliderLabel);
  slidLbl->withStyle(Style{.colours = {.fg = NamedColour::Yellow, .bg = NamedColour::Black}});

  auto sliderWidget = std::make_unique<DemoSlider>(0.35);
  sliderWidget->withOrientation(Orientation::Horizontal)
              .withRange(0.0, 1.0)
              .withStyle(sliderStyle::arrows)
              .withThumbColours(ColourPair{.fg = NamedColour::Yellow, .bg = NamedColour::Black})
              .withTrackColours(ColourPair{.fg = NamedColour::White, .bg = NamedColour::Black});
  sliderWidget->padding = Insets{0, 0, 1, 1};
  outSlider = sliderWidget.get();

  auto slidRow = std::make_unique<Row>();
  slidRow->withGap(1);
  slidRow->addChild(std::move(slidLbl), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  slidRow->addChild(std::move(sliderWidget), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Content, 0, {}});

  auto leftCol = std::make_unique<Column>();
  leftCol->withGap(1);
  leftCol->addChild(std::move(spinRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  leftCol->addChild(std::move(progRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  leftCol->addChild(std::move(btnRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  leftCol->addChild(std::move(checkRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  leftCol->addChild(std::move(radRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  leftCol->addChild(std::move(slidRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});

  auto leftBox = std::make_unique<Box>(std::move(leftCol));
  leftBox->withTitle(" Basic & Interactive Controls ", HorizontalAlignment::Left)
         .withOutline(boxStyle::rounded)
         .withColours(ColourPair{.fg = NamedColour::Green, .bg = NamedColour::Black});

  // ── Right Column: Form & Text Inputs (Phase 7) ─────────────────────────────
  // 1. Username
  auto uLbl = std::make_unique<Text>(state.userLabel);
  uLbl->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Black}});

  auto uIn = std::make_unique<TextInput>("username...", state.username);
  uIn->withValidator([](std::string_view v) { return !v.empty() && v.size() <= 20; })
     .withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
     .withPadding(Insets{0, 0, 1, 1});
  outUserInput = uIn.get();

  auto uRow = std::make_unique<Row>();
  uRow->withGap(1);
  uRow->addChild(std::move(uLbl), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  uRow->addChild(std::move(uIn), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 2. Password
  auto pLbl = std::make_unique<Text>(state.passLabel);
  pLbl->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Black}});

  auto pIn = std::make_unique<TextInput>("password...", state.password);
  pIn->withMask('*')
     .withStyle(Style{.colours = {.fg = NamedColour::Yellow, .bg = NamedColour::Navy}})
     .withPadding(Insets{0, 0, 1, 1});

  auto pRow = std::make_unique<Row>();
  pRow->withGap(1);
  pRow->addChild(std::move(pLbl), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  pRow->addChild(std::move(pIn), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 3. Age (Digits filter + Number validator)
  auto aLbl = std::make_unique<Text>(state.ageLabel);
  aLbl->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Black}});

  auto aIn = std::make_unique<TextInput>("age...", state.age);
  aIn->withFilter(filters::digits())
     .withValidator(validators::number())
     .withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
     .withPadding(Insets{0, 0, 1, 1});
  outAgeInput = aIn.get();

  auto aRow = std::make_unique<Row>();
  aRow->withGap(1);
  aRow->addChild(std::move(aLbl), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  aRow->addChild(std::move(aIn), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Content, 0, {}});

  // 4. Bio (Multiline TextArea)
  auto bLbl = std::make_unique<Text>(state.bioLabel);
  bLbl->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Black}});

  auto bIn = std::make_unique<TextInput>("bio...", state.bio);
  bIn->withMultiline(true)
     .withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
     .withPadding(Insets{0, 0, 1, 1});

  auto bRow = std::make_unique<Row>();
  bRow->withGap(1);
  bRow->addChild(std::move(bLbl), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  bRow->addChild(std::move(bIn), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Content, 0, {}});

  auto rightCol = std::make_unique<Column>();
  rightCol->withGap(1);
  rightCol->addChild(std::move(uRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  rightCol->addChild(std::move(pRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  rightCol->addChild(std::move(aRow), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  rightCol->addChild(std::move(bRow), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Flex, 1, {}});

  auto rightBox = std::make_unique<Box>(std::move(rightCol));
  rightBox->withTitle(" Form & Text Inputs ", HorizontalAlignment::Left)
          .withOutline(boxStyle::rounded)
          .withColours(ColourPair{.fg = NamedColour::Cyan, .bg = NamedColour::Black});

  // ── Side-by-Side Main Row ──────────────────────────────────────────────────
  auto mainRow = std::make_unique<Row>();
  mainRow->withGap(1);
  mainRow->addChild(std::move(leftBox), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Flex, 1, {}});
  mainRow->addChild(std::move(rightBox), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Flex, 1, {}});

  // ── Live Status Box ────────────────────────────────────────────────────────
  auto statusText = std::make_unique<Text>(state.statusLines);
  statusText->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
            .withPadding(Insets{0, 0, 1, 1});

  auto statusBox = std::make_unique<Box>(std::move(statusText));
  statusBox->withTitle(" Live State & Real-time Inspection ", HorizontalAlignment::Left)
           .withOutline(boxStyle::rounded)
           .withColours(ColourPair{.fg = NamedColour::Silver, .bg = NamedColour::Black});

  // ── Keybind Footer Box ─────────────────────────────────────────────────────
  auto footerText = std::make_unique<Text>(state.footerLines);
  footerText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Silver}})
            .withAlignment(HorizontalAlignment::Center);

  auto footerBox = std::make_unique<Box>(std::move(footerText));
  footerBox->withOutline(boxStyle::light)
           .withColours(ColourPair{.fg = NamedColour::Black, .bg = NamedColour::Silver});

  // ── Master Root Column ─────────────────────────────────────────────────────
  auto root = std::make_unique<Column>();
  root->withGap(1);
  root->addChild(std::move(headerBox), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  root->addChild(std::move(mainRow), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Flex, 1, {}});
  root->addChild(std::move(statusBox), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  root->addChild(std::move(footerBox), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});

  return root;
}

// ──────────────────────────────────────────────────────────────────────────────
// Main Entry Point & Event Loop
// ──────────────────────────────────────────────────────────────────────────────

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  // Initialize Linux raw terminal backend with keyboard and SGR mouse support
  linux_backend terminal(true, true);
  terminal.enterAlternateScreenBuffer();
  terminal.clearScreen();

  AppState state;
  AnimationManager animationManager;
  DemoSlider* sliderPtr = nullptr;
  radioGroup<Row>* radioPtr = nullptr;
  TextInput* userInputPtr = nullptr;
  TextInput* ageInputPtr = nullptr;

  WidgetTree tree(buildUI(state, animationManager, sliderPtr, radioPtr, userInputPtr, ageInputPtr), terminal);
  tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});

  EventDispatcher dispatcher;
  dispatcher.rebuildFocusOrder(tree.root);
  dispatcher.nextFocus(); // Focus the first interactive widget

  bool running = true;
  while (running) {
    // Dynamic progress update
    state.progress += 0.8f;
    if (state.progress > 100.0f) state.progress -= 100.0f;

    // Synchronize radio selection
    if (radioPtr) {
      for (std::size_t i = 0; i < radioPtr->base.children.size(); ++i) {
        if (auto* cb = dynamic_cast<checkbox*>(radioPtr->base.children[i].widget.get())) {
          if (cb->isChecked() && (!radioPtr->getSelected().has_value() || *radioPtr->getSelected() != i)) {
            radioPtr->select(i);
          }
        }
      }
    }

    std::u32string radioSelectedStr = U"none";
    if (radioPtr && radioPtr->getSelected().has_value()) {
      radioSelectedStr = formatUnsigned(*radioPtr->getSelected() + 1);
    }

    // Evaluate input validation status
    bool userValid = userInputPtr ? userInputPtr->isValid() : true;
    bool ageValid = ageInputPtr ? ageInputPtr->isValid() : true;

    // Update real-time status inspection readout
    state.statusLines[0] = U"Clicks: " + formatUnsigned(state.clickCount) +
                           U" | Radio: " + radioSelectedStr +
                           U" | Checkboxes: [" + u32Bool(state.checkSquare) + U", " + u32Bool(state.checkCheckmark) + U"]" +
                           U" | Toggle: " + u32Bool(state.toggleSwitchState) +
                           U" | Slider: " + (sliderPtr ? formatDouble(sliderPtr->getValue()) : U"-");

    state.statusLines[1] = U"Validation: [User: " + std::u32string(userValid ? U"✓" : U"✗") +
                           U", Pass: ✓" +
                           U", Age: " + std::u32string(ageValid ? U"✓" : U"✗") +
                           U"] | Pass Len: " + formatUnsigned(state.password.size()) +
                           U" | Age Digits: " + formatUnsigned(state.age.size());

    // Frame update & layout pass
    tree.fb.resizeBuffer();
    tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});
    animationManager.update(std::chrono::steady_clock::now());
    tree.render();
    tree.incrementDisplay();

    // Event handling
    auto event = terminal.readEvent();
    if (event) {
      bool handled = dispatcher.dispatchEvent(event.value(), tree);
      if (!handled) {
        if (auto key = std::get_if<keyEvent>(&event.value())) {
          if (key->key == 'q' || key->key == 'Q') {
            running = false;
          }
        }
      }
    }

    // Frame throttle (~30 FPS)
    usleep(33000);
  }

  // Restore screen buffer on clean exit
  terminal.clearScreen();
  terminal.leaveAlternateScreenBuffer();

  return 0;
}

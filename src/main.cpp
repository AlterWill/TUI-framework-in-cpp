// =============================================================================
// OLD DEMO — preserved but disabled
// =============================================================================
#if 0

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

struct CustomSpacer : Widget {
  Size measure(const SizeConstraints& constraints) override {
    return Size{constraints.getMinHeight(), constraints.getMinWidth()};
  }
  void render(RenderContext&) override {}
};

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

struct AppState {
  std::vector<std::u32string> titleLines{U"TUI Framework — Interactive Showcase & Demo"};

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

  std::vector<std::u32string> userLabel{U"User:"};
  std::vector<std::u32string> passLabel{U"Pass:"};
  std::vector<std::u32string> ageLabel{U"Age: "};
  std::vector<std::u32string> bioLabel{U"Bio:"};

  std::string username{"developer"};
  std::string password{"secret123"};
  std::string age{"25"};
  std::string bio{"C++20 Terminal Framework\nRetained-mode layout\nKeyboard & Mouse support"};

  bool checkSquare{true};
  bool checkCheckmark{false};
  bool toggleSwitchState{true};
  bool radio0{false};
  bool radio1{true};
  bool radio2{false};

  float progress{35.0f};
  unsigned int clickCount{0};

  std::vector<std::u32string> statusLines{
      U"Clicks: 0 | Radio: 1 | Checkboxes: [ON, OFF] | Toggle: ON | Slider: 0.35",
      U"Input Validation: [User: ✓, Pass: ✓, Age: ✓] | Bio: 3 lines"};

  std::vector<std::u32string> footerLines{
      U"[Tab / Shift+Tab] Focus   [Enter / Space] Toggle   [← / → / Drag] Slider   [Esc] Unfocus   [q] Quit"};
};

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
  auto title = std::make_unique<Text>(state.titleLines);
  title->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
        .withAlignment(HorizontalAlignment::Center);

  auto headerBox = std::make_unique<Box>(std::move(title));
  headerBox->withTitle(" TUI Library Showcase ", HorizontalAlignment::Center)
           .withOutline(boxStyle::heavy)
           .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Navy});

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

  auto mainRow = std::make_unique<Row>();
  mainRow->withGap(1);
  mainRow->addChild(std::move(leftBox), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Flex, 1, {}});
  mainRow->addChild(std::move(rightBox), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Flex, 1, {}});

  auto statusText = std::make_unique<Text>(state.statusLines);
  statusText->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
            .withPadding(Insets{0, 0, 1, 1});

  auto statusBox = std::make_unique<Box>(std::move(statusText));
  statusBox->withTitle(" Live State & Real-time Inspection ", HorizontalAlignment::Left)
           .withOutline(boxStyle::rounded)
           .withColours(ColourPair{.fg = NamedColour::Silver, .bg = NamedColour::Black});

  auto footerText = std::make_unique<Text>(state.footerLines);
  footerText->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Silver}})
            .withAlignment(HorizontalAlignment::Center);

  auto footerBox = std::make_unique<Box>(std::move(footerText));
  footerBox->withOutline(boxStyle::light)
           .withColours(ColourPair{.fg = NamedColour::Black, .bg = NamedColour::Silver});

  auto root = std::make_unique<Column>();
  root->withGap(1);
  root->addChild(std::move(headerBox), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  root->addChild(std::move(mainRow), SizeSpec{SizeType::Flex, 1, {}}, SizeSpec{SizeType::Flex, 1, {}});
  root->addChild(std::move(statusBox), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  root->addChild(std::move(footerBox), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});

  return root;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

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
  dispatcher.nextFocus();

  bool running = true;
  while (running) {
    state.progress += 0.8f;
    if (state.progress > 100.0f) state.progress -= 100.0f;

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

    bool userValid = userInputPtr ? userInputPtr->isValid() : true;
    bool ageValid = ageInputPtr ? ageInputPtr->isValid() : true;

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

    tree.fb.resizeBuffer();
    tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});
    animationManager.update(std::chrono::steady_clock::now());
    tree.render();
    tree.incrementDisplay();

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

    usleep(33000);
  }

  terminal.clearScreen();
  terminal.leaveAlternateScreenBuffer();

  return 0;
}

#endif  // OLD DEMO
// =============================================================================


// =============================================================================
// NEW DEMO — Tree View, List View, Table, Tabs, Menu, Popup, Modal,
//            Tooltip, Notification, Context Menu
// =============================================================================

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <unistd.h>

#include "core/widgetTree.hpp"
#include "input/eventHandler.hpp"
#include "layout/ColumnContainer.hpp"
#include "layout/Overlay.hpp"
#include "layout/RowContainer.hpp"
#include "layout/ScrollContainer.hpp"
#include "terminal/linux_backend.hpp"
#include "utilities/Rect.hpp"
#include "utilities/alignment.hpp"
#include "utilities/boxStyle.hpp"
#include "utilities/insets.hpp"
#include "widgets/box.hpp"
#include "widgets/button.hpp"
#include "widgets/text.hpp"
#include "widgets/treeView.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

static std::u32string toU32(const char* s) {
  return unicode::toUtf32(s ? std::string(s) : std::string());
}

// ─────────────────────────────────────────────────────────────────────────────
// TableWidget — custom widget that draws a bordered table from string data
// ─────────────────────────────────────────────────────────────────────────────

struct TableWidget : Widget {
  struct Row {
    std::vector<std::u32string> cells;
  };

  std::vector<std::u32string> headers;
  std::vector<Row>            rows;
  ColourPair                  headerColours{NamedColour::Black,  NamedColour::Cyan};
  ColourPair                  rowColours{NamedColour::White,    NamedColour::Black};
  ColourPair                  altRowColours{NamedColour::White,  NamedColour::Navy};
  ColourPair                  borderColours{NamedColour::Silver, NamedColour::Black};
  Rect                        rect{};

  TableWidget() = default;

  // Compute column widths: max of header vs each row cell, min 3
  std::vector<std::size_t> colWidths() const {
    std::vector<std::size_t> w(headers.size(), 3);
    for (std::size_t c = 0; c < headers.size(); ++c) {
      w[c] = std::max(w[c], headers[c].size() + 2); // +2 for padding
    }
    for (auto& row : rows) {
      for (std::size_t c = 0; c < row.cells.size() && c < headers.size(); ++c) {
        w[c] = std::max(w[c], row.cells[c].size() + 2);
      }
    }
    return w;
  }

  std::size_t tableWidth() const {
    auto w = colWidths();
    std::size_t total = 1; // left border
    for (auto cw : w) total += cw + 1; // col + right separator
    return total;
  }

  std::size_t tableHeight() const {
    // top border + header + separator + rows + bottom border
    return 3 + rows.size() + 1;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t tw = std::clamp(tableWidth(),  constraints.getMinWidth(),  constraints.getMaxWidth());
    std::size_t th = std::clamp(tableHeight(), constraints.getMinHeight(), constraints.getMaxHeight());
    return Size{th, tw};
  }

  void layout(const Rect& r) override { rect = r; }

  // Helper: draw a horizontal rule across the table
  void drawHRule(RenderContext& ctx, std::size_t y, std::size_t startX,
                 const std::vector<std::size_t>& widths,
                 char32_t left, char32_t mid, char32_t right, char32_t fill) const {
    Cell bc{U' ', Style{.colours = borderColours}};
    std::size_t x = startX;
    bc.glyph = left; ctx.setCell(x, y, bc); ++x;
    for (std::size_t ci = 0; ci < widths.size(); ++ci) {
      bc.glyph = fill;
      for (std::size_t k = 0; k < widths[ci]; ++k) { ctx.setCell(x, y, bc); ++x; }
      bc.glyph = (ci + 1 < widths.size()) ? mid : right;
      ctx.setCell(x, y, bc); ++x;
    }
  }

  // Helper: draw a row of cells
  void drawRow(RenderContext& ctx, std::size_t y, std::size_t startX,
               const std::vector<std::size_t>& widths,
               const std::vector<std::u32string>& cells,
               ColourPair colours) const {
    Cell bc{U'│', Style{.colours = borderColours}};
    std::size_t x = startX;
    ctx.setCell(x, y, bc); ++x;
    for (std::size_t ci = 0; ci < widths.size(); ++ci) {
      // fill background
      Cell fill{U' ', Style{.colours = colours}};
      for (std::size_t k = 0; k < widths[ci]; ++k) { ctx.setCell(x + k, y, fill); }
      // write text with 1-char left pad
      const std::u32string& text = (ci < cells.size()) ? cells[ci] : U"";
      for (std::size_t k = 0; k < text.size() && 1 + k < widths[ci]; ++k) {
        Cell tc{text[k], Style{.colours = colours}};
        ctx.setCell(x + 1 + k, y, tc);
      }
      x += widths[ci];
      bc.glyph = U'│'; ctx.setCell(x, y, bc); ++x;
    }
  }

  void render(RenderContext& ctx) override {
    if (rect.height < 4 || rect.width < 4) return;
    auto widths = colWidths();
    std::size_t sx = rect.x;
    std::size_t y  = rect.y;

    // Top border
    drawHRule(ctx, y, sx, widths, U'┌', U'┬', U'┐', U'─'); ++y;
    // Header row
    drawRow(ctx, y, sx, widths, headers, headerColours); ++y;
    // Header/body separator
    drawHRule(ctx, y, sx, widths, U'├', U'┼', U'┤', U'─'); ++y;
    // Data rows
    for (std::size_t ri = 0; ri < rows.size() && y < rect.y + rect.height - 1; ++ri, ++y) {
      ColourPair cp = (ri % 2 == 0) ? rowColours : altRowColours;
      drawRow(ctx, y, sx, widths, rows[ri].cells, cp);
    }
    // Bottom border
    if (y < rect.y + rect.height) {
      drawHRule(ctx, y, sx, widths, U'└', U'┴', U'┘', U'─');
    }
  }
};

static std::u32string formatUnsigned(unsigned long value) {
  char buffer[32];
  std::snprintf(buffer, sizeof buffer, "%lu", value);
  std::u32string result;
  for (char c : buffer) {
    if (c == '\0') break;
    result.push_back(static_cast<char32_t>(static_cast<unsigned char>(c)));
  }
  return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// App state
// ─────────────────────────────────────────────────────────────────────────────

struct DemoState {
  // Tabs: 0=TreeView, 1=ListView, 2=Table
  int activeTab{0};

  // List view selection
  int listSelected{0};

  // Status bar text (vector so Text widget can hold a ref)
  std::vector<std::u32string> statusText{U"Ready — [Tab] Focus  [1-3] Switch tab  [↑/↓] Navigate list  [m] Modal  [n] Notify  [Esc] Close"};

  // Notification auto-dismiss counter
  int notifyFrames{0};

  // Tooltip auto-dismiss counter
  int tooltipFrames{0};

  // Overlay pointer (set after build)
  Overlay* overlay{nullptr};

  // Tab buttons (to update their appearance)
  button* tabBtns[3]{};

  // List-view buttons and scroll container
  std::vector<button*> listBtns;
  Scroll* listScroll{nullptr};

  // Flag to trigger full terminal redraw when tabs or overlays change
  bool needFullRedraw{false};

  // Deferred action queue to prevent tree mutation / iterator invalidation during event handling
  std::vector<std::function<void()>> pendingActions;

  void postAction(std::function<void()> action) {
    if (action) {
      pendingActions.push_back(std::move(action));
    }
  }

  void processPendingActions() {
    if (pendingActions.empty()) return;
    auto actions = std::move(pendingActions);
    pendingActions.clear();
    for (auto& act : actions) {
      if (act) act();
    }
  }
};

static void selectListItem(DemoState& state, int newIndex) {
  int total = static_cast<int>(state.listBtns.size());
  if (total == 0) return;
  state.listSelected = std::clamp(newIndex, 0, total - 1);
  state.statusText[0] = U"List View: Item [" + formatUnsigned(static_cast<unsigned long>(state.listSelected + 1)) +
                        U"/" + formatUnsigned(static_cast<unsigned long>(total)) + U"] selected";

  // Auto-scroll so selected item is in view
  if (state.listScroll) {
    std::size_t sel = static_cast<std::size_t>(state.listSelected);
    std::size_t viewHeight = state.listScroll->scrollBase.rect.height;
    if (viewHeight > 2) {
      std::size_t usableH = viewHeight - 2;
      if (sel < state.listScroll->scrollBase.offsetY) {
        state.listScroll->scrollBase.offsetY = sel;
      } else if (sel >= state.listScroll->scrollBase.offsetY + usableH) {
        state.listScroll->scrollBase.offsetY = sel - usableH + 1;
      }
    }
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// TabContainer — MultiChildWidget containing tab panels, routing to active tab
// ─────────────────────────────────────────────────────────────────────────────

struct TabContainer : public MultiChildWidget {
  DemoState* st{};

  TabContainer() = default;
  explicit TabContainer(DemoState* state) : st(state) {}

  Size measure(const SizeConstraints& constraints) override {
    if (st && st->activeTab >= 0 && static_cast<std::size_t>(st->activeTab) < base.children.size()) {
      auto& activeChild = base.children[static_cast<std::size_t>(st->activeTab)];
      if (activeChild.widget) {
        return activeChild.widget->measure(constraints);
      }
    }
    return Size{constraints.getMinHeight(), constraints.getMinWidth()};
  }

  void setRectForChildren(const Rect& rect) override {
    for (std::size_t i = 0; i < base.children.size(); ++i) {
      if (st && static_cast<std::size_t>(st->activeTab) == i) {
        base.children[i].rect = rect;
      } else {
        base.children[i].rect = Rect{0, 0, 0, 0};
      }
    }
  }

  void render(RenderContext& ctx) override {
    // Clear entire tab area with clean background to prevent ghosting across tabs
    const Rect& r = ctx.getRect();
    Cell bgCell{U' ', Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Black}}};
    for (std::size_t y = r.y; y < r.y + r.height; ++y) {
      for (std::size_t x = r.x; x < r.x + r.width; ++x) {
        ctx.setCell(x, y, bgCell);
      }
    }

    if (st && st->activeTab >= 0 && static_cast<std::size_t>(st->activeTab) < base.children.size()) {
      auto& activeChild = base.children[static_cast<std::size_t>(st->activeTab)];
      if (activeChild.widget) {
        ctx.setRect(activeChild.rect);
        activeChild.widget->render(ctx);
      }
    }
  }

  bool handleEvent(const Event& event) override {
    if (st && st->activeTab >= 0 && static_cast<std::size_t>(st->activeTab) < base.children.size()) {
      auto& activeChild = base.children[static_cast<std::size_t>(st->activeTab)];
      if (activeChild.widget) {
        return activeChild.widget->handleEvent(event);
      }
    }
    return false;
  }
};

// ─────────────────────────────────────────────────────────────────────────────
// Factory helpers
// ─────────────────────────────────────────────────────────────────────────────

static std::unique_ptr<button> makeTabBtn(const char* label, bool active) {
  auto btn = std::make_unique<button>();
  btn->text = toU32(label);
  btn->hAlignment = HorizontalAlignment::Center;
  btn->vAlignment  = VerticalAlignment::Center;
  btn->style = active
    ? Style{.colours = {.fg = NamedColour::Black,  .bg = NamedColour::Cyan}}
    : Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Navy}};
  btn->padding = Insets{0, 0, 1, 1};
  return btn;
}

// ─────────────────────────────────────────────────────────────────────────────
// Panel builders
// ─────────────────────────────────────────────────────────────────────────────

// TreeView panel
static std::unique_ptr<Widget> buildTreePanel(DemoState& state) {
  std::vector<TreeNode> roots;

  TreeNode src{U"src"};
  src.children.push_back(TreeNode{U"main.cpp"});
  src.children.push_back(TreeNode{U"renderer.cpp"});
  src.children.push_back(TreeNode{U"events.cpp"});
  src.expanded = true;
  roots.push_back(std::move(src));

  TreeNode inc{U"include"};
  {
    TreeNode core{U"core"};
    core.children.push_back(TreeNode{U"widget.hpp"});
    core.children.push_back(TreeNode{U"widgetTree.hpp"});
    core.children.push_back(TreeNode{U"layoutNode.hpp"});
    inc.children.push_back(std::move(core));

    TreeNode widgets{U"widgets"};
    widgets.children.push_back(TreeNode{U"box.hpp"});
    widgets.children.push_back(TreeNode{U"button.hpp"});
    widgets.children.push_back(TreeNode{U"text.hpp"});
    widgets.children.push_back(TreeNode{U"treeView.hpp"});
    inc.children.push_back(std::move(widgets));

    TreeNode layout{U"layout"};
    layout.children.push_back(TreeNode{U"Overlay.hpp"});
    layout.children.push_back(TreeNode{U"RowContainer.hpp"});
    layout.children.push_back(TreeNode{U"ColumnContainer.hpp"});
    inc.children.push_back(std::move(layout));
  }
  roots.push_back(std::move(inc));

  TreeNode build{U"build"};
  build.children.push_back(TreeNode{U"CMakeCache.txt"});
  build.children.push_back(TreeNode{U"tui"});
  roots.push_back(std::move(build));

  auto tree = std::make_unique<TreeView>(std::move(roots));
  tree->withColours(
    ColourPair{.fg = NamedColour::White,  .bg = NamedColour::Black},
    ColourPair{.fg = NamedColour::Black,  .bg = NamedColour::Green},
    ColourPair{.fg = NamedColour::Yellow, .bg = NamedColour::Black}
  );
  tree->withGlyphs(U'▼', U'▶', U'·');
  tree->withIndentSize(2);
  tree->onSelect([&state](const std::vector<std::size_t>& path) {
    state.statusText[0] = U"Tree View: Node selected at depth " + formatUnsigned(path.size());
  });
  tree->onExpand([&state](const std::vector<std::size_t>& path) {
    state.statusText[0] = U"Tree View: Node expanded/collapsed at depth " + formatUnsigned(path.size());
  });

  auto box = std::make_unique<Box>(std::move(tree));
  box->withTitle(" File Tree (Arrows / Enter / Space) ", HorizontalAlignment::Left)
     .withOutline(boxStyle::rounded)
     .withColours(ColourPair{.fg = NamedColour::Green, .bg = NamedColour::Black});
  return box;
}

// ListView panel
static std::unique_ptr<Widget> buildListPanel(DemoState& state) {
  const char* items[] = {
    " 01. Alpha    - Core system initialization",
    " 02. Beta     - Network configuration module",
    " 03. Gamma    - User interface rendering engine",
    " 04. Delta    - Event dispatch & input handler",
    " 05. Epsilon  - Layout solver & constraint engine",
    " 06. Zeta     - Animation & timer manager",
    " 07. Eta      - Diagnostic logger & telemetry",
    " 08. Theta    - Database connector & cache",
    " 09. Iota     - Security credentials & auth",
    " 10. Kappa    - Compression & serialization",
    " 11. Lambda   - Cloud sync & background worker",
    " 12. Mu       - Audio synthesizer & mixer",
    " 13. Nu       - Scripting engine interpreter",
    " 14. Xi       - Plugin loader & extensions",
    " 15. Omicron  - Package manager & updates",
  };

  auto col = std::make_unique<Column>();
  state.listBtns.clear();

  for (int i = 0; i < static_cast<int>(std::size(items)); ++i) {
    auto btn = std::make_unique<button>();
    btn->text = toU32(items[i]);
    btn->hAlignment = HorizontalAlignment::Left;
    btn->vAlignment  = VerticalAlignment::Center;
    bool selected = (i == state.listSelected);
    btn->style = selected
      ? Style{.colours = {.fg = NamedColour::Black,  .bg = NamedColour::Cyan}}
      : Style{.colours = {.fg = NamedColour::White,  .bg = NamedColour::Black}};
    btn->padding = Insets{0, 0, 0, 1};
    int idx = i;
    btn->onClick = [&state, idx]() {
      state.postAction([&state, idx]() {
        selectListItem(state, idx);
      });
    };
    state.listBtns.push_back(btn.get());
    col->addChild(std::move(btn),
                  SizeSpec{SizeType::Content, 0, {}},
                  SizeSpec{SizeType::Flex, 1, {}});
  }

  auto scroll = std::make_unique<Scroll>(std::move(col), ScrollDirection::Vertical);
  state.listScroll = scroll.get();

  auto box = std::make_unique<Box>(std::move(scroll));
  box->withTitle(" List View (Arrows / PgUp / PgDn / Home / End / Click) ", HorizontalAlignment::Left)
     .withOutline(boxStyle::rounded)
     .withColours(ColourPair{.fg = NamedColour::Cyan, .bg = NamedColour::Black});
  return box;
}

// Table panel
static std::unique_ptr<Widget> buildTablePanel() {
  auto tbl = std::make_unique<TableWidget>();
  tbl->headers = {U"ID", U"Name", U"Role", U"Department", U"Status", U"Score"};
  tbl->rows = {
    {{U"#101", U"Alice",   U"Lead Engineer",  U"Core Architecture", U"Active",    U"98/100"}},
    {{U"#102", U"Bob",     U"UI/UX Designer", U"Product Design",    U"Active",    U"87/100"}},
    {{U"#103", U"Charlie", U"Eng Manager",    U"Engineering",       U"Away",      U"76/100"}},
    {{U"#104", U"Diana",   U"QA Lead",        U"Quality Assurance", U"Active",    U"91/100"}},
    {{U"#105", U"Eve",     U"DevOps Lead",    U"Infrastructure",    U"Inactive",  U"65/100"}},
    {{U"#106", U"Frank",   U"Data Analyst",   U"Business Intel",    U"Active",    U"80/100"}},
    {{U"#107", U"Grace",   U"HR Specialist",  U"Human Resources",   U"Away",      U"72/100"}},
    {{U"#108", U"Hank",    U"Support Tech",   U"Customer Success",  U"Active",    U"88/100"}},
  };
  tbl->headerColours   = {NamedColour::Black,  NamedColour::Aqua};
  tbl->rowColours      = {NamedColour::White,  NamedColour::Black};
  tbl->altRowColours   = {NamedColour::White,  NamedColour::Navy};
  tbl->borderColours   = {NamedColour::Silver, NamedColour::Black};

  auto box = std::make_unique<Box>(std::move(tbl));
  box->withTitle(" Data Table (Headers & Alternating Rows) ", HorizontalAlignment::Left)
     .withOutline(boxStyle::rounded)
     .withColours(ColourPair{.fg = NamedColour::Silver, .bg = NamedColour::Black});
  return box;
}

// ─────────────────────────────────────────────────────────────────────────────
// Overlay builders (modal, popup, tooltip, notification, context menu)
// ─────────────────────────────────────────────────────────────────────────────

static void showModal(Overlay* ov, DemoState& state) {
  if (!ov) return;

  static std::vector<std::u32string> msg{
    U"This is a modal dialog.",
    U"",
    U"It blocks all input behind it.",
    U"Click OK or press [Esc] to close."
  };
  auto txt = std::make_unique<Text>(msg);
  txt->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
     .withAlignment(HorizontalAlignment::Center)
     .withPadding(Insets{1, 1, 2, 2});

  auto okBtn = std::make_unique<button>();
  okBtn->text = U"  [ OK ]  ";
  okBtn->style = Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Green}};
  okBtn->hAlignment = HorizontalAlignment::Center;
  okBtn->vAlignment  = VerticalAlignment::Center;
  okBtn->padding = Insets{0, 0, 1, 1};
  okBtn->onClick = [&state]() {
    state.postAction([&state]() {
      if (state.overlay) {
        state.overlay->dismissTopOverlay();
        state.needFullRedraw = true;
        state.statusText[0] = U"Modal closed via OK button";
      }
    });
  };

  auto col = std::make_unique<Column>();
  col->withGap(1);
  col->addChild(std::move(txt),   SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  col->addChild(std::move(okBtn), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});

  auto box = std::make_unique<Box>(std::move(col));
  box->withTitle(" [!] Modal Dialog (Esc to Dismiss) ", HorizontalAlignment::Center)
     .withOutline(boxStyle::heavy)
     .withColours(ColourPair{.fg = NamedColour::Yellow, .bg = NamedColour::Navy});

  ov->addOverlay(std::move(box), /*modal=*/true, /*dismissOnClickOutside=*/true);
  state.needFullRedraw = true;
  state.statusText[0] = U"Modal Dialog opened (press Esc or click outside to dismiss)";
}

static void showNotification(Overlay* ov, DemoState& state) {
  if (!ov) return;
  ov->dismissAllOverlays();

  static std::vector<std::u32string> msg{U" [OK] Action completed successfully! (Press Esc to close) "};
  auto txt = std::make_unique<Text>(msg);
  txt->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Green}})
     .withPadding(Insets{0, 0, 1, 1});

  auto box = std::make_unique<Box>(std::move(txt));
  box->withOutline(boxStyle::rounded)
     .withColours(ColourPair{.fg = NamedColour::Black, .bg = NamedColour::Green});

  ov->addOverlay(std::move(box), /*modal=*/false, /*dismissOnClickOutside=*/true);
  state.notifyFrames = 90; // ~3 seconds at 30 FPS
  state.needFullRedraw = true;
  state.statusText[0] = U"Notification banner shown (auto-dismisses in 3s or press Esc)";
}

static void showTooltip(Overlay* ov, DemoState& state) {
  if (!ov) return;
  ov->dismissAllOverlays();

  static std::vector<std::u32string> msg{
    U" Tooltip: [Tab] focus   [1-3] tabs   [m] modal      ",
    U"          [n] notify     [c] context  [Esc] dismiss  "
  };
  auto txt = std::make_unique<Text>(msg);
  txt->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Yellow}})
     .withPadding(Insets{0, 0, 0, 0});

  auto box = std::make_unique<Box>(std::move(txt));
  box->withOutline(boxStyle::light)
     .withColours(ColourPair{.fg = NamedColour::Black, .bg = NamedColour::Yellow});

  ov->addOverlay(std::move(box), /*modal=*/false, /*dismissOnClickOutside=*/true);
  state.tooltipFrames = 150; // ~5 seconds
  state.needFullRedraw = true;
  state.statusText[0] = U"Tooltip popup shown (auto-dismisses in 5s or press Esc)";
}

static void showContextMenu(Overlay* ov, DemoState& state) {
  if (!ov) return;
  ov->dismissAllOverlays();

  struct Item { const char* label; };
  static const Item items[] = {
    {" Copy        "},
    {" Cut         "},
    {" Paste       "},
    {" ------------"},
    {" Select All  "},
    {" Delete      "},
  };

  auto col = std::make_unique<Column>();
  for (auto& it : items) {
    if (it.label[1] == '-') {
      static std::vector<std::u32string> sep{U"────────────"};
      auto t = std::make_unique<Text>(sep);
      t->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Black}});
      col->addChild(std::move(t), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
    } else {
      auto btn = std::make_unique<button>();
      btn->text = toU32(it.label);
      btn->style = Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Black}};
      btn->hAlignment = HorizontalAlignment::Left;
      btn->vAlignment  = VerticalAlignment::Center;
      std::string lbl(it.label);
      btn->onClick = [&state, lbl]() {
        state.postAction([&state, lbl]() {
          if (state.overlay) state.overlay->dismissTopOverlay();
          state.needFullRedraw = true;
          state.statusText[0] = toU32(("Context Menu: " + lbl).c_str());
        });
      };
      col->addChild(std::move(btn), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
    }
  }

  auto box = std::make_unique<Box>(std::move(col));
  box->withTitle(" Context Menu (Esc to Close) ", HorizontalAlignment::Left)
     .withOutline(boxStyle::light)
     .withColours(ColourPair{.fg = NamedColour::Silver, .bg = NamedColour::Black});

  ov->addOverlay(std::move(box), /*modal=*/false, /*dismissOnClickOutside=*/true);
  state.needFullRedraw = true;
  state.statusText[0] = U"Context Menu opened (press Esc to close)";
}

// ─────────────────────────────────────────────────────────────────────────────
// Menu-bar popup builder
// ─────────────────────────────────────────────────────────────────────────────

static void showMenuPopup(Overlay* ov, DemoState& state, const char* menuName,
                          const std::vector<const char*>& items) {
  if (!ov) return;
  ov->dismissAllOverlays();

  auto col = std::make_unique<Column>();
  for (auto* label : items) {
    if (label[0] == '-') {
      static std::vector<std::u32string> sep{U"─────────────"};
      auto t = std::make_unique<Text>(sep);
      t->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Navy}});
      col->addChild(std::move(t), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
    } else {
      auto btn = std::make_unique<button>();
      btn->text = toU32(label);
      btn->style = Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}};
      btn->hAlignment = HorizontalAlignment::Left;
      btn->vAlignment  = VerticalAlignment::Center;
      btn->padding = Insets{0, 0, 1, 0};
      std::string lbl(label);
      btn->onClick = [&state, lbl]() {
        state.postAction([&state, lbl]() {
          if (state.overlay) state.overlay->dismissTopOverlay();
          state.needFullRedraw = true;
          state.statusText[0] = toU32(("Menu: Selected " + lbl).c_str());
        });
      };
      col->addChild(std::move(btn), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
    }
  }

  auto box = std::make_unique<Box>(std::move(col));
  box->withTitle(std::string(" ") + menuName + " (Esc to Close) ", HorizontalAlignment::Left)
     .withOutline(boxStyle::light)
     .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Navy});

  ov->addOverlay(std::move(box), /*modal=*/false, /*dismissOnClickOutside=*/true);
  state.needFullRedraw = true;
  state.statusText[0] = toU32((std::string("Opened ") + menuName + " menu (press Esc to close)").c_str());
}

// ─────────────────────────────────────────────────────────────────────────────
// Full UI tree
// ─────────────────────────────────────────────────────────────────────────────

static std::unique_ptr<Widget> buildUI(DemoState& state) {

  // ── Header ────────────────────────────────────────────────────────────────
  static std::vector<std::u32string> titleLines{
    U"TUI Demo - Tree View | List View | Table | Tabs | Menus | Overlays"};
  auto titleTxt = std::make_unique<Text>(titleLines);
  titleTxt->withStyle(Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}})
           .withAlignment(HorizontalAlignment::Center);
  auto headerBox = std::make_unique<Box>(std::move(titleTxt));
  headerBox->withOutline(boxStyle::heavy)
           .withColours(ColourPair{.fg = NamedColour::Aqua, .bg = NamedColour::Navy});

  // ── Menu bar ──────────────────────────────────────────────────────────────
  auto makeMenuBtn = [&](const char* label,
                         std::vector<const char*> items) -> std::unique_ptr<button> {
    auto btn = std::make_unique<button>();
    btn->text = toU32(label);
    btn->style = Style{.colours = {.fg = NamedColour::White, .bg = NamedColour::Navy}};
    btn->hAlignment = HorizontalAlignment::Center;
    btn->vAlignment  = VerticalAlignment::Center;
    btn->padding = Insets{0, 0, 2, 2};
    std::string nm(label);
    std::vector<const char*> its = std::move(items);
    btn->onClick = [&state, nm, its]() {
      state.postAction([&state, nm, its]() {
        if (state.overlay) showMenuPopup(state.overlay, state, nm.c_str(), its);
      });
    };
    return btn;
  };

  auto menuRow = std::make_unique<Row>();
  menuRow->withGap(0);
  menuRow->addChild(
    makeMenuBtn(" File ",  {" New File   ", " Open File ", " Save File ", "---", " Exit      "}),
    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  menuRow->addChild(
    makeMenuBtn(" Edit ",  {" Cut       ", " Copy      ", " Paste     ", "---", " Select All"}),
    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  menuRow->addChild(
    makeMenuBtn(" View ",  {" Tree View ", " List View ", " Table     ", "---", " Refresh   "}),
    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  menuRow->addChild(
    makeMenuBtn(" Help ",  {" Shortcuts ", " About...  "}),
    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});

  auto menuBox = std::make_unique<Box>(std::move(menuRow));
  menuBox->withOutline(boxStyle::light)
         .withColours(ColourPair{.fg = NamedColour::Silver, .bg = NamedColour::Navy});

  // ── Tab bar ───────────────────────────────────────────────────────────────
  auto tab0 = makeTabBtn(" [1] Tree View ", state.activeTab == 0);
  auto tab1 = makeTabBtn(" [2] List View ", state.activeTab == 1);
  auto tab2 = makeTabBtn(" [3] Table     ", state.activeTab == 2);
  state.tabBtns[0] = tab0.get();
  state.tabBtns[1] = tab1.get();
  state.tabBtns[2] = tab2.get();

  tab0->onClick = [&state]() {
    state.postAction([&state]() {
      state.activeTab = 0;
      state.needFullRedraw = true;
      state.statusText[0] = U"Tab 1: Tree View";
    });
  };
  tab1->onClick = [&state]() {
    state.postAction([&state]() {
      state.activeTab = 1;
      state.needFullRedraw = true;
      state.statusText[0] = U"Tab 2: List View";
    });
  };
  tab2->onClick = [&state]() {
    state.postAction([&state]() {
      state.activeTab = 2;
      state.needFullRedraw = true;
      state.statusText[0] = U"Tab 3: Table";
    });
  };

  auto tabRow = std::make_unique<Row>();
  tabRow->withGap(1);
  tabRow->addChild(std::move(tab0), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  tabRow->addChild(std::move(tab1), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});
  tabRow->addChild(std::move(tab2), SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Content, 0, {}});

  // ── TabContainer with all 3 panels ─────────────────────────────────────────
  auto tabContainer = std::make_unique<TabContainer>(&state);
  {
    LayoutNode n0; n0.widget = buildTreePanel(state);
    LayoutNode n1; n1.widget = buildListPanel(state);
    LayoutNode n2; n2.widget = buildTablePanel();
    tabContainer->base.children.push_back(std::move(n0));
    tabContainer->base.children.push_back(std::move(n1));
    tabContainer->base.children.push_back(std::move(n2));
  }

  // ── Status bar ────────────────────────────────────────────────────────────
  auto statusTxt = std::make_unique<Text>(state.statusText);
  statusTxt->withStyle(Style{.colours = {.fg = NamedColour::Black, .bg = NamedColour::Silver}})
            .withPadding(Insets{0, 0, 1, 1});
  auto statusBox = std::make_unique<Box>(std::move(statusTxt));
  statusBox->withOutline(boxStyle::light)
           .withColours(ColourPair{.fg = NamedColour::Black, .bg = NamedColour::Silver});

  // ── Footer ────────────────────────────────────────────────────────────────
  static std::vector<std::u32string> footerLines{
    U"[Tab] Focus  |  [1-3] Tabs  |  [↑/↓] Navigate  |  [m] Modal  |  [n] Toast  |  [t] Tooltip  |  [c] Menu  |  [Esc] Close  |  [q] Quit"};
  auto footerTxt = std::make_unique<Text>(footerLines);
  footerTxt->withStyle(Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Navy}})
            .withAlignment(HorizontalAlignment::Center)
            .withPadding(Insets{0, 0, 1, 1});
  auto footerBox = std::make_unique<Box>(std::move(footerTxt));
  footerBox->withOutline(boxStyle::light)
           .withColours(ColourPair{.fg = NamedColour::Navy, .bg = NamedColour::Navy});

  // ── Inner root column ─────────────────────────────────────────────────────
  auto innerCol = std::make_unique<Column>();
  innerCol->withGap(0);
  innerCol->addChild(std::move(headerBox),    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  innerCol->addChild(std::move(menuBox),      SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  innerCol->addChild(std::move(tabRow),       SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  innerCol->addChild(std::move(tabContainer), SizeSpec{SizeType::Flex, 1, {}},    SizeSpec{SizeType::Flex, 1, {}});
  innerCol->addChild(std::move(statusBox),    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});
  innerCol->addChild(std::move(footerBox),    SizeSpec{SizeType::Content, 0, {}}, SizeSpec{SizeType::Flex, 1, {}});

  // ── Overlay wrapper ───────────────────────────────────────────────────────
  auto overlay = std::make_unique<Overlay>();
  overlay->withTransparent(false);
  overlay->withBackgroundColour(NamedColour::Black);
  overlay->setBaseLayer(std::move(innerCol));
  state.overlay = overlay.get();

  return overlay;
}

// ─────────────────────────────────────────────────────────────────────────────
// Main entry
// ─────────────────────────────────────────────────────────────────────────────

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  linux_backend terminal(true, true);
  terminal.enterAlternateScreenBuffer();
  terminal.clearScreen();

  DemoState state;
  WidgetTree tree(buildUI(state), terminal);
  tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});

  EventDispatcher dispatcher;
  dispatcher.rebuildFocusOrder(tree.root);
  dispatcher.nextFocus();

  bool running = true;
  while (running) {

    // ── Event handling ─────────────────────────────────────────────────────
    auto event = terminal.readEvent();
    if (event) {
      bool handledDirectly = false;

      if (auto* key = std::get_if<keyEvent>(&event.value())) {
        // 1. If ESC is pressed and an overlay is active, dismiss it immediately
        if (key->key == '\x1b' || key->key == 27) {
          if (state.overlay && state.overlay->overlayCount() > 0) {
            state.postAction([&state, &dispatcher, &tree]() {
              state.overlay->dismissTopOverlay();
              state.notifyFrames = 0;
              state.tooltipFrames = 0;
              state.needFullRedraw = true;
              state.statusText[0] = U"Overlay closed via Esc";
              dispatcher.rebuildFocusOrder(tree.root);
            });
            handledDirectly = true;
          }
        }

        // 2. Global Shortcuts & Tab Navigation
        if (!handledDirectly) {
          switch (key->key) {
            case 'q': case 'Q':
              running = false;
              handledDirectly = true;
              break;
            case '1':
              state.postAction([&state, &dispatcher, &tree]() {
                state.activeTab = 0;
                state.needFullRedraw = true;
                state.statusText[0] = U"Tab 1: Tree View";
                dispatcher.rebuildFocusOrder(tree.root);
              });
              handledDirectly = true;
              break;
            case '2':
              state.postAction([&state, &dispatcher, &tree]() {
                state.activeTab = 1;
                state.needFullRedraw = true;
                state.statusText[0] = U"Tab 2: List View";
                dispatcher.rebuildFocusOrder(tree.root);
              });
              handledDirectly = true;
              break;
            case '3':
              state.postAction([&state, &dispatcher, &tree]() {
                state.activeTab = 2;
                state.needFullRedraw = true;
                state.statusText[0] = U"Tab 3: Table";
                dispatcher.rebuildFocusOrder(tree.root);
              });
              handledDirectly = true;
              break;
            case 'm': case 'M':
              state.postAction([&state, &dispatcher, &tree]() {
                showModal(state.overlay, state);
                dispatcher.rebuildFocusOrder(tree.root);
              });
              handledDirectly = true;
              break;
            case 'n': case 'N':
              state.postAction([&state]() {
                showNotification(state.overlay, state);
              });
              handledDirectly = true;
              break;
            case 't': case 'T':
              state.postAction([&state]() {
                showTooltip(state.overlay, state);
              });
              handledDirectly = true;
              break;
            case 'c': case 'C':
              state.postAction([&state, &dispatcher, &tree]() {
                showContextMenu(state.overlay, state);
                dispatcher.rebuildFocusOrder(tree.root);
              });
              handledDirectly = true;
              break;

            // ── ListView keyboard navigation & shortcuts (when on Tab 1) ──
            case ARROW_UP:
            case 'k':
            case 'K':
              if (state.activeTab == 1) {
                state.postAction([&state]() {
                  selectListItem(state, state.listSelected - 1);
                });
                handledDirectly = true;
              }
              break;
            case ARROW_DOWN:
            case 'j':
            case 'J':
              if (state.activeTab == 1) {
                state.postAction([&state]() {
                  selectListItem(state, state.listSelected + 1);
                });
                handledDirectly = true;
              }
              break;
            case PAGE_UP:
              if (state.activeTab == 1) {
                state.postAction([&state]() {
                  selectListItem(state, state.listSelected - 5);
                });
                handledDirectly = true;
              }
              break;
            case PAGE_DOWN:
              if (state.activeTab == 1) {
                state.postAction([&state]() {
                  selectListItem(state, state.listSelected + 5);
                });
                handledDirectly = true;
              }
              break;
            case HOME_KEY:
              if (state.activeTab == 1) {
                state.postAction([&state]() {
                  selectListItem(state, 0);
                });
                handledDirectly = true;
              }
              break;
            case END_KEY:
              if (state.activeTab == 1) {
                state.postAction([&state]() {
                  selectListItem(state, static_cast<int>(state.listBtns.size()) - 1);
                });
                handledDirectly = true;
              }
              break;
            case '\n':
            case ' ':
              if (state.activeTab == 1) {
                state.postAction([&state]() {
                  selectListItem(state, state.listSelected);
                  state.statusText[0] = U"List View: Activated item #" +
                                        formatUnsigned(static_cast<unsigned long>(state.listSelected + 1));
                });
                handledDirectly = true;
              }
              break;
            default:
              break;
          }
        }
      }

      // 3. Fallback to dispatcher for Tab focus cycling, tree navigation, mouse clicks, etc.
      if (!handledDirectly) {
        dispatcher.dispatchEvent(event.value(), tree);
      }
    }

    // ── Execute deferred UI actions safely outside dispatch ────────────────
    state.processPendingActions();

    // ── Update tab button styles to reflect active tab ─────────────────────
    for (int i = 0; i < 3; ++i) {
      if (state.tabBtns[i]) {
        state.tabBtns[i]->style = (i == state.activeTab)
          ? Style{.colours = {.fg = NamedColour::Black,  .bg = NamedColour::Cyan}}
          : Style{.colours = {.fg = NamedColour::Silver, .bg = NamedColour::Navy}};
      }
    }

    // ── Update list item highlight ─────────────────────────────────────────
    for (std::size_t i = 0; i < state.listBtns.size(); ++i) {
      if (state.listBtns[i]) {
        state.listBtns[i]->style = (static_cast<int>(i) == state.listSelected)
          ? Style{.colours = {.fg = NamedColour::Black,  .bg = NamedColour::Cyan}}
          : Style{.colours = {.fg = NamedColour::White,  .bg = NamedColour::Black}};
      }
    }

    // ── Auto-dismiss notification ──────────────────────────────────────────
    if (state.notifyFrames > 0) {
      --state.notifyFrames;
      if (state.notifyFrames == 0 && state.overlay) {
        state.overlay->dismissAllOverlays();
        state.needFullRedraw = true;
      }
    }

    // ── Auto-dismiss tooltip ───────────────────────────────────────────────
    if (state.tooltipFrames > 0) {
      --state.tooltipFrames;
      if (state.tooltipFrames == 0 && state.overlay) {
        state.overlay->dismissAllOverlays();
        state.needFullRedraw = true;
      }
    }

    // ── Render frame ───────────────────────────────────────────────────────
    if (state.needFullRedraw) {
      tree.fb.fullRedraw();
      state.needFullRedraw = false;
    } else {
      tree.fb.resizeBuffer();
    }
    tree.layout({0, 0, tree.fb.terminalData.row, tree.fb.terminalData.col});
    tree.render();
    tree.incrementDisplay();

    usleep(33000); // ~30 FPS
  }

  terminal.clearScreen();
  terminal.leaveAlternateScreenBuffer();
  return 0;
}




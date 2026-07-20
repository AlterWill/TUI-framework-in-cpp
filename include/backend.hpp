#pragma once

#include <cstddef>
#include <optional>
#include <variant>

enum KeyAction { ARROW_UP = 1000, ARROW_DOWN, ARROW_RIGHT, ARROW_LEFT, PAGE_UP, PAGE_DOWN, HOME_KEY, END_KEY, DEL_KEY };

enum class MouseAction {
  Press,
  Release,
  Move,
  Drag,
  ScrollUp,
  ScrollDown,
};

enum class MouseButton { Left, Middle, Right, None };

struct Modifers {
  bool CTRL{false};
  bool SHIFT{false};
  bool ALT{false};
};

struct keyEvent {
  int key{0};
  Modifers mods{};
};

struct MouseEvent {
  MouseAction action{MouseAction::Move};
  MouseButton button{MouseButton::None};
  std::size_t x{0};
  std::size_t y{0};
  Modifers mods{};
};

using Event = std::variant<keyEvent, MouseEvent>;

class backend {
 public:
  std::size_t row{0};
  std::size_t col{0};
  bool supportsTrueColor{false};
  bool supports256Color{false};

  virtual ~backend() = default;

  virtual void findTerminalSize() = 0;
  virtual void findSupportsTrueAnd256Colour() = 0;
  virtual std::size_t getTerminalSizeRow() = 0;
  virtual std::size_t getTerminalSizeCol() = 0;
  virtual int readKey() = 0;
  virtual std::optional<Event> readEvent() = 0;
};

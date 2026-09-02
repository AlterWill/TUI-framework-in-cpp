#pragma once

#include <cstddef>
#include <variant>

// clang-format off
enum KeyAction {
  ARROW_UP = 1000,
  ARROW_DOWN,
  ARROW_RIGHT,
  ARROW_LEFT,
  PAGE_UP,
  PAGE_DOWN,
  HOME_KEY,
  END_KEY,
  DEL_KEY
};
// clang-format on

enum class MouseAction {
  Press,
  Release,
  Move,
  Drag,
  ScrollUp,
  ScrollDown,
};

enum class MouseButton { Left, Middle, Right, None };

struct Modifiers {
  bool CTRL{false};
  bool SHIFT{false};
  bool ALT{false};

  // Getters
  bool getCtrl() const { return CTRL; }
  bool getShift() const { return SHIFT; }
  bool getAlt() const { return ALT; }

  // Setters
  void setCtrl(bool val) { CTRL = val; }
  void setShift(bool val) { SHIFT = val; }
  void setAlt(bool val) { ALT = val; }
};

struct keyEvent {
  int key{0};
  Modifiers mods{};

  // Getters
  int getKey() const { return key; }
  Modifiers getMods() const { return mods; }

  // Setters
  void setKey(int val) { key = val; }
  void setMods(Modifiers val) { mods = val; }
};

struct MouseEvent {
  MouseAction action{MouseAction::Move};
  MouseButton button{MouseButton::None};
  std::size_t x{0};
  std::size_t y{0};
  Modifiers mods{};

  // Getters
  MouseAction getAction() const { return action; }
  MouseButton getButton() const { return button; }
  std::size_t getX() const { return x; }
  std::size_t getY() const { return y; }
  Modifiers getMods() const { return mods; }

  // Setters
  void setAction(MouseAction val) { action = val; }
  void setButton(MouseButton val) { button = val; }
  void setX(std::size_t val) { x = val; }
  void setY(std::size_t val) { y = val; }
  void setMods(Modifiers val) { mods = val; }
};

using Event = std::variant<keyEvent, MouseEvent>;

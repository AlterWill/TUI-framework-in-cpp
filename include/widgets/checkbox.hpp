#pragma once

#include "core/widget.hpp"
#include "input/event.hpp"
#include "widgets/toggle.hpp"

struct checkbox : toggle {
  using toggle::toggle;

  checkbox& withState(bool& s) {
    toggle::withState(s);
    return *this;
  }

  checkbox& withChecked(bool c) {
    setState(c);
    return *this;
  }

  checkbox& withStyle(Style s) {
    toggle::style = std::move(s);
    return *this;
  }

  checkbox& withColours(ColourPair c) {
    toggle::style.colours = std::move(c);
    return *this;
  }

  checkbox& withPadding(Insets p) {
    toggle::base.padding = p;
    return *this;
  }

  bool isChecked() const { return isOn(); }
  void setChecked(bool c) { setState(c); }
  void select() { setState(true); }
  void deselect() { setState(false); }

  bool handleEvent(const Event& event) override {
    if (std::holds_alternative<MouseEvent>(event)) {
      const MouseEvent& me = std::get<MouseEvent>(event);
      if (me.getAction() == MouseAction::Press && me.getButton() == MouseButton::Left) {
        flip();
        return true;
      }
    } else if (std::holds_alternative<keyEvent>(event)) {
      const keyEvent& ke = std::get<keyEvent>(event);
      if ((ke.getKey() == '\n' || ke.getKey() == ' ') && base.focused) {
        flip();
        return true;
      }
    }
    return false;
  }
};
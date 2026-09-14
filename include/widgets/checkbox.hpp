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
    padding = p;
    return *this;
  }

  checkbox& withPrefix(std::u32string p) {
    toggle::withPrefix(std::move(p));
    return *this;
  }

  checkbox& withPrefix(std::string_view p) {
    toggle::withPrefix(p);
    return *this;
  }

  checkbox& withSuffix(std::u32string s) {
    toggle::withSuffix(std::move(s));
    return *this;
  }

  checkbox& withSuffix(std::string_view s) {
    toggle::withSuffix(s);
    return *this;
  }

  checkbox& withLabel(std::u32string l) {
    toggle::withLabel(std::move(l));
    return *this;
  }

  checkbox& withLabel(std::string_view l) {
    toggle::withLabel(l);
    return *this;
  }

  bool isChecked() const { return isOn(); }
  void setChecked(bool c) { setState(c); }
  void select() { setState(true); }
  void deselect() { setState(false); }
};
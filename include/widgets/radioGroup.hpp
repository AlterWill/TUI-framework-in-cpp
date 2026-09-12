#pragma once

#include <cstddef>
#include <optional>

#include "widgets/checkbox.hpp"

template <typename Container>
struct radioGroup : Container {
  using Container::addChild;
  using Container::base;

  std::optional<std::size_t> selected{};

  radioGroup() = default;

  radioGroup& withSelected(std::size_t v) {
    select(v);
    return *this;
  }

  const std::optional<std::size_t>& getSelected() const { return selected; }
  bool hasSelection() const { return selected.has_value(); }
  bool isSelected(std::size_t v) const { return selected.has_value() && selected == v; }

  void select(std::size_t value) {
    selected = value;
    for (std::size_t i = 0; i < base.children.size(); ++i) {
      if (auto* cb = dynamic_cast<checkbox*>(base.children[i].widget.get())) {
        cb->setChecked(i == value);
      }
    }
  }

  bool handleEvent(const Event& event) override {
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      if (mouse->getAction() == MouseAction::Press && mouse->getButton() == MouseButton::Left) {
        for (std::size_t i = 0; i < base.children.size(); ++i) {
          if (base.children[i].widget && base.children[i].rect.contains(mouse->getX(), mouse->getY())) {
            select(i);
            return true;
          }
        }
      }
      return false;
    }

    if (auto key = std::get_if<keyEvent>(&event)) {
      if (key->getKey() == '\n' || key->getKey() == ' ') {
        for (std::size_t i = 0; i < base.children.size(); ++i) {
          if (auto* cb = dynamic_cast<checkbox*>(base.children[i].widget.get())) {
            if (cb->base.focused) {
              select(i);
              return true;
            }
          }
        }
      }
    }
    return false;
  }
};

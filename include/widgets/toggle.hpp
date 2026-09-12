#pragma once

#include <algorithm>

#include "core/widget.hpp"
#include "styling/style.hpp"
#include "utilities/checkboxStyle.hpp"

struct toggle : Widget {
  bool* state{nullptr};
  Style style{};
  checkboxData checkboxStyleData{checkboxStyle::x};

  toggle() { setFocusable(); }
  explicit toggle(bool& externalState) : state(&externalState) { setFocusable(); }
  explicit toggle(checkboxData data) : checkboxStyleData(data) { setFocusable(); }

  toggle& withState(bool& s) {
    state = &s;
    return *this;
  }

  toggle& withStyle(Style s) {
    style = std::move(s);
    return *this;
  }

  toggle& withColours(ColourPair c) {
    style.colours = std::move(c);
    return *this;
  }

  toggle& withPadding(Insets p) {
    padding = p;
    return *this;
  }

  bool isOn() const { return state && *state; }

  void setState(bool s) {
    if (state) *state = s;
  }

  void flip() {
    if (state) *state = !*state;
  }

  char32_t glyph() const {
    if (isOn()) {
      return isFocused() ? checkboxStyleData.checkedFocused : checkboxStyleData.checked;
    }
    return isFocused() ? checkboxStyleData.uncheckedFocused : checkboxStyleData.unchecked;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t horizontalPadding = padding.horizontal();
    std::size_t verticalPadding = padding.vertical();

    std::size_t width = std::clamp(
        horizontalPadding + 1,
        constraints.getMinWidth(),
        constraints.getMaxWidth()
    );
    std::size_t height = std::clamp(
        verticalPadding + 1,
        constraints.getMinHeight(),
        constraints.getMaxHeight()
    );

    return Size{height, width};
  }

  void render(RenderContext& rendercontext) override {
    const Rect& rect = rendercontext.getRect();
    std::size_t horizontalPadding = padding.horizontal();
    std::size_t verticalPadding = padding.vertical();

    if (rect.width <= horizontalPadding || rect.height <= verticalPadding) return;

    std::size_t contentWidth = rect.width - horizontalPadding;
    std::size_t contentHeight = rect.height - verticalPadding;
    if (contentWidth < 1 || contentHeight < 1) return;

    Cell fill{U' ', style};
    for (std::size_t y = 0; y < rect.height; ++y) {
      for (std::size_t x = 0; x < rect.width; ++x) {
        rendercontext.setCell(rect.getX() + x, rect.getY() + y, fill);
      }
    }

    std::size_t x = rect.getX() + padding.left + (contentWidth - 1) / 2;
    std::size_t y = rect.getY() + padding.top + (contentHeight - 1) / 2;

    rendercontext.setCell(x, y, Cell{glyph(), style});
  }
};
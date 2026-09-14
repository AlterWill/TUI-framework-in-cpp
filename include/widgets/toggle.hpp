#pragma once

#include <algorithm>
#include <string>

#include "core/widget.hpp"
#include "input/event.hpp"
#include "styling/style.hpp"
#include "utilities/checkboxStyle.hpp"
#include "utilities/unicode.hpp"

struct toggle : Widget {
  bool* state{nullptr};
  Style style{};
  checkboxData checkboxStyleData{checkboxStyle::x};
  std::u32string prefix{};
  std::u32string suffix{};
  std::u32string label{};

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

  toggle& withPrefix(std::u32string p) {
    prefix = std::move(p);
    return *this;
  }

  toggle& withPrefix(std::string_view p) {
    prefix = unicode::toUtf32(std::string(p));
    return *this;
  }

  toggle& withSuffix(std::u32string s) {
    suffix = std::move(s);
    return *this;
  }

  toggle& withSuffix(std::string_view s) {
    suffix = unicode::toUtf32(std::string(s));
    return *this;
  }

  toggle& withLabel(std::u32string l) {
    label = std::move(l);
    return *this;
  }

  toggle& withLabel(std::string_view l) {
    label = unicode::toUtf32(std::string(l));
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

  bool handleEvent(const Event& event) override {
    if (const auto* me = std::get_if<MouseEvent>(&event)) {
      if (me->getAction() == MouseAction::Press && me->getButton() == MouseButton::Left) {
        flip();
        return true;
      }
    } else if (const auto* ke = std::get_if<keyEvent>(&event)) {
      int k = ke->getKey();
      if ((k == '\n' || k == '\r' || k == ' ' || k == 'x' || k == 'X') && isFocused()) {
        flip();
        return true;
      }
    }
    return false;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t horizontalPadding = padding.horizontal();
    std::size_t verticalPadding = padding.vertical();

    std::size_t contentWidth = prefix.size() + 1 + suffix.size() + label.size();

    std::size_t width = std::clamp(
        horizontalPadding + contentWidth,
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

    std::size_t startX = rect.getX() + padding.left;
    std::size_t startY = rect.getY() + padding.top + (contentHeight - 1) / 2;
    std::size_t curX = startX;

    // Draw prefix
    for (char32_t ch : prefix) {
      if (curX >= rect.getX() + rect.width - padding.right) break;
      rendercontext.setCell(curX++, startY, Cell{ch, style});
    }

    // Draw glyph
    if (curX < rect.getX() + rect.width - padding.right) {
      Style glyphStyle = style;
      if (isFocused()) {
        glyphStyle.setTextStyle(TextStyle::Bold);
      }
      rendercontext.setCell(curX++, startY, Cell{glyph(), glyphStyle});
    }

    // Draw suffix
    for (char32_t ch : suffix) {
      if (curX >= rect.getX() + rect.width - padding.right) break;
      rendercontext.setCell(curX++, startY, Cell{ch, style});
    }

    // Draw label
    for (char32_t ch : label) {
      if (curX >= rect.getX() + rect.width - padding.right) break;
      rendercontext.setCell(curX++, startY, Cell{ch, style});
    }
  }
};
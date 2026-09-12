#pragma once

#include "core/widget.hpp"
#include "input/event.hpp"
#include "widgets/toggle.hpp"

struct checkbox : Widget {
  bool checked{false};
  toggle t;
  WidgetBase& base{t.base};

  char32_t prefix{U'['};
  char32_t suffix{U']'};
  bool prefixEnabled{true};
  bool suffixEnabled{true};

  checkbox() : base(t.base) {
    t.state = &checked;
    base.setFocusable();
  }

  explicit checkbox(checkboxData data) : t(data), base(t.base) {
    t.state = &checked;
    base.setFocusable();
  }

  checkbox& withChecked(bool c) {
    t.setState(c);
    return *this;
  }

  checkbox& withPrefix(char32_t p) {
    prefix = p;
    return *this;
  }

  checkbox& withSuffix(char32_t s) {
    suffix = s;
    return *this;
  }

  checkbox& withPrefixEnabled(bool e) {
    prefixEnabled = e;
    return *this;
  }

  checkbox& withSuffixEnabled(bool e) {
    suffixEnabled = e;
    return *this;
  }

  checkbox& withStyle(Style s) {
    t.style = std::move(s);
    return *this;
  }

  checkbox& withColours(ColourPair c) {
    t.style.colours = std::move(c);
    return *this;
  }

  checkbox& withPadding(Insets p) {
    t.base.padding = p;
    return *this;
  }

  bool isChecked() const { return t.isOn(); }
  void setChecked(bool c) { t.setState(c); }
  void select() { t.setState(true); }
  void deselect() { t.setState(false); }
  void toggle() { t.flip(); }

  bool handleEvent(const Event& event) override {
    if (std::holds_alternative<MouseEvent>(event)) {
      const MouseEvent& me = std::get<MouseEvent>(event);
      if (me.getAction() == MouseAction::Press && me.getButton() == MouseButton::Left) {
        toggle();
        return true;
      }
    } else if (std::holds_alternative<keyEvent>(event)) {
      const keyEvent& ke = std::get<keyEvent>(event);
      if ((ke.getKey() == '\n' || ke.getKey() == ' ') && base.focused) {
        toggle();
        return true;
      }
    }
    return false;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t horizontalPadding = t.base.padding.left + t.base.padding.right;
    std::size_t verticalPadding = t.base.padding.top + t.base.padding.bottom;

    std::size_t glyphCount = 1;
    if (prefixEnabled) glyphCount++;
    if (suffixEnabled) glyphCount++;

    std::size_t width = std::clamp(
        horizontalPadding + glyphCount,
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
    std::size_t horizontalPadding = t.base.padding.left + t.base.padding.right;
    std::size_t verticalPadding = t.base.padding.top + t.base.padding.bottom;

    if (rect.width <= horizontalPadding || rect.height <= verticalPadding) return;

    std::size_t glyphCount = 1;
    if (prefixEnabled) glyphCount++;
    if (suffixEnabled) glyphCount++;

    std::size_t contentWidth = rect.width - horizontalPadding;
    std::size_t contentHeight = rect.height - verticalPadding;
    if (contentWidth < glyphCount || contentHeight < 1) return;

    Cell fill{U' ', t.style};
    for (std::size_t y = 0; y < rect.height; ++y) {
      for (std::size_t x = 0; x < rect.width; ++x) {
        rendercontext.setCell(rect.getX() + x, rect.getY() + y, fill);
      }
    }

    std::size_t startX = rect.getX() + t.base.padding.left + (contentWidth - glyphCount) / 2;
    std::size_t y = rect.getY() + t.base.padding.top + (contentHeight - 1) / 2;

    if (prefixEnabled) {
      rendercontext.setCell(startX, y, Cell{prefix, t.style});
      startX++;
    }
    rendercontext.setCell(startX, y, Cell{t.glyph(), t.style});
    if (suffixEnabled) {
      rendercontext.setCell(startX + 1, y, Cell{suffix, t.style});
    }
  }
};
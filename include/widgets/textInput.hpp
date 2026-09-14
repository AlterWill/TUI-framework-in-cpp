#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "core/widget.hpp"
#include "input/event.hpp"
#include "layout/ScrollContainer.hpp"
#include "utilities/Rect.hpp"
#include "utilities/unicode.hpp"
#include "widgets/text.hpp"

enum class CursorShape { None, Block, Beam, Underline };

struct TextInput : public Widget {
  // State binding & Fallback
  std::string* boundText{nullptr};
  std::string internalText{};
  std::string placeholder{};

  // Configuration
  bool isMultiline{false};
  std::optional<char32_t> maskChar{std::nullopt};
  Style style{};
  Style placeholderStyle{};

  // Composed Subsystems
  std::vector<std::u32string> displayLines{U""};
  Scroll scrollContainer;

  // The widget-owned caret position and its cursor shape.
  Point cursor{0, 0};
  CursorShape shape{CursorShape::Block};

  // Callbacks
  std::function<void(const std::string&)> onChange;
  std::function<void(const std::string&)> onSubmit;
  using Validator = std::function<bool(std::string_view)>;
  using InputFilter = std::function<bool(char32_t)>;

  // Validation & Filtering
  Validator validator;
  InputFilter filter;

  // Constructors
  TextInput() {
    setFocusable();
    initScroll();
    initPlaceholderStyle();
    syncFromText();
    setCursorToEnd();
  }

  explicit TextInput(std::string& externalStr) : boundText(&externalStr) {
    setFocusable();
    initScroll();
    initPlaceholderStyle();
    syncFromText();
    setCursorToEnd();
  }

  TextInput(std::string placeholderText, std::string& externalStr)
      : boundText(&externalStr), placeholder(std::move(placeholderText)) {
    setFocusable();
    initScroll();
    initPlaceholderStyle();
    syncFromText();
    setCursorToEnd();
  }

  TextInput(TextInput&& other) noexcept
      : Widget(std::move(other)),
        boundText(other.boundText),
        internalText(std::move(other.internalText)),
        placeholder(std::move(other.placeholder)),
        isMultiline(other.isMultiline),
        maskChar(other.maskChar),
        style(other.style),
        placeholderStyle(other.placeholderStyle),
        displayLines(std::move(other.displayLines)),
        scrollContainer(std::move(other.scrollContainer)),
        cursor(other.cursor),
        shape(other.shape),
        onChange(std::move(other.onChange)),
        onSubmit(std::move(other.onSubmit)),
        validator(std::move(other.validator)),
        filter(std::move(other.filter)) {
    initScroll();
  }

  TextInput& operator=(TextInput&& other) noexcept {
    if (this != &other) {
      Widget::operator=(std::move(other));
      boundText = other.boundText;
      internalText = std::move(other.internalText);
      placeholder = std::move(other.placeholder);
      isMultiline = other.isMultiline;
      maskChar = other.maskChar;
      style = other.style;
      placeholderStyle = other.placeholderStyle;
      displayLines = std::move(other.displayLines);
      scrollContainer = std::move(other.scrollContainer);
      cursor = other.cursor;
      shape = other.shape;
      onChange = std::move(other.onChange);
      onSubmit = std::move(other.onSubmit);
      validator = std::move(other.validator);
      filter = std::move(other.filter);
      initScroll();
    }
    return *this;
  }

  // Builder Methods
  TextInput& withText(std::string& str) {
    boundText = &str;
    syncFromText();
    return *this;
  }
  TextInput& withValidator(Validator v) {
    validator = std::move(v);
    return *this;
  }

  TextInput& withFilter(InputFilter f) {
    filter = std::move(f);
    return *this;
  }

  TextInput& withPlaceholder(std::string ph) {
    placeholder = std::move(ph);
    syncFromText();
    return *this;
  }

  TextInput& withMultiline(bool multiline = true) {
    isMultiline = multiline;
    return *this;
  }

  TextInput& withMask(char mask) {
    maskChar = static_cast<char32_t>(static_cast<unsigned char>(mask));
    syncFromText();
    return *this;
  }

  TextInput& withMask(char32_t mask) {
    maskChar = mask;
    syncFromText();
    return *this;
  }

  TextInput& withStyle(Style s) {
    style = std::move(s);
    initPlaceholderStyle();
    return *this;
  }

  TextInput& withPlaceholderStyle(Style s) {
    placeholderStyle = std::move(s);
    return *this;
  }

  TextInput& withColours(ColourPair c) {
    style.colours = std::move(c);
    initPlaceholderStyle();
    return *this;
  }

  TextInput& withPadding(Insets p) {
    padding = p;
    scrollContainer.withPadding(p);
    return *this;
  }

  TextInput& withCursorShape(CursorShape s) {
    shape = s;
    return *this;
  }

  void setCursorToEnd() {
    auto lines = getRawLines();
    if (lines.empty()) {
      cursor.setXY(0, 0);
    } else {
      std::size_t lastRow = lines.size() - 1;
      cursor.setXY(lines[lastRow].size(), lastRow);
    }
  }

  void onFocus() override {
    setCursorToEnd();
  }

  TextInput& onChangeCallback(std::function<void(const std::string&)> cb) {
    onChange = std::move(cb);
    return *this;
  }

  TextInput& onSubmitCallback(std::function<void(const std::string&)> cb) {
    onSubmit = std::move(cb);
    return *this;
  }

  const std::string& getText() const {
    if (boundText) return *boundText;
    return internalText;
  }

  void setText(const std::string& text) {
    if (boundText) {
      *boundText = text;
    } else {
      internalText = text;
    }
    syncFromText();
    if (onChange) onChange(getText());
  }

  bool isValid() const {
    if (!validator) return true;
    return validator(getText());
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t padW = padding.horizontal();
    std::size_t padH = padding.vertical();

    syncFromText();

    if (!isMultiline) {
      std::size_t contentW = 0;
      if (!displayLines.empty()) {
        contentW = displayLines[0].size();
      }
      std::size_t preferredW = contentW + padW + 1;  // +1 for cursor space
      std::size_t clampedW = std::clamp(preferredW, constraints.getMinWidth(), constraints.getMaxWidth());
      std::size_t preferredH = 1 + padH;
      std::size_t clampedH = std::clamp(preferredH, constraints.getMinHeight(), constraints.getMaxHeight());
      return Size{clampedH, clampedW};
    }

    std::size_t lineCount = displayLines.empty() ? 1 : displayLines.size();
    std::size_t maxLineW = 0;
    for (const auto& line : displayLines) {
      maxLineW = std::max(maxLineW, line.size());
    }

    std::size_t preferredW = maxLineW + padW + 1;
    std::size_t preferredH = lineCount + padH;

    std::size_t clampedW = std::clamp(preferredW, constraints.getMinWidth(), constraints.getMaxWidth());
    std::size_t clampedH = std::clamp(preferredH, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{clampedH, clampedW};
  }

  void layout(const Rect& rect) override { scrollContainer.layout(rect); }

  void render(RenderContext& rendercontext) override {
    syncFromText();

    const Rect& rect = rendercontext.getRect();
    std::size_t padL = padding.left;
    std::size_t padR = padding.right;
    std::size_t padT = padding.top;
    std::size_t padB = padding.bottom;
    std::size_t padW = padL + padR;
    std::size_t padH = padT + padB;

    std::size_t visibleW = rect.width > padW ? rect.width - padW : 0;
    std::size_t visibleH = rect.height > padH ? rect.height - padH : 0;

    // Fill background with input style
    Cell fill{U' ', style};
    for (std::size_t y = 0; y < rect.height; ++y) {
      for (std::size_t x = 0; x < rect.width; ++x) {
        rendercontext.setCell(rect.x + x, rect.y + y, fill);
      }
    }

    if (visibleW == 0 || visibleH == 0) return;

    // Update child text style
    bool isShowingPlaceholder = getText().empty() && !placeholder.empty();
    if (auto* textWidget = dynamic_cast<Text*>(scrollContainer.base.child.widget.get())) {
      textWidget->withStyle(isShowingPlaceholder ? placeholderStyle : style);
    }

    // Measure virtual text bounds for scrolling
    std::size_t virtualLineCount = displayLines.empty() ? 1 : displayLines.size();
    std::size_t virtualMaxW = 0;
    for (const auto& line : displayLines) {
      virtualMaxW = std::max(virtualMaxW, line.size());
    }
    scrollContainer.base.child.measured = Size{virtualLineCount, virtualMaxW + 1};

    // Keep cursor inside viewport
    ensureCursorVisible(visibleW, visibleH);

    // Render text content through ScrollContainer
    scrollContainer.setRectForChild(rect);
    scrollContainer.render(rendercontext);

    if (shape != CursorShape::None) {
      // Handle cursor rendering — draw a Unicode character at cursor position
      // instead of using the actual terminal cursor. The terminal cursor stays hidden.
      // Block: █ (full block) when focused, □ (hollow box) when unfocused, at cursor position
      // Beam: │ (vertical line) at cursor.x - 1 (on the last character), or at 0 if at start
      // Underline: ▁ (lower block) at cursor position
      bool active = focused;
      std::size_t cursorY = cursor.getY();
      std::size_t offsetY = scrollContainer.scrollBase.offsetY;
      std::size_t offsetX = scrollContainer.scrollBase.offsetX;

      if (cursorY >= offsetY && cursorY < offsetY + visibleH) {
        std::size_t drawY = rect.y + padT + (cursorY - offsetY);

        auto renderCursorChar = [&](std::size_t indicatorX, char32_t cursorCharFocused, char32_t cursorCharUnfocused) {
          if (indicatorX < offsetX || indicatorX >= offsetX + visibleW) return;
          std::size_t drawX = rect.x + padL + (indicatorX - offsetX);

          Cell cell = rendercontext.at(drawX, drawY);
          cell.setGlyph(active ? cursorCharFocused : cursorCharUnfocused);
          cell.style.colours = style.colours;
          rendercontext.setCell(drawX, drawY, cell);
        };

        switch (shape) {
          case CursorShape::Block: {
            std::size_t indicatorX = cursor.x;
            renderCursorChar(indicatorX, U'█', U'▯');
            break;
          }
          case CursorShape::Beam: {
            std::size_t indicatorX = (cursor.x > 0) ? cursor.x - 1 : 0;
            renderCursorChar(indicatorX, U'│', U'│');
            break;
          }
          case CursorShape::Underline: {
            std::size_t indicatorX = cursor.x;
            renderCursorChar(indicatorX, U'▁', U'▁');
            break;
          }
          case CursorShape::None:
            break;
        }
      }
    }
  }

  bool handleEvent(const Event& event) override {
    if (const auto* mouse = std::get_if<MouseEvent>(&event)) {
      if (mouse->action == MouseAction::Press && mouse->button == MouseButton::Left) {
        setFocused(true);
        const Rect& rect = scrollContainer.scrollBase.rect;
        std::size_t padL = padding.left;
        std::size_t padT = padding.top;

        if (mouse->x >= rect.x + padL && mouse->y >= rect.y + padT) {
          std::size_t clickX = mouse->x - (rect.x + padL) + scrollContainer.scrollBase.offsetX;
          std::size_t clickY = mouse->y - (rect.y + padT) + scrollContainer.scrollBase.offsetY;

          auto lines = getRawLines();
          if (clickY >= lines.size()) {
            clickY = lines.empty() ? 0 : lines.size() - 1;
          }
          std::size_t lineLen = lines.empty() ? 0 : lines[clickY].size();
          clickX = std::min(clickX, lineLen);

          cursor.setXY(clickX, clickY);
        }
        return true;
      }
    } else if (const auto* key = std::get_if<keyEvent>(&event)) {
      if (!isFocused()) return false;

      return handleKeyEvent(*key);
    }
    return false;
  }

 private:
  void initScroll() {
    scrollContainer.scrollBase.direction = ScrollDirection::Both;
    scrollContainer.base.child.widget = std::make_unique<Text>(displayLines);
    scrollContainer.withPadding(padding);
  }

  void initPlaceholderStyle() {
    placeholderStyle = style;
    placeholderStyle.setTextStyle(TextStyle::Dim);
  }

  std::vector<std::u32string> getRawLines() const {
    const std::string& cur = getText();
    std::u32string u32 = unicode::toUtf32(cur);
    std::vector<std::u32string> lines;
    std::u32string currentLine;
    for (char32_t ch : u32) {
      if (ch == U'\n') {
        lines.push_back(currentLine);
        currentLine.clear();
      } else if (ch != U'\r') {
        currentLine.push_back(ch);
      }
    }
    lines.push_back(currentLine);
    return lines;
  }

  void saveRawLines(const std::vector<std::u32string>& lines) {
    std::u32string joined;
    for (std::size_t i = 0; i < lines.size(); ++i) {
      if (i > 0) joined.push_back(U'\n');
      joined += lines[i];
    }
    std::string utf8 = unicode::toUtf8(joined);
    if (boundText) {
      *boundText = utf8;
    } else {
      internalText = utf8;
    }
    syncFromText();
    if (onChange) onChange(getText());
  }

  void syncFromText() {
    const std::string& cur = getText();
    if (cur.empty() && !placeholder.empty()) {
      displayLines.clear();
      std::u32string ph32 = unicode::toUtf32(placeholder);
      std::u32string curLine;
      for (char32_t ch : ph32) {
        if (ch == U'\n') {
          displayLines.push_back(curLine);
          curLine.clear();
        } else if (ch != U'\r') {
          curLine.push_back(ch);
        }
      }
      displayLines.push_back(curLine);
      return;
    }

    auto lines = getRawLines();
    if (maskChar.has_value()) {
      for (auto& line : lines) {
        for (auto& ch : line) {
          ch = maskChar.value();
        }
      }
    }
    displayLines = std::move(lines);
  }

  void ensureCursorVisible(std::size_t visibleW, std::size_t visibleH) {
    if (visibleW == 0 || visibleH == 0) return;

    std::size_t cursorX = cursor.getX();
    std::size_t cursorY = cursor.getY();
    std::size_t& offsetX = scrollContainer.scrollBase.offsetX;
    std::size_t& offsetY = scrollContainer.scrollBase.offsetY;

    if (cursorX < offsetX) {
      offsetX = cursorX;
    } else if (cursorX >= offsetX + visibleW) {
      offsetX = cursorX - visibleW + 1;
    }

    if (cursorY < offsetY) {
      offsetY = cursorY;
    } else if (cursorY >= offsetY + visibleH) {
      offsetY = cursorY - visibleH + 1;
    }
  }

  // Column (relative to the widget) the caret marker should be drawn on.
  // A Beam caret is shifted back one cell so it sits on the last character.
  bool tryInsert(char32_t ch) {
    if (filter && !filter(ch)) return false;

    auto lines = getRawLines();
    std::size_t row = cursor.getY();
    std::size_t col = cursor.getX();
    if (row >= lines.size()) {
      row = lines.empty() ? 0 : lines.size() - 1;
    }
    if (!lines.empty() && col > lines[row].size()) {
      col = lines[row].size();
    }

    if (ch == U'\n' && isMultiline) {
      // Multiline: split current line
      std::u32string rightPart = lines[row].substr(col);
      lines[row] = lines[row].substr(0, col);
      lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(row) + 1, rightPart);
      cursor.setXY(0, row + 1);
    } else {
      lines[row].insert(lines[row].begin() + static_cast<std::ptrdiff_t>(col), ch);
      cursor.setXY(col, row);
      cursor.incrementX();
    }
    saveRawLines(lines);
    return true;
  }

  bool handleKeyEvent(const keyEvent& ke) {
    auto lines = getRawLines();
    std::size_t row = cursor.getY();
    std::size_t col = cursor.getX();

    if (row >= lines.size()) {
      row = lines.empty() ? 0 : lines.size() - 1;
    }
    if (!lines.empty() && col > lines[row].size()) {
      col = lines[row].size();
    }
    cursor.setXY(col, row);

    int k = ke.getKey();

    // 1. Enter
    if (k == '\n' || k == '\r') {
      if (!isMultiline) {
        if (onSubmit) onSubmit(getText());
        return true;
      }
      tryInsert(U'\n');
      return true;
    }

    // 2. Backspace
    if (k == 127 || k == 8 || k == '\b') {
      if (col > 0) {
        lines[row].erase(col - 1, 1);
        cursor.decrementX();
        saveRawLines(lines);
        return true;
      }
      if (col == 0 && row > 0 && isMultiline) {
        std::size_t prevLen = lines[row - 1].size();
        lines[row - 1] += lines[row];
        lines.erase(lines.begin() + static_cast<std::ptrdiff_t>(row));
        cursor.setXY(prevLen, row - 1);
        saveRawLines(lines);
        return true;
      }
      return true;
    }

    // 3. Delete
    if (k == DEL_KEY) {
      if (col < lines[row].size()) {
        lines[row].erase(col, 1);
        saveRawLines(lines);
        return true;
      }
      if (col == lines[row].size() && row + 1 < lines.size() && isMultiline) {
        lines[row] += lines[row + 1];
        lines.erase(lines.begin() + static_cast<std::ptrdiff_t>(row) + 1);
        saveRawLines(lines);
        return true;
      }
      return true;
    }

    // 4. Arrow Left
    if (k == ARROW_LEFT) {
      if (col > 0) {
        cursor.decrementX();
      } else if (row > 0 && isMultiline) {
        cursor.setXY(lines[row - 1].size(), row - 1);
      }
      return true;
    }

    // 5. Arrow Right
    if (k == ARROW_RIGHT) {
      if (col < lines[row].size()) {
        cursor.incrementX();
      } else if (row + 1 < lines.size() && isMultiline) {
        cursor.setXY(0, row + 1);
      }
      return true;
    }

    // 6. Arrow Up
    if (k == ARROW_UP) {
      if (isMultiline && row > 0) {
        std::size_t newCol = std::min(col, lines[row - 1].size());
        cursor.setXY(newCol, row - 1);
      }
      return true;
    }

    // 7. Arrow Down
    if (k == ARROW_DOWN) {
      if (isMultiline && row + 1 < lines.size()) {
        std::size_t newCol = std::min(col, lines[row + 1].size());
        cursor.setXY(newCol, row + 1);
      }
      return true;
    }

    // 8. Home Key
    if (k == HOME_KEY) {
      cursor.setXY(0, row);
      return true;
    }

    // 9. End Key
    if (k == END_KEY) {
      cursor.setXY(lines[row].size(), row);
      return true;
    }

    // 10. Printable character insertion
    if (k >= 32 && k != 127) {
      tryInsert(static_cast<char32_t>(k));
      return true;
    }

    return false;
  }
};

// Aliases and Convenience Constructors
using textInput = TextInput;

inline TextInput TextArea(std::string& text) {
  TextInput input(text);
  input.withMultiline(true);
  return input;
}

inline TextInput TextArea(std::string placeholder, std::string& text) {
  TextInput input(std::move(placeholder), text);
  input.withMultiline(true);
  return input;
}

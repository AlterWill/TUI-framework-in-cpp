#pragma once

#include <cstddef>
#include <functional>
#include <string_view>

// Reusable filters and validators for TextInput widgets.
//
// A filter decides whether a single input/edit operation should be accepted
// (see TextInput::withFilter). A validator decides whether the current value is
// valid (see TextInput::withValidator / TextInput::isValid).

namespace validators {

// Matches a possibly-signed integer (e.g. "42", "-7", "+100").
inline std::function<bool(std::string_view)> integer() {
  return [](std::string_view value) {
    if (value.empty()) return false;
    std::size_t i = 0;
    if (value[i] == '-' || value[i] == '+') {
      if (value.size() == 1) return false;
      ++i;
    }
    for (; i < value.size(); ++i) {
      if (value[i] < '0' || value[i] > '9') return false;
    }
    return true;
  };
}

// Matches a decimal number (e.g. "3.14", "-0.5", "100"). A single '.' is
// allowed and at least one digit is required.
inline std::function<bool(std::string_view)> number() {
  return [](std::string_view value) {
    if (value.empty()) return false;
    std::size_t i = 0;
    if (value[i] == '-' || value[i] == '+') {
      if (value.size() == 1) return false;
      ++i;
    }
    bool seenDigit = false;
    bool seenDot = false;
    for (; i < value.size(); ++i) {
      const char c = value[i];
      if (c >= '0' && c <= '9') {
        seenDigit = true;
      } else if (c == '.' && !seenDot) {
        seenDot = true;
      } else {
        return false;
      }
    }
    return seenDigit;
  };
}

// Matches a plausibly-shaped email address: local@domain.tld. This is a
// practical rule, not an RFC-complete parser.
inline std::function<bool(std::string_view)> email() {
  return [](std::string_view value) {
    const std::size_t at = value.find('@');
    if (at == std::string_view::npos || at == 0 || at + 1 >= value.size()) return false;
    const std::size_t lastDot = value.rfind('.');
    if (lastDot == std::string_view::npos || lastDot <= at + 1 || lastDot == value.size() - 1) return false;
    return true;
  };
}

}  // namespace validators

namespace filters {

// Allows only ASCII digits 0-9.
inline std::function<bool(char32_t)> digits() {
  return [](char32_t ch) { return ch >= U'0' && ch <= U'9'; };
}

}  // namespace filters
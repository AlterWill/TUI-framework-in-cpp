#pragma once
#include <string>

namespace unicode {

inline std::string toUtf8(char32_t cp) {
  std::string result;

  if (cp <= 0x7F) {
    result.push_back(static_cast<char>(cp));
  } else if (cp <= 0x7FF) {
    result.push_back(static_cast<char>(0b11000000 | (cp >> 6)));
    result.push_back(static_cast<char>(0b10000000 | (cp & 0x3F)));
  } else if (cp <= 0xFFFF) {
    result.push_back(static_cast<char>(0b11100000 | (cp >> 12)));
    result.push_back(static_cast<char>(0b10000000 | ((cp >> 6) & 0x3F)));
    result.push_back(static_cast<char>(0b10000000 | (cp & 0x3F)));
  } else {
    result.push_back(static_cast<char>(0b11110000 | (cp >> 18)));
    result.push_back(static_cast<char>(0b10000000 | ((cp >> 12) & 0x3F)));
    result.push_back(static_cast<char>(0b10000000 | ((cp >> 6) & 0x3F)));
    result.push_back(static_cast<char>(0b10000000 | (cp & 0x3F)));
  }

  return result;
}

inline std::string toUtf8(const std::u32string &text) {
  std::string result;
  for (char32_t cp : text) {
    result += toUtf8(cp);
  }
  return result;
}

inline std::u32string toUtf32(const std::string &text) {
  std::u32string result;
  std::size_t i = 0;
  while (i < text.size()) {
    unsigned char c = static_cast<unsigned char>(text[i]);
    if (c <= 0x7F) {
      result.push_back(c);
      i += 1;
    } else if ((c & 0xE0) == 0xC0) {
      if (i + 1 < text.size()) {
        char32_t cp = static_cast<char32_t>(((c & 0x1F) << 6) |
                      (static_cast<unsigned char>(text[i + 1]) & 0x3F));
        result.push_back(cp);
        i += 2;
      } else {
        break;
      }
    } else if ((c & 0xF0) == 0xE0) {
      if (i + 2 < text.size()) {
        char32_t cp = static_cast<char32_t>(((c & 0x0F) << 12) |
                      ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) |
                      (static_cast<unsigned char>(text[i + 2]) & 0x3F));
        result.push_back(cp);
        i += 3;
      } else {
        break;
      }
    } else if ((c & 0xF8) == 0xF0) {
      if (i + 3 < text.size()) {
        char32_t cp = static_cast<char32_t>(((c & 0x07) << 18) |
                      ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12) |
                      ((static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6) |
                      (static_cast<unsigned char>(text[i + 3]) & 0x3F));
        result.push_back(cp);
        i += 4;
      } else {
        break;
      }
    } else {
      i += 1;
    }
  }
  return result;
}

} // namespace unicode

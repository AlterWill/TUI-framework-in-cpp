module;
#include <string>

export module unicode;

export namespace unicode {

std::string toUtf8(char32_t cp) {
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

std::string toUtf8(const std::u32string &text) {
  std::string result;

  for (char32_t cp : text) {
    result += toUtf8(cp);
  }

  return result;
}

} // namespace unicode

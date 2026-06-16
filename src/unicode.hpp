#pragma once
#include <string>

namespace unicode {

std::string toUtf8(char32_t cp);

std::string toUtf8(const std::u32string &text);

} // namespace unicode

#pragma once

#include <cmath>
#include <string>
#include <vector>

inline std::vector<std::u32string> splitStringByChar(const std::u32string& text, char32_t delimiter) {
  std::vector<std::u32string> lines;
  std::u32string line;
  for (auto ch : text) {
    if (ch == delimiter) {
      if (line != U"") {
        lines.push_back(line);
        line.clear();
      }
    } else {
      line += ch;
    }
  }
  if (!line.empty()) {
    lines.push_back(line);
  }
  return lines;
}

inline std::vector<std::u32string> splitVectorByChar(const std::vector<std::u32string>& array, char32_t delimiter) {
  std::vector<std::u32string> lines;
  std::u32string line;
  for (const std::u32string& text : array) {
    for (auto ch : text) {
      if (ch == delimiter) {
        if (!line.empty()) {
          lines.push_back(line);
          line.clear();
        }
      } else {
        line += ch;
      }
    }
  }
  if (!line.empty()) {
    lines.push_back(line);
  }
  return lines;
}

inline std::vector<std::u32string> splitByLength(const std::u32string& text, std::size_t length) {
  std::vector<std::u32string> lines;
  if (length <= 0) {
    return lines;
  }
  std::u32string line = U"";
  for (auto ch : text) {
    line += ch;
    if (line.size() == length) {
      lines.push_back(line);
      line.clear();
    }
  }
  if (!line.empty()) {
    lines.push_back(line);
  }
  return lines;
}

inline std::vector<std::u32string> convertStringToParagraph(const std::u32string& text, std::size_t length) {
  std::vector<std::u32string> lines;
  if (text.empty() || length <= 0) {
    return lines;
  }
  if(text.size() <= length) return std::vector<std::u32string>(1,text);
  std::vector<std::u32string> words = splitStringByChar(text, ' ');
  std::u32string line;
  for (const std::u32string& word : words) {
    if (word.size() > length) {
      if (!line.empty()) {
        lines.push_back(line);
        line.clear();
      }

      std::vector<std::u32string> chunks = splitByLength(word, length);

      for (const std::u32string& chunk : chunks) {
        lines.push_back(chunk);
      }
      continue;
    }
    if (line.empty()) {
      line = word;
    } else if (line.size() + 1 + word.size() <= length) {
      line += U" " + word;
    } else {
      lines.push_back(line);
      line = word;
    }
  }
  if (!line.empty()) {
    lines.push_back(line);
  }
  return lines;
}

inline std::size_t findHeightForParagraph(const std::u32string& text, std::size_t length){
  if(length == 0) return 0;
  return static_cast<std::size_t>(std::ceil(static_cast<float>(text.size())/static_cast<float>(length)));
}


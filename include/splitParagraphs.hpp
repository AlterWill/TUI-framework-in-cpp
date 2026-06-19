#pragma once

#include <string>
#include <vector>

inline std::vector<std::string> splitStringByChar(const std::string &text, char delimiter) {
  std::vector<std::string> lines;
  std::string line;
  for (char ch : text) {
    if (ch == delimiter) {
      if (line != "") {
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

inline std::vector<std::string> splitVectorByChar(const std::vector<std::string> &array,
                                                  char delimiter) {
  std::vector<std::string> lines;
  std::string line;
  for (const std::string &text : array) {
    for (char ch : text) {
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

inline std::vector<std::string> splitByLength(const std::string &text, int length) {
  std::vector<std::string> lines;
  if (length <= 0) {
    return lines;
  }
  std::string line = "";
  for (char ch : text) {
    line += ch;
    if ((int)line.size() == length) {
      lines.push_back(line);
      line.clear();
    }
  }
  if (!line.empty()) {
    lines.push_back(line);
  }
  return lines;
}

inline std::vector<std::string> convertStringToParagraph(const std::string &text, int length) {
  std::vector<std::string> lines;
  if (text.empty() || length <= 0) {
    return lines;
  }
  std::vector<std::string> words = splitStringByChar(text, ' ');
  std::string line;
  for (const std::string &word : words) {
    if ((int)word.size() > length) {
      if (!line.empty()) {
        lines.push_back(line);
        line.clear();
      }

      std::vector<std::string> chunks = splitByLength(word, length);

      for (const std::string &chunk : chunks) {
        lines.push_back(chunk);
      }
      continue;
    }
    if (line.empty()) {
      line = word;
    } else if ((int)(line.size() + 1 + word.size()) <= length) {
      line += " " + word;
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

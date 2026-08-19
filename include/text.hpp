#pragma once

#include <string>
#include <vector>

#include "splitParagraphs.hpp"
#include "widget.hpp"

enum class Alignment {
  left,
  right,
  center,
};

class Text : public Widget {
 public:
  std::string& text;
  Style style;

  Text(std::string& t, Style s = Style()) : text(t), style(s) {}

  void setText(std::string& t) { text = std::move(t); }

  void setStyle(Style s) { style = std::move(s); }

  void setColourPair(ColourPair c) { style.colours = std::move(c); }

  void layout() override {};

  SizeConstraints measure() override {
    SizeConstraints resultConstraint{Size{0, 0}, Size{0, 0}};
    if (text.size() == 0) return resultConstraint;
    std::vector<std::string> words = splitStringByChar(text, ' ');

    std::size_t maximumWordLength{};
    for (const std::string& word : words) {
      if (word.size() < maximumWordLength) maximumWordLength = word.size();
    }

    resultConstraint.setMinSize(Size{words.size(),maximumWordLength});
    resultConstraint.setMaxSize(Size{1,text.size()});

    return resultConstraint;
  }

  void render(RenderContext& rendercontext) override {
    if( rendercontext.clip.getWidth() == 0 || rendercontext.clip.getHeight() == 0) return;
    std::vector<std::string> words = convertStringToParagraph(text, rendercontext.clip.getWidth());

    // we have the string stored in text 
    // calculate what needs to be displayed by offset with this text 
    // by using it as a cache 
    //
    // also we need to handle the text having unicode characters
    //
    // how to get the height and width tho?
  };
};

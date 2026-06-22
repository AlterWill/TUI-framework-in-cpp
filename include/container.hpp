#pragma once

#include "widget.hpp"
#include <algorithm>
#include <memory>
#include <vector>

enum class LayoutType { Grid, Row, Column };

class Container : public Widget {
public:
  LayoutType layoutType;
  std::vector<std::unique_ptr<Widget>> children;

  void render(frameBuffer &fb) override;

  void addChild(std::unique_ptr<Widget> widget) {
    children.push_back(std::move(widget));
  }

  void removeChild(std::unique_ptr<Widget> widget) {
    auto index = std::find(children.begin(), children.end(), widget);

    if (index != children.end()) {
      children.erase(index);
    }
  }

  void clearChildren() {
    children.clear();
  }
};

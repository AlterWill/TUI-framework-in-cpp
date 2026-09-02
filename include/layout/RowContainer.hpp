#pragma once

#include <algorithm>

#include "core/multiChildWidget.hpp"

// in row, there is one row and there is multiple children in that row
// width gets divided, not height
class Row : public MultiChildWidget {
 public:
  Row() = default;

  Row(std::vector<std::unique_ptr<Widget>> c) { children = std::move(c); }

  Size measure(const SizeConstraints& constraints) override {
    
  }

  virtual bool handleEvent(const Event&) override { return false; };

  void setRectForChildren() override {
    
  }
};

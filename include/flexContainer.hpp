#pragma once

#include <algorithm>

#include "multiChildWidget.hpp"

enum FlexDirection { Row, Column };

class Flex : public MultiChildWidget {
  FlexDirection flexdirection;

  Flex(FlexDirection dir = FlexDirection::Row) : flexdirection(dir) {}

  void setRectForChildren() override {
    int totalFlex{}, fillCount{};
    for (auto& child : children) {
      if (child->getFlex() < 0) {
        fillCount++;
        continue;
      }
      totalFlex += child->getFlex();
    }

    // calculate remaining width that is going to child that has flex < 0 ( fill )

    std::size_t totalHeight{};
    std::size_t totalWidth{};
    for (auto& child : children) {
      int flex = child->getFlex();
      if (child->getFlex() < 0) continue;
      // set teh width and height for child who has flex > 0
      switch (flexdirection) {
        case Row: {
          double ratio = static_cast<double>(flex) / totalFlex;
          std::size_t width =
              std::clamp(static_cast<std::size_t>(ratio * rect.width), child->getMinWidth(), child->getMaxWidth());
          child->setWidth(width);
          totalWidth += width;
          child->setHeight(rect.height);
          break;
        }
        case Column: {
          double ratio = static_cast<double>(flex) / totalFlex;
          std::size_t height =
              std::clamp(static_cast<std::size_t>(ratio * rect.width), child->getMinWidth(), child->getMaxWidth());
          child->setWidth(rect.width);
          child->setHeight(height);
          totalHeight += height;
          break;
        }
      }
    }

    std::size_t remainingWidth = rect.width > totalWidth ? rect.width - totalWidth : 0;
    std::size_t remainingHeight = rect.height > totalHeight ? rect.height - totalHeight : 0;

    Widget* lastFillChild = nullptr;
    if (fillCount > 0) {
      std::size_t flexWidth = remainingWidth / fillCount;
      std::size_t flexHeight = remainingHeight / fillCount;
      for (auto& child : children) {
        if (child->getFlex() > 0) continue;
        // set teh width and height for child who has flex > 0
        switch (flexdirection) {
          case Row: {
            std::size_t width = std::clamp(flexWidth, child->getMinWidth(), child->getMaxWidth());
            child->setWidth(width);
            child->setHeight(rect.height);
            remainingWidth -= width;
            lastFillChild = child.get();
            break;
          }

          case Column: {
            std::size_t height = std::clamp(flexHeight, child->getMinHeight(), child->getMaxHeight());
            child->setWidth( rect.width);
            child->setHeight(height);
            remainingHeight -= height;
            lastFillChild = child.get();
            break;
          }
        }
      }
    }

    if(lastFillChild){
      switch (flexdirection) {
        case Row: {
          lastFillChild->rect.width += remainingWidth;
        }
        case Column:{
          lastFillChild->rect.height += remainingHeight;
        }
      }
    }

    std::size_t currentX = rect.x;
    std::size_t currentY = rect.y;
    for(auto& child : children){
      child->rect.x = currentX;
      child->rect.y = currentY;

      switch(flexdirection){
        case Row:{
          currentX += child->getWidth();
          break;
        }
        case Column:{
          currentY += child->getHeight();
          break;
        }
      }
    }
  }
};

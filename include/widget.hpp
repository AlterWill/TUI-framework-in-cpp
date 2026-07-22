#pragma once

#include "frameBuffer.hpp"
#include "insets.hpp"
#include "rect.hpp"

class Widget {
 protected:
  bool focusable{false};

 public:
  Rect rect;
  LayoutProperies layoutProperies;
  Rect clip;
  Widget* parent = nullptr;
  bool focused{false};

  Insets padding;
  Insets margin;

  void setClip() {
    clip.height = std::max(rect.height - padding.top - padding.bottom, static_cast<std::size_t>(0));
    clip.width = std::max(rect.width - padding.right - padding.left, static_cast<std::size_t>(0));
    clip.x = rect.x + padding.left;
    clip.y = rect.y + padding.top;
  }

  void setWidth(std::size_t width){
    rect.width = width;
  }

  void setHeight(std::size_t height){
    rect.height = height;
  }


  std::size_t getWidth(){
    return rect.width ;
  }

  std::size_t getHeight(){
    return rect.height ;
  }

  bool containsPoint(std::size_t x, std::size_t y) {
    if (rect.x <= x && x < rect.x + rect.width && rect.y <= y && y < rect.y + rect.height) {
      return true;
    }
    return false;
  }

  void setFlex(int flex){
    layoutProperies.flex = flex;
  }

  int getFlex(){
    return layoutProperies.flex;
  }

  void setMinWidth(std::size_t minWidth){
    layoutProperies.minWidth = minWidth;
  }

  std::size_t getMinWidth(){
    return layoutProperies.minWidth;
  }

  std::size_t getMaxWidth(){
    return layoutProperies.maxWidth.value();
  }

  std::size_t getMaxHeight(){
    return layoutProperies.maxHeight.value();
  }

  std::size_t getMinHeight(){
    return layoutProperies.minHeight;
  }

  void onFocus() { focused = true; }

  void onUnFocus() { focused = false; }

  bool isFocusable() { return focusable; }

  void disableFocusable() { focusable = false;}

  std::size_t childrenSize() { return 0; }

  virtual std::vector<Widget*> getChildren() { return {}; }

  void setPadding(Insets p) {
    padding = p;
    setClip();
  }

  void setMargin(Insets m) { margin = m; }

  void setRect(int x, int y, int height, int width) {
    rect.x = x;
    rect.y = y;
    rect.height = height;
    rect.width = width;
    setClip();
  }

  virtual void render(frameBuffer& fb) = 0;
  virtual void layout() = 0;
  virtual bool handleEvent(const Event&) { return false; };

  virtual ~Widget() {}
};

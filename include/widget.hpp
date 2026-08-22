#pragma once

#include "Rect.hpp"
#include "SizeConstraints.hpp"
#include "event.hpp"
#include "insets.hpp"
#include "renderContext.hpp"

class Widget {
 protected:
  bool focusable{false};

 public:
  Rect rect;
  LayoutProperties layoutProperties;
  Widget* parent = nullptr;
  bool focused{false};

  Insets padding;
  Insets margin;

  // Width / Height
  void setWidth(std::size_t width) { rect.setWidth(width); }
  void setHeight(std::size_t height) { rect.setHeight(height); }
  std::size_t getWidth() const { return rect.getWidth(); }
  std::size_t getHeight() const { return rect.getHeight(); }

  // X / Y position
  void setX(std::size_t x) { rect.setX(x); }
  void setY(std::size_t y) { rect.setY(y); }
  std::size_t getX() const { return rect.getX(); }
  std::size_t getY() const { return rect.getY(); }

  // Focus
  bool isFocused() const { return focused; }
  void setFocused(bool val) { focused = val; }

  // Parent
  Widget* getParent() const { return parent; }
  void setParent(Widget* val) { parent = val; }

  // Padding / Margin
  Insets getPadding() const { return padding; }
  Insets getMargin() const { return margin; }

  bool containsPoint(std::size_t x, std::size_t y) {
    if (rect.x <= x && x < rect.x + rect.width && rect.y <= y && y < rect.y + rect.height) {
      return true;
    }
    return false;
  }

  // Flex
  void setFlex(int flex) { layoutProperties.flex = flex; }
  int getFlex() const { return layoutProperties.flex; }

  // Min / Max size constraints
  void setMinWidth(std::size_t minWidth) { layoutProperties.constraints.setMinWidth(minWidth); }
  std::size_t getMinWidth() const { return layoutProperties.constraints.getMinWidth(); }

  void setMaxWidth(std::size_t maxWidth) { layoutProperties.constraints.setMaxWidth(maxWidth); }
  std::size_t getMaxWidth() const { return layoutProperties.constraints.getMaxWidth(); }

  void setMinHeight(std::size_t minHeight) { layoutProperties.constraints.setMinHeight(minHeight); }
  std::size_t getMinHeight() const { return layoutProperties.constraints.getMinHeight(); }

  void setMaxHeight(std::size_t maxHeight) { layoutProperties.constraints.setMaxHeight(maxHeight); }
  std::size_t getMaxHeight() const { return layoutProperties.constraints.getMaxHeight(); }

  void onFocus() { focused = true; }
  void onUnFocus() { focused = false; }

  bool isFocusable() const { return focusable; }
  void disableFocusable() { focusable = false; }

  std::size_t childrenSize() { return 0; }

  virtual std::vector<Widget*> getChildren() { return {}; }

  void setPadding(Insets p) {
    padding = p;
    setClip();
  }

  void setMargin(Insets m) { margin = m; }

  void setRect(std::size_t x, std::size_t y, std::size_t height, std::size_t width) {
    rect.x = x;
    rect.y = y;
    rect.height = height;
    rect.width = width;
    setClip();
  }

  void setRect(Rect r) {
    rect = r;
    setClip();
  }

  virtual Size intrinsicSize() = 0;
  virtual Size measure(const SizeConstraints& constraints) = 0;
  virtual void render(RenderContext& rendercontext) = 0;
  virtual void layout() = 0;
  virtual bool handleEvent(const Event&) { return false; }

  virtual ~Widget() {}

 protected:
  virtual void setClip() {}
};

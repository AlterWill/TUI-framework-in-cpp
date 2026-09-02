#pragma once

#include <cstddef>
#include <vector>

#include "layout/SizeConstraints.hpp"
#include "input/event.hpp"
#include "utilities/insets.hpp"
#include "rendering/renderContext.hpp"

// A normal widget would have to just handle 
// min, max, fixed size
//
// A parent has to handle the constraints of 
// fixed,min,max,fill, child alignment

class Widget {
 protected:
  bool focusable{false};

 public:
  Widget* parent = nullptr;
  bool focused{false};

  Insets padding;

  // Focus
  bool isFocused() const { return focused; }
  void setFocused(bool val) { focused = val; }

  // Parent
  Widget* getParent() const { return parent; }
  void setParent(Widget* val) { parent = val; }

  // Padding
  Insets getPadding() const { return padding; }
  void setPadding(Insets p) { padding = p; }

  void onFocus() { focused = true; }
  void onUnFocus() { focused = false; }

  bool isFocusable() const { return focusable; }
  void disableFocusable() { focusable = false; }

  std::size_t childrenSize() { return 0; }

  virtual std::vector<Widget*> getChildren() { return {}; }

  virtual Size intrinsicSize() = 0;
  virtual Size measure(const SizeConstraints& constraints) = 0;
  virtual void render(RenderContext& rendercontext) = 0;
  virtual void layout() = 0;
  virtual bool handleEvent(const Event&) { return false; }

  virtual ~Widget() {}
};

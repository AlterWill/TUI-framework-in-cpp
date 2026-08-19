#pragma once

#include "renderContext.hpp"
#include "widget.hpp"
#include <memory>

class SingleChildWidget : public Widget {
public:
  std::unique_ptr<Widget> child;

  SingleChildWidget(std::unique_ptr<Widget> c) : child(std::move(c)) {
    if (child) {
      child->parent = this;
    }
  }

  std::size_t childrenSize(){
    return (child)? 1 : 0;
  }

  std::vector<Widget*> getChildren() override{
    if(child) return { child.get() };
    return {};
  }

  void setChild(std::unique_ptr<Widget> c) {
    child = std::move(c);
    child->parent = this;
  }

  void removeChild() { child = nullptr; }

  SizeConstraints buildConstraints(){
    return SizeConstraints{Size{0,0},Size{rect.getHeight(),rect.getWidth()}};
  }

  Size measure(const SizeConstraints& constraints) override{
    if(child){
      return child->measure(constraints);
    }
    return Size{0,0};
  }

  virtual Rect computeRect(Size& ) { return Rect{};}

  virtual void setRectForChild(){}

  void render(RenderContext& renderContext) override {
    if (!child)
      return;
    child->render(renderContext);
  }

void layout() override {
    if (!child) return;

    SizeConstraints constraints = buildConstraints();

    Size size = child->measure(constraints);

    Rect rect = computeRect(size);

    child->setRect(rect);

    child->layout();
}

};

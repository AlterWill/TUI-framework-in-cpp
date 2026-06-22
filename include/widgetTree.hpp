#pragma once

#include <iostream>

#include "container.hpp"
class WidgetTree {
 public:
  std::unique_ptr<Container> root;
  frameBuffer fb;

  WidgetTree(std::unique_ptr<Container> rootContainer) : root(std::move(rootContainer)) {
  }

  void dfs() {
    if (!root) throw std::runtime_error("Root is null");
    if (root->children.size() == 0) throw std::runtime_error("The root should have widget to render");

    dfsHelper(*root);
  }

  void display() {
    fb.display();
    std::cout << fb.displayOutput << std::endl;
  }

 private:
  void dfsHelper(Container& container) {
    for (auto& child : container.children) {
      child->parent = &container;
      if (auto* childContainer = dynamic_cast<Container*>(child.get())) {
        dfsHelper(*childContainer);
      } else {
        child->render(fb);
      }
    }
  };
};

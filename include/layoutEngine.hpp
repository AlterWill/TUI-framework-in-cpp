#pragma once
#include "node.hpp"
#include <cmath>
#include <stdexcept>

/*
enum class NodeType {
  Text,
  Box,
  Column,
  Row,
  Grid,
};

struct Node {
  NodeType type;
  std::unordered_map<std::string, std::variant<std::string, int, float, bool>>
      props;
  std::vector<Node> children;
};
*/

void layoutEngine(Node &root, int width, int height, int row = 1, int col = 1) {
  int n = root.children.size();
  if (n == 0)
    return;

  switch (root.type) {
  case NodeType::Row: {
    float dividedHeightLength = static_cast<float>(height) / n;
    for (int i = 0; i < n - 1; i++) {
      root.children[i].dimeansion.row =
          row + i * std::floor(dividedHeightLength);
      root.children[i].dimeansion.col = col;
      root.children[i].dimeansion.width = width;
      root.children[i].dimeansion.height = std::floor(height / n);
    }
    root.children[n - 1].dimeansion.row =
        row + (n - 1) * std::ceil(dividedHeightLength);
    root.children[n - 1].dimeansion.col = col;
    root.children[n - 1].dimeansion.width = width;
    root.children[n - 1].dimeansion.height = std::ceil(dividedHeightLength);
    break;
  }

  case NodeType::Column: {
    float dividedWidthLength = static_cast<float>(width) / n;
    for (int i = 0; i < n; i++) {
      root.children[i].dimeansion.row = row;
      root.children[i].dimeansion.col =
          col + i * std::floor(dividedWidthLength);
      root.children[i].dimeansion.width = std::floor(dividedWidthLength);
      root.children[i].dimeansion.height = height;
    }
    root.children[n - 1].dimeansion.row = row;
    root.children[n - 1].dimeansion.col =
        col + (n - 1) * std::ceil(dividedWidthLength);
    ;
    root.children[n - 1].dimeansion.width = std::ceil(dividedWidthLength);
    root.children[n - 1].dimeansion.height = width;
    break;
  }

  case NodeType::Grid: {
    int col = root.props["column"];
    int row = root.props["column"];
  }
  }
}

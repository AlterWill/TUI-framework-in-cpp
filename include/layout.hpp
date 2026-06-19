#pragma once

#include "node.hpp"
#include <node.hpp>
#include <vector>

Node ColumnLayout(std::vector<Node> children) {
  Node n;
  n.type = NodeType::Column;
  n.children = std::move(children);
  return n;
}

Node RowLayout(std::vector<Node> children) {
  Node n;
  n.type = NodeType::Row;
  n.children = std::move(children);
  return n;
}

Node GridLayout(int column, int rows, std::vector<Node> children) {
  Node n;
  n.type = NodeType::Grid;
  n.props["column"] = column;
  n.props["rows"] = rows;
  n.children = std::move(children);
  return n;
}

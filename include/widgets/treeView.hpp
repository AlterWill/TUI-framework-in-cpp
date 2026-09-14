#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "core/widget.hpp"
#include "input/event.hpp"
#include "layout/SizeConstraints.hpp"
#include "rendering/renderContext.hpp"
#include "styling/style.hpp"
#include "utilities/insets.hpp"
#include "utilities/Rect.hpp"

struct TreeNode {
  std::u32string label;
  std::vector<TreeNode> children;
  bool expanded{false};
  void* userData{nullptr};

  TreeNode() = default;
  explicit TreeNode(std::u32string l) : label(std::move(l)) {}
  TreeNode(std::u32string l, std::vector<TreeNode> c) : label(std::move(l)), children(std::move(c)) {}
};

struct TreeViewBase {
  std::vector<TreeNode> roots;
  std::vector<std::size_t> selectedPath;
  std::vector<std::size_t> focusedPath;
  Rect rect{};

  Style defaultStyle{};
  Style selectedStyle{};
  Style focusedStyle{};

  std::size_t indentSize{2};
  char32_t expandedGlyph{U'\u25BC'};
  char32_t collapsedGlyph{U'\u25B6'};
  char32_t leafGlyph{U' '};

  std::function<void(const std::vector<std::size_t>&)> onSelectionChange;
  std::function<void(const std::vector<std::size_t>&)> onExpansionChange;
};

struct TreeView : public Widget {
  TreeViewBase base;

  TreeView() {
    setFocusable();
    base.defaultStyle = Style{};
    base.selectedStyle = Style{};
    base.selectedStyle.setTextStyle(TextStyle::Reverse);
    base.focusedStyle = Style{};
    base.focusedStyle.setTextStyle(TextStyle::Bold);
  }

  explicit TreeView(std::vector<TreeNode> nodes) : TreeView() {
    base.roots = std::move(nodes);
    if (!base.roots.empty()) {
      base.focusedPath = {0};
    }
  }

  TreeView& withRoots(std::vector<TreeNode> nodes) {
    base.roots = std::move(nodes);
    base.selectedPath.clear();
    base.focusedPath.clear();
    if (!base.roots.empty()) {
      base.focusedPath = {0};
    }
    return *this;
  }

  TreeView& addRoot(TreeNode node) {
    base.roots.push_back(std::move(node));
    if (base.focusedPath.empty() && base.roots.size() == 1) {
      base.focusedPath = {0};
    }
    return *this;
  }

  TreeView& withIndentSize(std::size_t size) {
    base.indentSize = size;
    return *this;
  }

  TreeView& withGlyphs(char32_t expanded, char32_t collapsed, char32_t leaf) {
    base.expandedGlyph = expanded;
    base.collapsedGlyph = collapsed;
    base.leafGlyph = leaf;
    return *this;
  }

  TreeView& withDefaultStyle(Style s) {
    base.defaultStyle = std::move(s);
    return *this;
  }

  TreeView& withSelectedStyle(Style s) {
    base.selectedStyle = std::move(s);
    return *this;
  }

  TreeView& withFocusedStyle(Style s) {
    base.focusedStyle = std::move(s);
    return *this;
  }

  TreeView& withColours(ColourPair defaultCol, ColourPair selectedCol, ColourPair focusedCol) {
    base.defaultStyle.colours = defaultCol;
    base.selectedStyle.colours = selectedCol;
    base.focusedStyle.colours = focusedCol;
    return *this;
  }

  TreeView& onSelect(std::function<void(const std::vector<std::size_t>&)> cb) {
    base.onSelectionChange = std::move(cb);
    return *this;
  }

  TreeView& onExpand(std::function<void(const std::vector<std::size_t>&)> cb) {
    base.onExpansionChange = std::move(cb);
    return *this;
  }

  const std::vector<std::size_t>& getSelectedPath() const { return base.selectedPath; }
  const std::vector<std::size_t>& getFocusedPath() const { return base.focusedPath; }
  bool hasSelection() const { return !base.selectedPath.empty(); }
  bool hasFocus() const { return isFocused(); }

  void select(const std::vector<std::size_t>& path) {
    if (isValidPath(path)) {
      base.selectedPath = path;
      base.focusedPath = path;
      if (base.onSelectionChange) base.onSelectionChange(path);
    }
  }

  void selectFocused() {
    if (!base.focusedPath.empty()) {
      select(base.focusedPath);
    }
  }

  void clearSelection() {
    base.selectedPath.clear();
  }

  bool isExpanded(const std::vector<std::size_t>& path) const {
    const TreeNode* node = getNode(path);
    return node && node->expanded;
  }

  void expand(const std::vector<std::size_t>& path) {
    TreeNode* node = getNodeMutable(path);
    if (node && !node->children.empty() && !node->expanded) {
      node->expanded = true;
      if (base.onExpansionChange) base.onExpansionChange(path);
    }
  }

  void collapse(const std::vector<std::size_t>& path) {
    TreeNode* node = getNodeMutable(path);
    if (node && node->expanded) {
      node->expanded = false;
      if (base.onExpansionChange) base.onExpansionChange(path);
    }
  }

  void toggle(const std::vector<std::size_t>& path) {
    TreeNode* node = getNodeMutable(path);
    if (node && !node->children.empty()) {
      node->expanded = !node->expanded;
      if (base.onExpansionChange) base.onExpansionChange(path);
    }
  }

  void toggleFocused() {
    if (!base.focusedPath.empty()) {
      toggle(base.focusedPath);
    }
  }

  Size measure(const SizeConstraints& constraints) override {
    auto visible = collectVisibleNodes();
    std::size_t contentHeight = visible.size();
    std::size_t contentWidth = 0;

    for (const auto& vn : visible) {
      std::size_t rowWidth = vn.depth * base.indentSize + 1 + vn.node->label.size();
      contentWidth = std::max(contentWidth, rowWidth);
    }

    std::size_t totalPadH = padding.vertical();
    std::size_t totalPadW = padding.horizontal();

    std::size_t clampedWidth = std::clamp(
        contentWidth + totalPadW,
        constraints.getMinWidth(),
        constraints.getMaxWidth()
    );

    std::size_t heightNeeded = totalPadH + contentHeight;

    std::size_t clampedHeight = std::clamp(
        heightNeeded,
        constraints.getMinHeight(),
        constraints.getMaxHeight()
    );

    return Size{clampedHeight, clampedWidth};
  }

  void layout(const Rect& rect) override {
    base.rect = rect;
  }

  void render(RenderContext& rendercontext) override {
    const Rect& rect = base.rect;
    std::size_t totalPadH = padding.vertical();
    std::size_t totalPadW = padding.horizontal();

    if (rect.height <= totalPadH || rect.width <= totalPadW) {
      return;
    }

    std::size_t contentWidth = rect.width - totalPadW;
    std::size_t startX = rect.x + padding.left;
    std::size_t startY = rect.y + padding.top;
    std::size_t maxY = startY + rect.height - padding.bottom;

    auto visible = collectVisibleNodes();

    for (std::size_t i = 0; i < visible.size() && startY + i < maxY; ++i) {
      const auto& vn = visible[i];
      std::size_t y = startY + i;

      bool isSelected = (vn.path == base.selectedPath);
      bool isFocused = (vn.path == base.focusedPath) && this->isFocused();

      Style rowStyle = isSelected ? base.selectedStyle : base.defaultStyle;
      if (isFocused && !isSelected) {
        rowStyle = base.focusedStyle;
      }

      Cell fill{U' ', rowStyle};
      for (std::size_t x = startX; x < startX + contentWidth; ++x) {
        rendercontext.setCell(x, y, fill);
      }

      std::size_t x = startX;

      for (std::size_t d = 0; d < vn.depth; ++d) {
        for (std::size_t s = 0; s < base.indentSize; ++s) {
          if (x < startX + contentWidth) {
            rendercontext.setCell(x, y, Cell{U' ', rowStyle});
            ++x;
          }
        }
      }

      char32_t indicator = vn.node->children.empty() ? base.leafGlyph
                            : (vn.node->expanded ? base.expandedGlyph : base.collapsedGlyph);
      if (x < startX + contentWidth) {
        rendercontext.setCell(x, y, Cell{indicator, rowStyle});
        ++x;
      }

      for (char32_t ch : vn.node->label) {
        if (x >= startX + contentWidth) break;
        rendercontext.setCell(x, y, Cell{ch, rowStyle});
        ++x;
      }
    }
  }

  bool handleEvent(const Event& event) override {
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      return handleMouse(*mouse);
    } else if (auto key = std::get_if<keyEvent>(&event)) {
      return handleKey(*key);
    }
    return false;
  }

  void onFocus() override {
    if (base.focusedPath.empty() && !base.roots.empty()) {
      base.focusedPath = {0};
    }
  }

private:
  struct VisibleNode {
    const TreeNode* node;
    std::vector<std::size_t> path;
    std::size_t depth;
  };

  std::vector<VisibleNode> collectVisibleNodes() const {
    std::vector<VisibleNode> result;
    for (std::size_t i = 0; i < base.roots.size(); ++i) {
      collectVisibleRecursive(base.roots[i], {i}, 0, result);
    }
    return result;
  }

  void collectVisibleRecursive(const TreeNode& node, std::vector<std::size_t> path,
                               std::size_t depth, std::vector<VisibleNode>& result) const {
    result.push_back({&node, path, depth});
    if (node.expanded) {
      for (std::size_t i = 0; i < node.children.size(); ++i) {
        auto childPath = path;
        childPath.push_back(i);
        collectVisibleRecursive(node.children[i], childPath, depth + 1, result);
      }
    }
  }

  const TreeNode* getNode(const std::vector<std::size_t>& path) const {
    if (path.empty()) return nullptr;
    const TreeNode* current = &base.roots[path[0]];
    for (std::size_t i = 1; i < path.size(); ++i) {
      if (path[i] >= current->children.size()) return nullptr;
      current = &current->children[path[i]];
    }
    return current;
  }

  TreeNode* getNodeMutable(const std::vector<std::size_t>& path) {
    if (path.empty()) return nullptr;
    TreeNode* current = &base.roots[path[0]];
    for (std::size_t i = 1; i < path.size(); ++i) {
      if (path[i] >= current->children.size()) return nullptr;
      current = &current->children[path[i]];
    }
    return current;
  }

  bool isValidPath(const std::vector<std::size_t>& path) const {
    return getNode(path) != nullptr;
  }

  bool handleMouse(const MouseEvent& mouse) {
    if (mouse.action != MouseAction::Press || mouse.button != MouseButton::Left) {
      return false;
    }

    const Rect& rect = base.rect;
    std::size_t totalPadH = padding.vertical();
    std::size_t totalPadW = padding.horizontal();

    if (rect.height <= totalPadH || rect.width <= totalPadW) {
      return false;
    }

    std::size_t startY = rect.y + padding.top;
    std::size_t startX = rect.x + padding.left;
    std::size_t contentHeight = rect.height - totalPadH;

    if (mouse.y < startY || mouse.y >= startY + contentHeight) {
      return false;
    }

    std::size_t rowIndex = mouse.y - startY;
    auto visible = collectVisibleNodes();
    if (rowIndex >= visible.size()) {
      return false;
    }

    const auto& vn = visible[rowIndex];
    std::size_t indentWidth = vn.depth * base.indentSize;
    std::size_t indicatorX = startX + indentWidth;

    if (mouse.x == indicatorX && !vn.node->children.empty()) {
      toggle(vn.path);
      return true;
    }

    if (mouse.x >= startX && mouse.x < rect.x + rect.width - padding.right) {
      base.focusedPath = vn.path;
      select(vn.path);
      return true;
    }

    return false;
  }

  bool handleKey(const keyEvent& key) {
    if (!isFocused()) return false;

    auto visible = collectVisibleNodes();
    if (visible.empty()) return false;

    std::size_t focusedIndex = findFocusedIndex(visible);
    if (focusedIndex == visible.size()) {
      focusedIndex = 0;
      base.focusedPath = visible[0].path;
    }

    switch (key.key) {
      case ARROW_UP: {
        if (focusedIndex > 0) {
          base.focusedPath = visible[focusedIndex - 1].path;
        }
        return true;
      }
      case ARROW_DOWN: {
        if (focusedIndex + 1 < visible.size()) {
          base.focusedPath = visible[focusedIndex + 1].path;
        }
        return true;
      }
      case ARROW_RIGHT: {
        const auto& focusedNode = visible[focusedIndex];
        if (!focusedNode.node->children.empty()) {
          if (!focusedNode.node->expanded) {
            expand(focusedNode.path);
          } else if (focusedIndex + 1 < visible.size()) {
            base.focusedPath = visible[focusedIndex + 1].path;
          }
        }
        return true;
      }
      case ARROW_LEFT: {
        const auto& focusedNode = visible[focusedIndex];
        if (focusedNode.node->expanded) {
          collapse(focusedNode.path);
        } else if (focusedNode.depth > 0) {
          auto parentPath = focusedNode.path;
          parentPath.pop_back();
          base.focusedPath = parentPath;
        }
        return true;
      }
      case HOME_KEY: {
        base.focusedPath = visible[0].path;
        return true;
      }
      case END_KEY: {
        base.focusedPath = visible.back().path;
        return true;
      }
      case PAGE_UP: {
        std::size_t pageSize = visible.size() > 10 ? 10 : (visible.size() / 2);
        if (focusedIndex >= pageSize) {
          base.focusedPath = visible[focusedIndex - pageSize].path;
        } else {
          base.focusedPath = visible[0].path;
        }
        return true;
      }
      case PAGE_DOWN: {
        std::size_t pageSize = visible.size() > 10 ? 10 : (visible.size() / 2);
        if (focusedIndex + pageSize < visible.size()) {
          base.focusedPath = visible[focusedIndex + pageSize].path;
        } else {
          base.focusedPath = visible.back().path;
        }
        return true;
      }
      case '\n':
      case ' ': {
        toggleFocused();
        return true;
      }
      default:
        return false;
    }
  }

  std::size_t findFocusedIndex(const std::vector<VisibleNode>& visible) const {
    for (std::size_t i = 0; i < visible.size(); ++i) {
      if (visible[i].path == base.focusedPath) {
        return i;
      }
    }
    return visible.size();
  }
};
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <vector>

#include "core/multiChildWidget.hpp"
#include "layout/RowContainer.hpp"

struct GridBase {
  Rect rect{};
};

template <std::size_t Rows, std::size_t Cols>
struct Grid : public MultiChildWidget {
  static_assert(Rows > 0 && Cols > 0, "Grid must have at least 1 row and 1 column");

  GridBase gridBase;
  std::size_t rowGap{};
  std::size_t colGap{};
  std::array<Row, Rows> rows{};

  Grid() {
    for (auto& row : rows) {
      row.base.children.resize(Cols);
    }
  }

  // Builder methods
  Grid& withRowGap(std::size_t rg) {
    rowGap = rg;
    return *this;
  }

  Grid& withColGap(std::size_t cg) {
    colGap = cg;
    for (auto& row : rows) {
      row.withGap(cg);
    }
    return *this;
  }

  Grid& withGaps(std::size_t rg, std::size_t cg) {
    rowGap = rg;
    colGap = cg;
    for (auto& row : rows) {
      row.withGap(cg);
    }
    return *this;
  }

  Grid& withPadding(Insets p) {
    padding = p;
    return *this;
  }

  Grid& setCell(std::size_t r, std::size_t c, LayoutNode node) {
    if (r < Rows && c < Cols) {
      rows[r].base.children[c] = std::move(node);
    }
    return *this;
  }

  Grid& setCell(std::size_t r, std::size_t c, std::unique_ptr<Widget> w,
                HorizontalAlignment hAlign = HorizontalAlignment::Left,
                VerticalAlignment vAlign = VerticalAlignment::Top) {
    if (r < Rows && c < Cols) {
      auto& child = rows[r].base.children[c];
      child.widget = std::move(w);
      child.horizontalAlignment = hAlign;
      child.verticalAlignment = vAlign;
      child.dirty = true;
    }
    return *this;
  }

  Grid& setCellSize(std::size_t r, std::size_t c, SizeSpec wSpec, SizeSpec hSpec) {
    if (r < Rows && c < Cols) {
      rows[r].base.children[c].width = wSpec;
      rows[r].base.children[c].height = hSpec;
    }
    return *this;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::vector<LayoutNode> rowNodes(Rows);
    for (std::size_t r = 0; r < Rows; ++r) {
      rowNodes[r].height = getRowHeightSpec(r);
      rowNodes[r].width = SizeSpec{SizeType::Flex, 1, {}};
      rowNodes[r].widget = std::unique_ptr<Widget>(&rows[r]);
    }

    Size measuredSize = LinearLayoutSolver::solveMeasure(
        rowNodes, Axis::Vertical, constraints, padding, rowGap);

    for (std::size_t r = 0; r < Rows; ++r) {
      rowNodes[r].widget.release();
    }

    return measuredSize;
  }

  void setRectForChildren(const Rect& rect) override {
    gridBase.rect = rect;

    std::vector<LayoutNode> rowNodes(Rows);
    for (std::size_t r = 0; r < Rows; ++r) {
      rowNodes[r].height = getRowHeightSpec(r);
      rowNodes[r].width = SizeSpec{SizeType::Flex, 1, {}};
      rowNodes[r].widget = std::unique_ptr<Widget>(&rows[r]);
    }

    SizeConstraints sc{Size{0, 0}, Size{rect.height, rect.width}};
    LinearLayoutSolver::solveMeasure(rowNodes, Axis::Vertical, sc, padding, rowGap);

    for (std::size_t r = 0; r < Rows; ++r) {
      rowNodes[r].widget.release();
    }

    std::size_t startX = rect.x + padding.left;
    std::size_t currentY = rect.y + padding.top;
    std::size_t usableWidth =
        rect.width > padding.horizontal() ? rect.width - padding.horizontal() : 0;

    for (std::size_t r = 0; r < Rows; ++r) {
      std::size_t rowH = rowNodes[r].measured.height;
      Rect rowRect{startX, currentY, rowH, usableWidth};
      rows[r].layout(rowRect);
      currentY += rowH + rowGap;
    }
  }

  void render(RenderContext& rendercontext) override {
    for (auto& row : rows) {
      row.render(rendercontext);
    }
  }

  bool handleEvent(const Event& event) override {
    for (auto& row : rows) {
      if (row.handleEvent(event)) {
        return true;
      }
    }
    return false;
  }

  ConnectionInfo getConnections() const override {
    ConnectionInfo info;
    const Rect& rect = gridBase.rect;
    std::size_t startX = rect.x + padding.left;
    std::size_t currentY = rect.y + padding.top;

    for (std::size_t r = 0; r < Rows; ++r) {
      if (!rows[r].base.children.empty()) {
        std::size_t rowH = rows[r].base.children[0].measured.height;
        std::size_t currentX = startX;

        for (std::size_t c = 0; c < Cols; ++c) {
          const auto& child = rows[r].base.children[c];
          std::size_t childW = child.measured.width;
          std::size_t childH = child.measured.height;
          std::size_t childY = child.rect.y;
          currentX += childW + child.margin.horizontal();

          if (c + 1 < Cols && colGap > 0) {
            Rect sepRect{currentX, childY, childH, colGap};
            info.addSeparator(sepRect, false, c, c + 1);
            currentX += colGap;
          }
        }

        if (r + 1 < Rows && rowGap > 0) {
          std::size_t sepY = currentY + rowH;
          std::size_t rowW = 0;
          for (const auto& ch : rows[r].base.children) {
            rowW += ch.measured.width + ch.margin.horizontal();
          }
          rowW += (Cols > 1) ? (Cols - 1) * colGap : 0;
          Rect sepRect{startX, sepY, rowGap, rowW};
          info.addSeparator(sepRect, true, r, r + 1);
        }

        currentY += rowH + rowGap;
      }
    }

    for (const auto& row : rows) {
      info.addChildConnections(row.getConnections());
    }

    return info;
  }

 private:
  SizeSpec getRowHeightSpec(std::size_t r) const {
    for (std::size_t c = 0; c < Cols; ++c) {
      const auto& spec = rows[r].base.children[c].height;
      if (spec.type != SizeType::Content) {
        return spec;
      }
    }
    return SizeSpec{SizeType::Content, 0, {}};
  }
};

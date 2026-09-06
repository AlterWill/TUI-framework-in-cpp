#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <vector>

#include "core/multiChildWidget.hpp"

struct GridBase {
  Rect rect{};
};

template <std::size_t Rows, std::size_t Cols>
struct Grid : public MultiChildWidget {
  static_assert(Rows > 0 && Cols > 0, "Grid must have at least 1 row and 1 column");

  GridBase gridBase;
  std::size_t rowGap{};
  std::size_t colGap{};
  std::array<LayoutNode, Rows * Cols> cells{};

  Grid() {
    base.children.resize(Rows * Cols);
  }

  // Builder methods
  Grid& withRowGap(std::size_t rg) {
    rowGap = rg;
    return *this;
  }

  Grid& withColGap(std::size_t cg) {
    colGap = cg;
    return *this;
  }

  Grid& withGaps(std::size_t rg, std::size_t cg) {
    rowGap = rg;
    colGap = cg;
    return *this;
  }

  Grid& withPadding(Insets p) {
    base.widgetBase.padding = p;
    return *this;
  }

  Grid& setCell(std::size_t r, std::size_t c, LayoutNode node) {
    if (r < Rows && c < Cols) {
      std::size_t idx = r * Cols + c;
      cells[idx] = std::move(node);
      syncChildren();
    }
    return *this;
  }

  Grid& setCell(std::size_t r, std::size_t c, std::unique_ptr<Widget> w,
                HorizontalAlignment hAlign = HorizontalAlignment::Left,
                VerticalAlignment vAlign = VerticalAlignment::Top) {
    if (r < Rows && c < Cols) {
      std::size_t idx = r * Cols + c;
      cells[idx].widget = std::move(w);
      cells[idx].horizontalAlignment = hAlign;
      cells[idx].verticalAlignment = vAlign;
      syncChildren();
    }
    return *this;
  }

  void syncChildren() {
    for (std::size_t i = 0; i < Rows * Cols; ++i) {
      base.children[i] = std::move(cells[i]);
    }
  }

  bool handleEvent(const Event&) override { return false; }

  Size measure(const SizeConstraints& constraints) override {
    const auto& padding = base.widgetBase.padding;
    std::size_t padW = padding.left + padding.right;
    std::size_t padH = padding.top + padding.bottom;
    std::size_t totalColGaps = (Cols > 1) ? (Cols - 1) * colGap : 0;
    std::size_t totalRowGaps = (Rows > 1) ? (Rows - 1) * rowGap : 0;

    std::size_t usableW =
        constraints.getMaxWidth() > (padW + totalColGaps) ? constraints.getMaxWidth() - padW - totalColGaps : 0;
    std::size_t usableH =
        constraints.getMaxHeight() > (padH + totalRowGaps) ? constraints.getMaxHeight() - padH - totalRowGaps : 0;

    std::size_t cellMaxW = usableW / Cols;
    std::size_t cellMaxH = usableH / Rows;

    std::array<std::size_t, Cols> colWidths{};
    std::array<std::size_t, Rows> rowHeights{};

    for (std::size_t r = 0; r < Rows; ++r) {
      for (std::size_t c = 0; c < Cols; ++c) {
        std::size_t idx = r * Cols + c;
        auto& child = base.children[idx];
        if (!child.widget) continue;

        std::size_t margW = child.margin.left + child.margin.right;
        std::size_t margH = child.margin.top + child.margin.bottom;
        std::size_t cMaxW = cellMaxW > margW ? cellMaxW - margW : 0;
        std::size_t cMaxH = cellMaxH > margH ? cellMaxH - margH : 0;

        if (child.dirty || child.measured.width == 0 || child.measured.height == 0) {
          SizeConstraints sc{Size{0, 0}, Size{cMaxH, cMaxW}};
          child.constraints = sc;
          child.measured = child.widget->measure(sc);
          child.dirty = false;
        }

        colWidths[c] = std::max(colWidths[c], child.measured.width + margW);
        rowHeights[r] = std::max(rowHeights[r], child.measured.height + margH);
      }
    }

    std::size_t totalColsW = 0;
    for (auto w : colWidths) totalColsW += w;
    totalColsW += padW + totalColGaps;

    std::size_t totalRowsH = 0;
    for (auto h : rowHeights) totalRowsH += h;
    totalRowsH += padH + totalRowGaps;

    std::size_t finalW = std::clamp(totalColsW, constraints.getMinWidth(), constraints.getMaxWidth());
    std::size_t finalH = std::clamp(totalRowsH, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{finalH, finalW};
  }

  void setRectForChildren() override {
    const auto& padding = base.widgetBase.padding;
    std::size_t padW = padding.left + padding.right;
    std::size_t padH = padding.top + padding.bottom;
    std::size_t totalColGaps = (Cols > 1) ? (Cols - 1) * colGap : 0;
    std::size_t totalRowGaps = (Rows > 1) ? (Rows - 1) * rowGap : 0;

    std::size_t usableW =
        gridBase.rect.width > (padW + totalColGaps) ? gridBase.rect.width - padW - totalColGaps : 0;
    std::size_t usableH =
        gridBase.rect.height > (padH + totalRowGaps) ? gridBase.rect.height - padH - totalRowGaps : 0;

    std::size_t cellW = usableW / Cols;
    std::size_t cellH = usableH / Rows;

    std::size_t startX = gridBase.rect.x + padding.left;
    std::size_t startY = gridBase.rect.y + padding.top;

    for (std::size_t r = 0; r < Rows; ++r) {
      for (std::size_t c = 0; c < Cols; ++c) {
        std::size_t idx = r * Cols + c;
        auto& child = base.children[idx];

        std::size_t slotX = startX + c * (cellW + colGap);
        std::size_t slotY = startY + r * (cellH + rowGap);

        std::size_t childW = child.measured.width;
        std::size_t childH = child.measured.height;

        std::size_t childX = slotX + child.margin.left;
        std::size_t totalChildW = childW + child.margin.left + child.margin.right;
        if (cellW > totalChildW) {
          std::size_t extraW = cellW - totalChildW;
          if (child.horizontalAlignment == HorizontalAlignment::Center) {
            childX += extraW / 2;
          } else if (child.horizontalAlignment == HorizontalAlignment::Right) {
            childX += extraW;
          }
        }

        std::size_t childY = slotY + child.margin.top;
        std::size_t totalChildH = childH + child.margin.top + child.margin.bottom;
        if (cellH > totalChildH) {
          std::size_t extraH = cellH - totalChildH;
          if (child.verticalAlignment == VerticalAlignment::Center) {
            childY += extraH / 2;
          } else if (child.verticalAlignment == VerticalAlignment::Bottom) {
            childY += extraH;
          }
        }

        child.rect = Rect{childX, childY, childH, childW};
      }
    }
  }
};

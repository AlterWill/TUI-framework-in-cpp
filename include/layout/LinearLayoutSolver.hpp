#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "core/layoutNode.hpp"
#include "layout/SizeConstraints.hpp"

enum class Axis {
  Horizontal,  // Row: Main axis = Width, Cross axis = Height
  Vertical     // Column: Main axis = Height, Cross axis = Width
};

struct LinearLayoutSolver {
  static inline std::size_t getMainSpec(const LayoutNode& node, Axis axis) {
    return (axis == Axis::Horizontal) ? node.width.value : node.height.value;
  }

  static inline SizeType getMainType(const LayoutNode& node, Axis axis) {
    return (axis == Axis::Horizontal) ? node.width.type : node.height.type;
  }

  static inline std::size_t getMainMargin(const LayoutNode& node, Axis axis) {
    return (axis == Axis::Horizontal) ? (node.margin.left + node.margin.right)
                                      : (node.margin.top + node.margin.bottom);
  }

  static inline std::size_t getCrossMargin(const LayoutNode& node, Axis axis) {
    return (axis == Axis::Horizontal) ? (node.margin.top + node.margin.bottom)
                                      : (node.margin.left + node.margin.right);
  }

  static inline std::size_t getMainMeasured(const LayoutNode& node, Axis axis) {
    return (axis == Axis::Horizontal) ? node.measured.width : node.measured.height;
  }

  static inline void setMainMeasured(LayoutNode& node, Axis axis, std::size_t val) {
    if (axis == Axis::Horizontal) {
      node.measured.width = val;
    } else {
      node.measured.height = val;
    }
  }

  static inline void setCrossMeasured(LayoutNode& node, Axis axis, std::size_t val) {
    if (axis == Axis::Horizontal) {
      node.measured.height = val;
    } else {
      node.measured.width = val;
    }
  }

  // 1. Resolve Fixed children
  static void resolveFixed(std::vector<LayoutNode>& children, Axis axis, std::size_t& remainingSpace) {
    for (auto& child : children) {
      if (getMainType(child, axis) == SizeType::Fixed) {
        std::size_t fixedVal = getMainSpec(child, axis);
        std::size_t totalChildMain = fixedVal + getMainMargin(child, axis);
        setMainMeasured(child, axis, fixedVal);
        if (remainingSpace >= totalChildMain) {
          remainingSpace -= totalChildMain;
        } else {
          remainingSpace = 0;
        }
      }
    }
  }

  // 2. Resolve Percentage children
  static void resolvePercentage(std::vector<LayoutNode>& children, Axis axis, std::size_t availableSpace,
                                std::size_t& remainingSpace) {
    for (auto& child : children) {
      if (getMainType(child, axis) == SizeType::Percentage) {
        double pct = std::clamp(static_cast<double>(getMainSpec(child, axis)), 0.0, 100.0);
        std::size_t pctVal = static_cast<std::size_t>((static_cast<double>(availableSpace) * pct) / 100.0);
        std::size_t totalChildMain = pctVal + getMainMargin(child, axis);
        setMainMeasured(child, axis, pctVal);
        if (remainingSpace >= totalChildMain) {
          remainingSpace -= totalChildMain;
        } else {
          remainingSpace = 0;
        }
      }
    }
  }

  // 3. Resolve Content children (single measure pass with Clay proportional shrink)
  static void resolveContent(std::vector<LayoutNode>& children, Axis axis, std::size_t maxCrossSize,
                             std::size_t& remainingSpace) {
    std::vector<std::size_t> contentIndices;
    std::size_t totalContentPreferred = 0;

    for (std::size_t i = 0; i < children.size(); ++i) {
      auto& child = children[i];
      if (getMainType(child, axis) == SizeType::Content) {
        contentIndices.push_back(i);
        if (child.widget) {
          // If not dirty and previously measured with compatible constraints, we can reuse
          if (child.dirty || child.measured.width == 0 || child.measured.height == 0) {
            std::size_t childCrossMax =
                maxCrossSize > getCrossMargin(child, axis) ? maxCrossSize - getCrossMargin(child, axis) : 0;

            SizeConstraints sc = (axis == Axis::Horizontal)
                                     ? SizeConstraints{Size{0, 0}, Size{childCrossMax, remainingSpace}}
                                     : SizeConstraints{Size{0, 0}, Size{remainingSpace, childCrossMax}};

            child.constraints = sc;
            child.measured = child.widget->measure(sc);
            child.dirty = false;
          }
          std::size_t pref = getMainMeasured(child, axis);
          totalContentPreferred += pref;
        }
      }
    }

    if (!contentIndices.empty()) {
      std::size_t contentMargins = 0;
      for (std::size_t idx : contentIndices) {
        contentMargins += getMainMargin(children[idx], axis);
      }

      std::size_t spaceForContent = remainingSpace > contentMargins ? remainingSpace - contentMargins : 0;

      if (totalContentPreferred > spaceForContent && totalContentPreferred > 0) {
        // Clay proportional shrink
        std::size_t deficit = totalContentPreferred - spaceForContent;
        for (std::size_t idx : contentIndices) {
          auto& child = children[idx];
          double share = static_cast<double>(getMainMeasured(child, axis)) / static_cast<double>(totalContentPreferred);
          std::size_t reduction = static_cast<std::size_t>(static_cast<double>(deficit) * share);
          std::size_t currentMain = getMainMeasured(child, axis);
          if (currentMain > reduction) {
            setMainMeasured(child, axis, currentMain - reduction);
          } else {
            setMainMeasured(child, axis, 0);
          }
        }
        remainingSpace = 0;
      } else {
        std::size_t consumed = totalContentPreferred + contentMargins;
        remainingSpace = remainingSpace > consumed ? remainingSpace - consumed : 0;
      }
    }
  }

  // 4. Resolve Flex children
  static void resolveFlex(std::vector<LayoutNode>& children, Axis axis, std::size_t& remainingSpace) {
    std::size_t totalFlex = 0;
    std::vector<std::size_t> flexIndices;

    for (std::size_t i = 0; i < children.size(); ++i) {
      auto& child = children[i];
      if (getMainType(child, axis) == SizeType::Flex) {
        flexIndices.push_back(i);
        std::size_t factor = getMainSpec(child, axis);
        totalFlex += (factor > 0 ? factor : 1);
      }
    }

    if (totalFlex > 0 && remainingSpace > 0) {
      std::size_t flexMargins = 0;
      for (std::size_t idx : flexIndices) {
        flexMargins += getMainMargin(children[idx], axis);
      }

      std::size_t flexPool = remainingSpace > flexMargins ? remainingSpace - flexMargins : 0;
      std::size_t allocatedFlex = 0;

      for (std::size_t idx : flexIndices) {
        auto& child = children[idx];
        std::size_t factor = getMainSpec(child, axis);
        if (factor == 0) factor = 1;
        double ratio = static_cast<double>(factor) / static_cast<double>(totalFlex);
        std::size_t mainVal = static_cast<std::size_t>(static_cast<double>(flexPool) * ratio);
        setMainMeasured(child, axis, mainVal);
        allocatedFlex += mainVal;
      }
      if (flexPool > allocatedFlex && !flexIndices.empty()) {
        std::size_t lastIdx = flexIndices.back();
        setMainMeasured(children[lastIdx], axis, getMainMeasured(children[lastIdx], axis) + (flexPool - allocatedFlex));
      }
    } else {
      for (std::size_t idx : flexIndices) {
        setMainMeasured(children[idx], axis, 0);
      }
    }
  }

  // 5. Finalize Cross Axis (Only measures non-content or dirty widgets)
  static std::size_t resolveCrossAxis(std::vector<LayoutNode>& children, Axis axis, std::size_t maxCrossSize,
                                      std::size_t padCross) {
    std::size_t maxCrossChild = 0;

    for (auto& child : children) {
      if (!child.widget) continue;

      std::size_t childMain = getMainMeasured(child, axis);
      std::size_t crossMarg = getCrossMargin(child, axis);
      std::size_t usableCrossMax = maxCrossSize > (padCross + crossMarg) ? maxCrossSize - padCross - crossMarg : 0;

      // Only invoke measure if it was not already measured with this main dimension
      if (getMainType(child, axis) != SizeType::Content || child.dirty) {
        SizeConstraints sc = (axis == Axis::Horizontal)
                                 ? SizeConstraints{Size{0, childMain}, Size{usableCrossMax, childMain}}
                                 : SizeConstraints{Size{childMain, 0}, Size{childMain, usableCrossMax}};
        child.constraints = sc;
        child.measured = child.widget->measure(sc);
        child.dirty = false;
      }

      std::size_t childCross = (axis == Axis::Horizontal) ? child.measured.height : child.measured.width;
      maxCrossChild = std::max(maxCrossChild, childCross + crossMarg);
    }

    return maxCrossChild;
  }

  // Complete linear layout measurement
  static Size solveMeasure(std::vector<LayoutNode>& children, Axis axis, const SizeConstraints& constraints,
                           const Insets& padding, std::size_t gap) {
    if (children.empty()) {
      return Size{0, 0};
    }

    std::size_t padMain = (axis == Axis::Horizontal) ? (padding.left + padding.right) : (padding.top + padding.bottom);
    std::size_t padCross = (axis == Axis::Horizontal) ? (padding.top + padding.bottom) : (padding.left + padding.right);
    std::size_t totalGaps = (children.size() > 1) ? (children.size() - 1) * gap : 0;

    std::size_t maxAvailableMain = (axis == Axis::Horizontal) ? constraints.getMaxWidth() : constraints.getMaxHeight();
    std::size_t maxAvailableCross = (axis == Axis::Horizontal) ? constraints.getMaxHeight() : constraints.getMaxWidth();

    std::size_t nonChildOverhead = padMain + totalGaps;
    std::size_t availableMain = maxAvailableMain > nonChildOverhead ? maxAvailableMain - nonChildOverhead : 0;
    std::size_t remainingSpace = availableMain;

    // Sizing Steps
    resolveFixed(children, axis, remainingSpace);
    resolvePercentage(children, axis, availableMain, remainingSpace);
    resolveContent(children, axis, maxAvailableCross, remainingSpace);
    resolveFlex(children, axis, remainingSpace);

    // Cross-Axis Resolution (avoids redundant calls)
    std::size_t maxCross = resolveCrossAxis(children, axis, maxAvailableCross, padCross);

    std::size_t totalAllocatedMain = 0;
    for (const auto& child : children) {
      totalAllocatedMain += getMainMeasured(child, axis) + getMainMargin(child, axis);
    }
    totalAllocatedMain += padMain + totalGaps;

    std::size_t finalMain = std::clamp(
        totalAllocatedMain,
        (axis == Axis::Horizontal) ? constraints.getMinWidth() : constraints.getMinHeight(),
        (axis == Axis::Horizontal) ? constraints.getMaxWidth() : constraints.getMaxHeight());

    std::size_t finalCross = std::clamp(
        maxCross + padCross,
        (axis == Axis::Horizontal) ? constraints.getMinHeight() : constraints.getMinWidth(),
        (axis == Axis::Horizontal) ? constraints.getMaxHeight() : constraints.getMaxWidth());

    return (axis == Axis::Horizontal) ? Size{finalCross, finalMain} : Size{finalMain, finalCross};
  }
};

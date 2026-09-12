#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>

#include "core/layoutNode.hpp"
#include "core/widget.hpp"
#include "rendering/renderContext.hpp"
#include "utilities/Orientation.hpp"
#include "utilities/sliderDrag.hpp"
#include "widgets/slider.hpp"

struct SplitPane : public Widget {
  LayoutNode first;
  LayoutNode second;

  Orientation orientation{Orientation::Horizontal};
  slider dividerSlider;
  bool dragging{false};
  Rect sliderRect{};

  SplitPane() = default;

  SplitPane(std::unique_ptr<Widget> firstChild, std::unique_ptr<Widget> secondChild,
            Orientation orient = Orientation::Horizontal) {
    orientation = orient;
    first.widget = std::move(firstChild);
    second.widget = std::move(secondChild);
  }

  SplitPane& withOrientation(Orientation orient) {
    orientation = orient;
    return *this;
  }

  SplitPane& withSplitRatio(double ratio) {
    dividerSlider.withValue(ratio);
    return *this;
  }

  SplitPane& withMinRatio(double r) {
    dividerSlider.withRange(r, dividerSlider.maxValue);
    return *this;
  }

  SplitPane& withMaxRatio(double r) {
    dividerSlider.withRange(dividerSlider.minValue, r);
    return *this;
  }

  SplitPane& withDividerStyle(sliderStyleData s) {
    dividerSlider.withStyle(s);
    return *this;
  }

  SplitPane& withDividerColours(ColourPair thumb, ColourPair track) {
    dividerSlider.withThumbColours(thumb);
    dividerSlider.withTrackColours(track);
    return *this;
  }

  SplitPane& withFirstChild(std::unique_ptr<Widget> w) {
    first.widget = std::move(w);
    return *this;
  }

  SplitPane& withSecondChild(std::unique_ptr<Widget> w) {
    second.widget = std::move(w);
    return *this;
  }

  SplitPane& withPadding(Insets p) {
    padding = p;
    return *this;
  }

  Size measure(const SizeConstraints& constraints) override {
    std::size_t padW = padding.horizontal();
    std::size_t padH = padding.vertical();

    std::size_t maxW = constraints.getMaxWidth() > padW ? constraints.getMaxWidth() - padW : 0;
    std::size_t maxH = constraints.getMaxHeight() > padH ? constraints.getMaxHeight() - padH : 0;

    bool horizontal = (orientation == Orientation::Horizontal);

    std::size_t childMaxW = horizontal ? (maxW > 1 ? maxW - 1 : 0) : maxW;
    std::size_t childMaxH = horizontal ? maxH : (maxH > 1 ? maxH - 1 : 0);

    SizeConstraints childConstraints{Size{0, 0}, Size{childMaxH, childMaxW}};

    if (first.widget) {
      first.constraints = childConstraints;
      first.measured = first.widget->measure(childConstraints);
    } else {
      first.measured = Size{0, 0};
    }

    if (second.widget) {
      second.constraints = childConstraints;
      second.measured = second.widget->measure(childConstraints);
    } else {
      second.measured = Size{0, 0};
    }

    std::size_t totalW;
    std::size_t totalH;

    if (horizontal) {
      totalW = first.measured.width + second.measured.width + 1;
      totalH = std::max(first.measured.height, second.measured.height);
    } else {
      totalH = first.measured.height + second.measured.height + 1;
      totalW = std::max(first.measured.width, second.measured.width);
    }

    totalW = std::clamp(totalW + padW, constraints.getMinWidth(), constraints.getMaxWidth());
    totalH = std::clamp(totalH + padH, constraints.getMinHeight(), constraints.getMaxHeight());

    return Size{totalH, totalW};
  }

  void layout(const Rect& rect) override {
    bool horizontal = (orientation == Orientation::Horizontal);

    std::size_t startX = rect.x + padding.left;
    std::size_t startY = rect.y + padding.top;
    std::size_t usableW =
        rect.width > padding.horizontal() ? rect.width - padding.horizontal() : 0;
    std::size_t usableH =
        rect.height > padding.vertical() ? rect.height - padding.vertical() : 0;

    if (usableW == 0 || usableH == 0) return;

    double ratio = std::clamp(dividerSlider.getValue(), dividerSlider.minValue, dividerSlider.maxValue);

    if (horizontal) {
      std::size_t splitSpace = usableW > 1 ? usableW - 1 : 0;
      std::size_t firstW = static_cast<std::size_t>(static_cast<double>(splitSpace) * ratio);
      std::size_t secondW = splitSpace > firstW ? splitSpace - firstW : 0;

      first.rect = Rect{startX, startY, usableH, firstW};
      second.rect = Rect{startX + firstW + 1, startY, usableH, secondW};
      sliderRect = Rect{startX + firstW, startY, usableH, 1};
    } else {
      std::size_t splitSpace = usableH > 1 ? usableH - 1 : 0;
      std::size_t firstH = static_cast<std::size_t>(static_cast<double>(splitSpace) * ratio);
      std::size_t secondH = splitSpace > firstH ? splitSpace - firstH : 0;

      first.rect = Rect{startX, startY, firstH, usableW};
      second.rect = Rect{startX, startY + firstH + 1, secondH, usableW};
      sliderRect = Rect{startX, startY + firstH, 1, usableW};
    }

    if (first.widget) {
      first.widget->layout(first.rect);
    }
    if (second.widget) {
      second.widget->layout(second.rect);
    }
  }

  void render(RenderContext& rendercontext) override {
    if (first.widget) {
      rendercontext.setRect(first.rect);
      first.widget->render(rendercontext);
    }

    if (second.widget) {
      rendercontext.setRect(second.rect);
      second.widget->render(rendercontext);
    }

    renderSlider(rendercontext);
  }

  bool handleEvent(const Event& event) override {
    if (auto mouse = std::get_if<MouseEvent>(&event)) {
      if (dragging) {
        if (mouse->action == MouseAction::Release) {
          dragging = false;
          return true;
        }
        if (mouse->action == MouseAction::Drag) {
          Orientation sliderOrientation =
              (orientation == Orientation::Horizontal) ? Orientation::Vertical : Orientation::Horizontal;
          dividerSlider.withValue(sliderValueFromMouse(
              mouse->x, mouse->y, sliderRect, sliderOrientation,
              dividerSlider.minValue, dividerSlider.maxValue, 0, 0, 0, 0));
          return true;
        }
      }

      if (mouse->action == MouseAction::Press && sliderRect.contains(mouse->x, mouse->y)) {
        dragging = true;
        Orientation sliderOrientation =
            (orientation == Orientation::Horizontal) ? Orientation::Vertical : Orientation::Horizontal;
        dividerSlider.withValue(sliderValueFromMouse(
            mouse->x, mouse->y, sliderRect, sliderOrientation,
            dividerSlider.minValue, dividerSlider.maxValue, 0, 0, 0, 0));
        return true;
      }

      if (second.widget && second.rect.contains(mouse->x, mouse->y)) {
        if (second.widget->handleEvent(event)) return true;
      }
      if (first.widget && first.rect.contains(mouse->x, mouse->y)) {
        if (first.widget->handleEvent(event)) return true;
      }
    }
    return false;
  }

 private:
  void renderSlider(RenderContext& rendercontext) {
    const Rect& rect = sliderRect;
    double norm = dividerSlider.normalizedValue();

    Orientation sliderOrientation =
        (orientation == Orientation::Horizontal) ? Orientation::Vertical : Orientation::Horizontal;

    if (sliderOrientation == Orientation::Horizontal) {
      if (rect.width < 3 || rect.height < 1) return;

      std::size_t trackLen = rect.width;
      std::size_t thumbPos = static_cast<std::size_t>(norm * static_cast<double>(trackLen - 1));
      std::size_t thumbY = rect.y + rect.height / 2;

      for (std::size_t i = 0; i < trackLen; i++) {
        Cell cell{dividerSlider.style.track, dividerSlider.trackStyle};
        rendercontext.setCell(rect.x + i, thumbY, cell);
      }

      Cell thumb{dividerSlider.style.thumb, dividerSlider.thumbStyle};
      rendercontext.setCell(rect.x + thumbPos, thumbY, thumb);
    } else {
      if (rect.height < 3 || rect.width < 1) return;

      std::size_t trackLen = rect.height;
      std::size_t thumbPos = static_cast<std::size_t>(norm * static_cast<double>(trackLen - 1));
      std::size_t thumbX = rect.x + rect.width / 2;

      for (std::size_t i = 0; i < trackLen; i++) {
        Cell cell{dividerSlider.style.track, dividerSlider.trackStyle};
        rendercontext.setCell(thumbX, rect.y + i, cell);
      }

      Cell thumb{dividerSlider.style.thumb, dividerSlider.thumbStyle};
      rendercontext.setCell(thumbX, rect.y + thumbPos, thumb);
    }
  }
};

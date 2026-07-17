#pragma once

#include <unistd.h>

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "ColumnContainer.hpp"
#include "GridContainer.hpp"
#include "RowContainer.hpp"
#include "box.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widget.hpp"
#include "widgetTree.hpp"

// ============================================================================
// Attribute representation for Layout configurations
// ============================================================================

struct RowAttr {
  int val;
};

struct ColAttr {
  int val;
};

// ============================================================================
// Layout Builders / DSL Elements
// ============================================================================

// The RowBuilder struct allows 'row' to be used both as a row layout builder:
//     row(child1, child2, ...)
// and as a grid attribute assignment:
//     row = 2
struct RowBuilder {
  template <typename... Args>
  auto operator()(Args&&... args) const {
    auto container = std::make_unique<Row>();
    auto process = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_convertible_v<T, std::unique_ptr<Widget>>) {
        container->addChild(std::move(arg));
      }
    };
    (process(std::forward<Args>(args)), ...);
    return container;
  }

  RowAttr operator=(int v) const { return RowAttr{v}; }
};

// The ColBuilder struct allows 'col' to be used both as a column layout builder:
//     col(child1, child2, ...)
// and as a grid attribute assignment:
//     col = 2
struct ColBuilder {
  template <typename... Args>
  auto operator()(Args&&... args) const {
    auto container = std::make_unique<Column>();
    auto process = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_convertible_v<T, std::unique_ptr<Widget>>) {
        container->addChild(std::move(arg));
      }
    };
    (process(std::forward<Args>(args)), ...);
    return container;
  }

  ColAttr operator=(int v) const { return ColAttr{v}; }
};

// Builders specifically named to match classes
struct ColumnBuilder {
  template <typename... Args>
  auto operator()(Args&&... args) const {
    auto container = std::make_unique<Column>();
    auto process = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_convertible_v<T, std::unique_ptr<Widget>>) {
        container->addChild(std::move(arg));
      }
    };
    (process(std::forward<Args>(args)), ...);
    return container;
  }
};

struct GridBuilder {
  template <typename... Args>
  auto operator()(Args&&... args) const {
    auto container = std::make_unique<Grid>(1, 1);
    auto process = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, RowAttr>) {
        container->rows = arg.val;
      } else if constexpr (std::is_same_v<T, ColAttr>) {
        container->columns = arg.val;
      } else if constexpr (std::is_convertible_v<T, std::unique_ptr<Widget>>) {
        container->addChild(std::move(arg));
      }
    };
    (process(std::forward<Args>(args)), ...);
    return container;
  }
};

struct TextBuilder {
  template <typename... Args>
  auto operator()(std::string t, Args&&... args) const {
    Style s = Style();
    Alignment a = Alignment::left;
    auto process = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, Style>) {
        s = std::move(arg);
      } else if constexpr (std::is_same_v<T, Alignment>) {
        a = arg;
      }
    };
    (process(std::forward<Args>(args)), ...);
    return std::make_unique<Text>(std::move(t), s, a);
  }
};

struct BoxBuilder {
  template <typename... Args>
  auto operator()(std::unique_ptr<Widget> child, Args&&... args) const {
    boxOutlineDetails outline = boxStyle::light;
    int border = 1;
    auto process = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, boxOutlineDetails>) {
        outline = arg;
      } else if constexpr (std::is_integral_v<T>) {
        border = static_cast<int>(arg);
      }
    };
    (process(std::forward<Args>(args)), ...);
    return std::make_unique<Box>(std::move(child), outline, border);
  }
};

// Builder for plural rows attribute: rows = X
struct RowsAttrBuilder {
  RowAttr operator=(int v) const { return RowAttr{v}; }
};

// Builder for plural cols attribute: cols = X
struct ColsAttrBuilder {
  ColAttr operator=(int v) const { return ColAttr{v}; }
};

// Global builder and attribute constants for UI composing
inline constexpr RowBuilder row{};
inline constexpr ColBuilder col{};
inline constexpr ColumnBuilder column{};
inline constexpr GridBuilder grid{};
inline constexpr TextBuilder text{};
inline constexpr BoxBuilder box{};

// Standalone plural attributes for cleaner differentiation
inline constexpr RowsAttrBuilder rows{};
inline constexpr ColsAttrBuilder cols{};
inline constexpr ColsAttrBuilder columns{};

// ============================================================================
// Root/Tree instantiator
// ============================================================================

template <typename... Args>
auto root(Args&&... args) {
  if constexpr (sizeof...(args) == 1) {
    auto helper = [](auto&& first) {
      using T = std::decay_t<decltype(first)>;
      if constexpr (std::is_convertible_v<T, std::unique_ptr<Widget>>) {
        return WidgetTree(std::move(first));
      } else {
        auto col_container = std::make_unique<Column>();
        return WidgetTree(std::move(col_container));
      }
    };
    return helper(std::forward<Args>(args)...);
  } else {
    auto col_container = std::make_unique<Column>();
    auto process = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_convertible_v<T, std::unique_ptr<Widget>>) {
        col_container->addChild(std::move(arg));
      }
    };
    (process(std::forward<Args>(args)), ...);
    return WidgetTree(std::move(col_container));
  }
}

// ============================================================================
// Application Runner Lifecycle Encapsulation
// ============================================================================

template <typename... Args>
inline void run_app(Args&&... args) {
  // 1. Initial terminal configuration
  tools::invisiableCursor();
  tools::clearScreen();

  // 2. RAII Guard to ensure cursor visibility is restored on scope exit (clean up)
  struct TerminalGuard {
    ~TerminalGuard() { tools::visiableCursor(); }
  } guard;

  // 3. Automatically build the WidgetTree from parameters
  WidgetTree tree = [&]() {
    if constexpr (sizeof...(Args) == 1) {
      using FirstType = std::decay_t<std::tuple_element_t<0, std::tuple<Args...>>>;
      if constexpr (std::is_same_v<FirstType, WidgetTree>) {
        return [](auto&& t) { return std::move(t); }(std::forward<Args>(args)...);
      } else {
        return root(std::forward<Args>(args)...);
      }
    } else {
      return root(std::forward<Args>(args)...);
    }
  }();

  // 4. Main execution loop
  for (;;) {
    tools::cursorHomePosition();
    tree.fb.resizeBuffer();
    tree.layout({0, 0, tree.fb.row, tree.fb.col});
    tree.render();
    tree.display();
    usleep(50000);
  }
}

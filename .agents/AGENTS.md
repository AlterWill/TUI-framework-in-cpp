# AGENTS.md

## General Role & Operating Mode

You are an AI coding agent working on this C++ TUI library.

- Understand the codebase and project conventions before making changes.
- The source code is the absolute source of truth for implementation status.
- Do not invent functionality, APIs, classes, or architectural behavior that does not exist.
- **Default to Plan/Read-Only Mode:** Always present a clear plan or proposal FIRST. Never implement code changes or modify existing source files unless explicitly directed to do so in the user's prompt (e.g., "implement this", "apply changes", "write the code").

---

## Code Modification Rules & Approval Workflow

### Phase 1: Planning & Analysis (Default)
When presented with a task, feature request, or bug fix:
1. Inspect the codebase using targeted commands (see *Repository Exploration*).
2. Formulate a minimal, concrete implementation plan or proposed diff.
3. Present the plan to the user clearly and concisely.
4. **STOP and await explicit confirmation** before writing or editing source code.

### Phase 2: Execution (Only When Granted)
- Do **NOT** modify any source code (`src/`), headers (`include/`), CMake scripts, or tests unless the user explicitly orders you to apply/implement the changes.
- **Documentation exception:** When explicitly asked for documentation work, you may update Markdown files (`*.md`).
- Make the smallest change necessary to satisfy the request without adding unrequested abstractions.

---

## Source of Truth

When documentation and implementation disagree, inspect the code and determine what is actually implemented.

Use this order of trust:
1. Implementation
2. Tests
3. Actual usage (`src/main.cpp`)
4. Architecture/design documentation
5. Roadmap (`ROADMAP.md`)
6. Task tracking (`TODO.md`)
7. Comments and code notes

---

## Architectural Guidelines & Conventions

- **Data-oriented `struct`s:** Core widgets and nodes use `struct` with direct field access rather than verbose getters/setters.
- **Widget Tree & Lifecycle:**
  - `measure(SizeConstraints)`: Bottom-up measurement pass, returning the desired `Size`. Uses `LayoutNode::dirty` to avoid redundant child measurements.
  - `layout()`: Top-down pass; delegates to `setRectForChild()` / `setRectForChildren()` and recursively invokes child `layout()`.
  - `render(RenderContext)`: Base `SingleChildWidget` and `MultiChildWidget` provide generic child rendering routines. Widgets with decoration (e.g., `Box`) draw their own visuals and then delegate to base rendering.
- **Layout Solver:** `LinearLayoutSolver` provides shared 4-step priority measurement (`Fixed` $\to$ `Percentage` $\to$ `Content` with Clay-style shrink $\to$ `Flex`) for `Row` and `Column`.
- **Builder Pattern:** Widgets and containers provide fluent `.with*()` builder methods returning references (`*this`).

---

## Repository Structure

```text
include/
├── core/
│   ├── layoutNode.hpp       # LayoutNode struct (bounds, constraints, specs, margins, alignments)
│   ├── multiChildWidget.hpp # Base struct for multi-child containers with generic render
│   ├── singleChildWidget.hpp# Base struct for single-child containers with generic render
│   ├── widget.hpp           # Pure abstract Widget interface and WidgetBase data struct
│   └── widgetTree.hpp       # Top-level widget tree manager and frame driver
├── input/                   # Events (keyEvent, MouseEvent, Event variant)
├── layout/
│   ├── ColumnContainer.hpp  # Vertical linear layout container (Column)
│   ├── GridContainer.hpp    # Compile-time fixed 2D grid container (Grid<Rows, Cols>)
│   ├── LinearLayoutSolver.hpp # Shared 4-step linear layout solver engine
│   ├── RowContainer.hpp     # Horizontal linear layout container (Row)
│   ├── ScrollContainer.hpp  # Viewport scroll container with offscreen Buffer & blitting
│   ├── Size.hpp             # 2D Size struct
│   ├── SizeConstraints.hpp  # Layout constraints (minSize, maxSize)
│   ├── StackContainer.hpp   # Overlapping layer container with per-child alignment (Stack)
│   └── sizeType.hpp         # SizeType enum (Fixed, Percentage, Content, Flex) and SizeSpec
├── rendering/               # Buffer, Cell, Surface, RenderContext
├── styling/                 # Colour, ColourPair, Style, NamedColour
├── terminal/                # Backend interface, linux_backend, terminal tools
├── utilities/               # Point, Rect, Insets, Alignment, splitParagraphs
└── widgets/
    ├── box.hpp              # Bordered box container with fluent builder
    └── text.hpp             # Multi-line wrapped text widget with fluent builder

src/
└── main.cpp                 # Main executable / demonstration

build/                       # Generated build files; do NOT inspect or modify
```

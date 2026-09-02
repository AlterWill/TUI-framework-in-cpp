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

## Repository Structure

```text
include/
├── core/       # Fundamental widget and tree infrastructure
├── input/      # Events and event handling
├── layout/     # Containers, sizing, and layout algorithms
├── rendering/  # Buffers, cells, surfaces, and render context
├── styling/    # Colours, colour pairs, and styles
├── terminal/   # Backend and terminal-specific functionality
├── utilities/  # Shared utility types and helper functionality
└── widgets/    # Concrete user-facing widgets

src/
└── main.cpp    # Main executable / current usage

build/          # Generated build files; do NOT inspect or modify

# AGENTS.md

## General Role

You are an AI coding agent working on this C++ TUI library.

Understand the existing codebase and project conventions before making changes.

The source code is the source of truth for implementation status.

Do not invent functionality, APIs, classes, or architectural behavior that does not exist.

---

## File Modification Rules

Do not modify files unless the user explicitly asks you to.

When the user asks for documentation changes, you may modify Markdown files:

* `*.md`
* `README.md`
* `ROADMAP.md`
* `TODO.md`
* `AGENTS.md`
* architecture/design documentation

Do not modify source code, headers, CMake files, tests, scripts, configuration files, or generated files unless explicitly requested.

You may inspect any repository files needed to understand the implementation.

---

## Source of Truth

When documentation and implementation disagree, inspect the code and determine what is actually implemented.

Use this order of trust:

1. Implementation
2. Tests
3. Actual usage
4. Architecture/design documentation
5. Roadmap
6. TODO
7. Comments and notes

Do not mark something as implemented simply because a file, class, interface, or documentation entry exists.

---

## Repository Structure

The repository is organized primarily by architectural responsibility:

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

build/          # Generated build files; do not inspect unless required
```

Use this directory structure as the primary guide for deciding which files are relevant to a task.

---

## Repository Exploration and Token Efficiency

Prefer efficient, targeted exploration over repeatedly opening unrelated files.

### Initial Structure Inspection

When first entering the repository or when the architecture is unclear, inspect the structure with:

```bash
tree --gitignore -L 3
```

Do not recursively inspect `build/` unless the task specifically concerns the build system or generated build information.

### Focused Tasks

For a task involving a specific subsystem, inspect only the relevant directory.

Examples:

```bash
bat include/layout/*.hpp
```

```bash
bat include/rendering/*.hpp
```

```bash
bat include/widgets/*.hpp
```

Search for relevant symbols before reading additional files when the task is focused on a particular class, function, or API.

Prefer:

```bash
rg "SymbolName" include src
```

Then inspect the files that actually contain relevant definitions or usage.

### Broad Architectural Tasks

For architecture or major refactoring tasks, first inspect the repository structure and identify the relevant subsystems.

Read multiple subsystem directories together only when their interaction is necessary. Do not dump the entire `include/` tree unless the task genuinely requires repository-wide understanding.

Also inspect:

```bash
bat src/main.cpp
```

when understanding how the library is currently used.

Do not perform a full repository dump for a small or focused task.

### Avoid Repeated Reads

Do not repeatedly open the same file when its relevant contents are already available in the current context.

Before reading a file again, determine whether the required information has already been obtained.

Use searches and targeted inspection to fill information gaps rather than rereading the entire subsystem.

---

## Build and Generated Files

The `build/` directory contains generated CMake/Ninja files and compiled artifacts.

Treat it as generated output.

Do not modify files inside `build/`.

Do not spend tokens inspecting generated build files unless they are directly relevant to diagnosing a build or compilation problem.

`compile_commands.json` may be used when useful for understanding compiler commands or tooling.

---

## Documentation Structure

Keep the documentation focused:

### README.md

Public project overview.

It should explain:

* What the project is
* What is currently implemented
* Core architecture at a high level
* How to build and run it
* Basic usage
* Current limitations
* Links to other documentation

Keep future plans out of the main feature description.

### ROADMAP.md

Development roadmap.

It should contain:

* Completed work
* Work in progress
* Planned work
* Long-term ideas

The status must match the actual implementation.

### TODO.md

Concrete implementation tasks.

Every task MUST use:

```markdown
### Task Name
- **Goal**: Explain what the task does and why it is needed.
- **Completion Requirement**: Clearly define what must be implemented or tested for the task to be complete.
```

Do not mark a task complete based only on documentation or assumptions. Verify against the implementation and tests.

---

## Codebase Understanding

Before making source changes:

1. Identify the relevant subsystem from the repository structure.
2. Search for the relevant symbols and usages.
3. Read the relevant interfaces and implementations.
4. Check related tests or current usage when available.
5. Make the smallest change necessary to satisfy the request.

Preserve existing project conventions unless the user explicitly asks for a redesign or refactor.

Do not introduce abstractions merely because they seem generally useful. Base changes on the existing architecture and the user's request.

---

## C++ Guidelines

Follow the existing C++ style and design already present in the project.

Before changing a class hierarchy, ownership model, layout system, or rendering architecture, inspect how the existing components interact rather than assuming a conventional design.

Be particularly careful with changes that affect common base classes or widely used interfaces, as these may require coordinated changes across multiple headers.

---

## Validation

After making changes, validate them using the project's existing build and test mechanisms where applicable.

Prefer targeted validation first.

For example, when changing the layout subsystem, inspect and validate the affected layout components rather than treating unrelated parts of the repository as changed.

Do not claim that something works unless it has been verified or the limitation is clearly stated.

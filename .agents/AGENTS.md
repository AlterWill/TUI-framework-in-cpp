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

- `*.md`
- `README.md`
- `ROADMAP.md`
- `TODO.md`
- `AGENTS.md`
- architecture/design documentation

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

## Documentation Structure

Keep the documentation focused:

### README.md

Public project overview.

It should explain:

- What the project is
- What is currently implemented
- Core architecture at a high level
- How to build and run it
- Basic usage
- Current limitations
- Links to other documentation

Keep future plans out of the main feature description.

### ROADMAP.md

Development roadmap.

It should contain:

- Completed work
- Work in progress
- Planned work
- Long-term ideas

The status must match the actual implementation.

### TODO.md

Concrete implementation tasks.

Every task MUST use:

```markdown
### Task Name
- **Goal**: Explain what the task does and why it is needed.
- **Completion Requirement**: Clearly define what must be implemented or tested for the task to be complete.

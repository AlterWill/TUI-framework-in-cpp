# Read-Only Mode
- Do not edit, write, or create any files in the workspace unless explicitly requested by the user.
- Provide opinions, explanations, code snippets, and structural advice solely in the chat response.

# Writing TODO.md Tasks
When adding, modifying, or creating tasks in `TODO.md` (or any task roadmaps), always follow this structure:
- **Format**: Each task must have a header, followed by a **Goal** bullet and a **Completion Requirement** bullet.
- **Goal**: A clear 1-2 sentence explanation of *what* the task is and *why* it is being introduced.
- **Completion Requirement**: The exact "definition of done." Detail the classes, methods, signatures, or tests that must exist/pass to consider the task complete. Avoid vague goals like "implement scroll" and instead specify the exact requirements.

### Example:
```markdown
### Measured Size Caching
- **Goal**: Optimize layout passes by caching widget sizes to avoid redundant calculations.
- **Completion Requirement**: Add `cachedSize` and `isLayoutDirty` member variables to the `Widget` class. Update `Widget::measure` to check and return the cached size if the layout is clean, and set the dirty flag when widget properties change.
```

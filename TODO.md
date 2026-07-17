 I have created a comprehensive technical design plan artifact detailing how these rendering
  pipeline features can be structured and implemented:

  • rendering_pipeline_design.md
  ──────

### What These Tasks Mean

  Here is an explanation of each item in Phase 2: Rendering Pipeline and how they apply to your TUI
  framework:

#### 1. ANSI Style Output

  Currently,  frameBuffer::display()  converts cell glyphs into raw UTF-8 text but ignores the
  Cell.sytle  structure (colors, bold, etc.).

  • Meaning: Translating formatting properties ( fg ,  bg ,  bold ,  underline ,  italic ) into
  ANSI escape sequences (e.g.,  \033[32;1m  for green bold) and appending them to the output string
  so the terminal renders rich colors and formatted text.

#### 2. Incremental Rendering

  Currently, the entire screen's content is rebuilt and printed to the terminal on every tick of
  the run loop.

  • Meaning: Only printing the characters (and their styles) that have actually changed since the
  last frame, and using cursor movement sequences (e.g.,  \033[Row;ColH ) to jump directly to those
  modified cells. This drastically reduces CPU, memory usage, terminal bandwidth, and eliminates
  screen flicker.

#### 3. Dirty Cell Tracking

  • Meaning: The underlying mechanism for incremental rendering. It compares the newly rendered
  buffer to the previous frame's buffer cell-by-cell. Only cells that differ in glyph, colors, or
  styles are marked as "dirty" and scheduled to be updated in the incremental render pass.

#### 4. Dirty Rectangle Tracking

  • Meaning: An optimization step above cell tracking. If a single widget updates, instead of
  generating a separate cursor jump command for every individual cell, the system groups changed
  cells into rectangular bounding boxes and renders them as contiguous blocks.

#### 5. Double Buffering

  • Meaning: Keeping two terminal buffers in memory:
      1. A front buffer representing what the terminal is currently showing.
      2. A back buffer where widgets currently write their new state.
      Once all widgets have finished rendering to the back buffer, the differences between the two
      are computed, written to stdout, and the back buffer is copied/swapped to the front buffer.

#### 6. Clipping

  • Meaning: Restricting drawing operations to a specific boundary. If a child widget attempts to
  write to a coordinate outside its parent's boundaries (e.g. text spilling over a box border), the
  framebuffer discards the write.

#### 7. Viewports

  • Meaning: Establishing a virtual window over a larger canvas (like a scrollable text area). The
  viewport translates local coordinate drawing calls (like rendering line 50 of a document) into
  the actual physical bounds on the screen (lines 5-15 of the terminal).

#### 8. Off-screen Rendering

  • Meaning: Drawing components to an independent, intermediate buffer instead of the main back
  buffer. This is used for caching static/complex widgets (re-using the cached image without re-
  rendering every frame), screenshots, or compositing layered popups/overlays.
                                                                      1 artifact · /artifact to review

Resume with -c (or command below):
agy --conversation=f507a9f6-8172-4d11-a9f3-621607a33ad3

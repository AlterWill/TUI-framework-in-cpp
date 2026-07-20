# Modern TUI Architecture

Building a modern Terminal User Interface (TUI) library is essentially like building a lightweight web browser or a 2D game engine that renders to a grid of characters instead of pixels.

Based on modern libraries (like `charmbracelet/bubbletea` in Go, `Textual` in Python, or `Ratatui` in Rust), here is the standard, battle-tested architecture for a declarative TUI framework.

---

## The 4-Layer Architecture

A well-designed TUI library is usually split into four distinct layers:

1. **Terminal Backend (I/O & Abstraction)**
2. **Rendering Engine (Buffers & Diffing)**
3. **UI Framework (Widgets & Layouts)**
4. **Application State & Event Loop (The Core)**

Let's break down each layer.

### 1. Terminal Backend (I/O Layer)
Terminals are notoriously chaotic. They communicate via streams of text mixed with invisible "ANSI Escape Sequences". The backend's job is to hide this mess.

* **Raw Mode:** Terminals normally buffer input until you press `Enter` (cooked mode) and intercept keys like `Ctrl+C`. Your library must immediately put the terminal into **Raw Mode** using OS-specific APIs (`termios` on Linux/macOS, `SetConsoleMode` on Windows). This gives you keystrokes immediately and stops the terminal from echoing what the user types.

* **Input Parsing:** When a user presses an arrow key, the terminal doesn't send "Arrow Up". It sends a sequence of bytes like `ESC [ A`. Your input parser acts like a state machine, consuming bytes and turning them into structured `KeyPress` events.

* **Output Generation:** To move the cursor or change colors, you print ANSI sequences (e.g., `\x1b[31m` for red). The backend provides a clean API so the rest of the app doesn't have to hardcode escape sequences.

### 2. The Rendering Engine (Double Buffering)
Printing to the terminal is very slow. If you clear the screen and redraw everything on every frame, the terminal will flicker horribly. Standard TUIs solve this using a **Virtual Screen** and **Double Buffering**.

* **The Cells:** The screen is modeled as a 2D grid of `Cell` objects. A `Cell` contains a character (or string, for wide UTF-8 graphemes) and its styling (foreground color, background color, bold, underline, etc.).

* **Front & Back Buffers:** You maintain two 2D grids (buffers) in memory. The **Front Buffer** represents what is *currently* on the terminal screen. The **Back Buffer** represents what you *want* to draw next.

* **The Diffing Algorithm (Incremental Rendering):** 
  1. Your UI widgets draw themselves onto the **Back Buffer** in memory.
  2. The engine compares the Back Buffer against the Front Buffer cell by cell.
  3. It generates the minimum necessary ANSI escape codes to update *only* the cells that changed.
  4. It flushes this optimized string to the terminal and swaps the buffers.

### 3. UI Framework (Widgets & Layout)
This is the layer the developer interacts with. Modern architectures typically use a **Declarative Widget Tree** (similar to Flutter or React).

* **The Widget Tree:** The UI is a tree of nodes. A `Window` might contain a `Column`, which contains a `Text` widget and a `Button`.

* **The Two-Pass Layout System:** Before drawing, the engine must figure out exactly where everything goes on the grid.

  1. **Measure Pass (Bottom-Up):** The root asks its children "How much space do you need?", passing down maximum constraints. Children calculate their size (factoring in text wrapping, margins, etc.) and return their desired dimensions.

  2. **Position Pass (Top-Down):** The parent assigns actual coordinates (X, Y, Width, Height) to its children based on the layout rules (e.g., Flexbox, Grid) and the measured sizes.

* **The Render Pass:** Once layout is done, the engine walks the tree again. Each widget is given a reference to a section of the **Back Buffer** (often clamped/clipped to its assigned bounds) and writes its characters and colors into it.

### 4. Application State & Event Loop
The heart that keeps everything moving is the event loop.

1. **Wait for Event:** The engine blocks, waiting for an event (a keystroke, a mouse click, a terminal resize signal `SIGWINCH`, or a custom application message).
2. **Event Routing:** The event is dispatched. 
   - Global events (like `Ctrl+C` to quit) are handled by the engine.
   - UI events are routed to the currently **focused** widget in the tree.
3. **State Update:** The widget (or your application logic) handles the event and updates its internal state.
4. **Re-render:** If the state changed, the engine triggers a layout pass and a render pass, updating the Back Buffer, diffing it, and flushing the changes to the screen.

---

## Evaluating Your Implementation

If you want to verify you are on the right track with your framework, ask yourself these questions:

1. **Are you drawing directly to standard output inside your widgets?** 
   * *If yes, this is an anti-pattern.* Widgets should write to an in-memory buffer. The renderer should handle outputting to the terminal.
2. **Does your engine handle terminal resizes smoothly?**
   * You should be catching the `SIGWINCH` signal on Linux/macOS, which tells you the terminal dimensions changed, prompting an immediate full layout re-calculation.
3. **How do you handle UTF-8 and Wide Characters?**
   * Standard `char` is 1 byte, but emojis and CJK characters can be up to 4 bytes and take up 2 terminal grid columns. Your `Cell` struct needs to account for character width (using functions like `wcwidth`), or your layout will break when rendering non-ASCII text.
4. **Is your layout engine decoupled from your rendering?**
   * A widget should know its `X, Y, W, H` *before* its `render()` method is ever called.


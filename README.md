# TUI Library Architecture Notes

## Goal

Build a React/JSX-inspired TUI library in C++ that:

* Uses a JSX-like syntax
* Parses files into a tree structure
* Supports custom widgets
* Supports multiple layout systems
* Avoids major rewrites later
* Separates parsing from rendering

---

# High-Level Architecture

```text
App.jsx
    ↓
Lexer
    ↓
Tokens
    ↓
Parser
    ↓
AST / Node Tree
    ↓
Widget Creation
    ↓
Layout Pass
    ↓
Render Pass
    ↓
Terminal Buffer
    ↓
Terminal
```

---

# Important Principle

The parser should NOT know about:

* Buttons
* Text widgets
* Layout algorithms
* Rendering
* Events

The parser only builds a tree.

---

# JSX Example

Input:

```jsx
<Box layout="vertical">
    <Text>Hello</Text>
    <Button text="OK" />
</Box>
```

Parser Output:

```text
Box
├── Text
│   └── Hello
└── Button
```

---

# Lexer

Purpose:

Convert raw text into tokens.

Example:

```jsx
<Button text="OK" />
```

Tokens:

```text
<
Button
text
=
"OK"
/>
```

Lexer should:

* Read characters
* Create tokens
* Ignore whitespace when appropriate

Lexer does NOT understand UI structure.

---

# Parser

Purpose:

Convert tokens into a tree.

Input:

```text
Tokens
```

Output:

```cpp
Node Tree
```

The parser should use recursive descent.

Example:

```jsx
<Box>
    <Text>Hello</Text>
</Box>
```

Produces:

```text
Box
└── Text
    └── Hello
```

---

# AST

AST = Abstract Syntax Tree

The AST is the output of the parser.

For this project, the AST can also be the runtime UI tree.

Example:

```cpp
struct Node {
    ...
};
```

---

# Recommended Node Structure

```cpp
struct Rect {
    int x;
    int y;
    int width;
    int height;
};

using Value = std::variant<
    int,
    float,
    bool,
    std::string
>;

struct Prop {
    std::string name;
    Value value;
};

struct Node {
    std::string type;

    std::vector<Prop> props;

    Rect rect;

    Node* parent = nullptr;

    std::vector<std::unique_ptr<Node>> children;
};
```

---

# Why Props Exist

Example:

```jsx
<Button text="OK" width="20" />
```

Props:

```cpp
{
    {"text", "OK"},
    {"width", 20}
}
```

Props are simply attributes from tags.

---

# Why Not Store Widget-Specific Structs In The Parser

Bad:

```cpp
ButtonProps
TextProps
InputProps
```

The parser would need to know every widget.

Good:

```cpp
Node {
    type = "Button"
}
```

Widget code interprets props later.

---

# unique_ptr

Purpose:

Automatic memory management.

Example:

```cpp
std::unique_ptr<Node>
```

Benefits:

* No manual delete
* No memory leaks
* Children are destroyed automatically

Perfect for trees.

---

# Layout System

Recommended:

```jsx
<Box layout="vertical">
```

instead of:

```jsx
<VBox>
```

because it reduces special cases.

---

Supported Layout Types

```text
vertical
horizontal
grid
```

Example:

```jsx
<Box layout="vertical">
```

```jsx
<Box layout="horizontal">
```

```jsx
<Box layout="grid">
```

---

# Layout Pass

Input:

```text
Node Tree
```

Output:

```text
Node Tree + Rectangles
```

Example:

```cpp
Sidebar.rect = {0,0,30,40};
Main.rect    = {30,0,70,40};
```

The layout pass computes sizes and positions.

---

# Render Pass

Input:

```text
Node Tree with Rects
```

Output:

```text
Terminal Buffer
```

Rendering is usually DFS:

```cpp
render(root);
```

---

# Widget Registry

Do NOT use giant if statements.

Bad:

```cpp
if(type == "Button")
```

Good:

```cpp
registry["Button"] = createButton;
registry["Text"] = createText;
```

This allows adding widgets without touching parser code.

---

# Future Features To Leave Room For

Likely additions:

```text
focus
events
padding
margin
styles
visibility
min/max sizes
ids
classes
themes
```

Store these as props.

Do not hardcode them into the parser.

---

# Development Order

Phase 1

* Node structure
* Rect structure
* Props system

Phase 2

* Lexer

Phase 3

* Parser

Phase 4

* Print AST for debugging

Example:

```text
Box
├── Text
└── Button
```

Phase 5

* Layout engine

Phase 6

* Terminal renderer

Phase 7

* Widget registry

Phase 8

* Keyboard input

Phase 9

* Focus system

Phase 10

* State management

---

# Core Rule

Keep these stable:

```text
Lexer
Parser
Node Tree
```

Most future changes should happen in:

```text
Widgets
Layouts
Rendering
Events
State
```

If the Node tree is generic enough, adding widgets, layouts, and styles later should not require rewriting the lexer or parser.

## How to Build and Run

Since this project uses C++20 modules, a modern generator like **Ninja** is required for CMake to scan module dependencies correctly.

### 1. Clean (Optional)
If you get a generator mismatch or cache error, run:
```bash
rm -rf build
```

### 2. Configure the Build
Generate the build system using the Ninja generator and export compilation commands for the LSP (language server):
```bash
cmake -G Ninja -B build
```

### 3. Build the Project
Compile the program:
```bash
cmake --build build
```

### 4. Run the Program
Run the built TUI executable:
```bash
./build/tui
```

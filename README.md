# TUI - Terminal User Interface Framework

A modern, declarative Terminal User Interface (TUI) framework written in C++20. This project aims to provide a stable, extensible foundation for building rich terminal applications with a declarative DSL, flexible layouts, and an efficient rendering engine.

## Features

- **Core Engine:** Widget tree, declarative DSL, layout and render passes, UTF-8 rendering.
- **Widgets:** Text, Box, layout containers (Row, Column, Grid).
- **Rendering:** ANSI style output, incremental rendering, double buffering.
- **Layout & Styling:** Margins, padding, per-widget styling.

Check out the [Roadmap](ROADMAP.md) to see current progress and future milestones, and [TODO](TODO.md) for pending tasks.

## Requirements

- C++20 compatible compiler (e.g., `clang++` or `g++`)
- CMake 3.28 or higher
- Ninja (optional, but recommended)

## Getting Started

```bash
# Clone the repository
git clone https://github.com/AlterWill/TUI.git
cd TUI

# Clean any existing build artifacts
rm -rf build

# Configure using CMake with the Ninja generator
cmake -G Ninja -B build

# Build the executable
cmake --build build

# Run the program
./build/tui
```

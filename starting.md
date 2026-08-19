# Getting Started

Follow these steps to clone, build, and run the C++ TUI framework demonstration.

## Prerequisites

- **C++ Compiler**: GCC 10+ or Clang 11+ (supporting C++20 standard)
- **Build System**: CMake (v3.20 or newer)
- **Generator**: Ninja (recommended) or GNU Make

## Quick Start

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

# Run the binary
./build/tui
```

## Running Benchmarks and Debugging

When executing `./build/tui`, press `q` to terminate the application and output frame timing statistics (resize, layout, render, display, and event processing benchmark breakdowns).

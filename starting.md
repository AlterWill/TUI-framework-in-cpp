# Getting Started

Follow these steps to clone, build, and run the C++ TUI framework demonstration.

## Prerequisites

- **C++ Compiler**: Clang (`clang++`) with C++20 support — the build system explicitly sets `CMAKE_CXX_COMPILER=clang++`
- **Build System**: CMake 3.28 or newer
- **Generator**: Ninja (recommended) or GNU Make
- **Platform**: Linux — the only implemented terminal backend is `linux_backend`

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

Press `q` to exit. The demo prints frame startup timing in milliseconds after exiting.

## Enabling Logging

To enable diagnostic file logging, pass `-DENABLE_LOGGING=ON` at configure time:

```bash
cmake -G Ninja -B build -DENABLE_LOGGING=ON
cmake --build build
./build/tui
# Diagnostic output will be written to tui_debug.log
```

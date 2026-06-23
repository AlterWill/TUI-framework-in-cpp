cmake_minimum_required(VERSION 3.28)

# Enforce Clang compiler for C++20 modules compatibility with clangd LSP
set(CMAKE_CXX_COMPILER clang++)

project(TUI LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for LSP (clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the executable target
add_executable(tui)

# Add sources
target_sources(tui
  PRIVATE
    src/main.cpp
    src/tools.cpp
    src/unicode.cpp
)

# Add header search path
target_include_directories(tui PRIVATE include)

# Set common compiler warnings and optimization levels
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(tui PRIVATE -Wall -Wextra -O2)
endif()

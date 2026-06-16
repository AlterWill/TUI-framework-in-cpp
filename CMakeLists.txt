cmake_minimum_required(VERSION 3.28)
project(TUI LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the executable target
add_executable(tui)

# Add sources and specify C++20 modules using FILE_SET CXX_MODULES
target_sources(tui
  PRIVATE
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
  PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS src
    FILES
      src/boxTypes.cppm
      src/tools.cppm
      src/unicode.cppm
      src/terminal.cppm
)

# Set common compiler warnings and optimization levels
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(tui PRIVATE -Wall -Wextra -O2)
endif()

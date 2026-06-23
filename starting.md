 ### Getting Started
---
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
 

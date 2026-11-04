# Requirements

- Compiler: GCC (g++) ver 15.20.0
- C++ Standard: C++20

# Build Instructions

From root directory:
> g++ -std=c++20 -Iinclude src/benchmark.cpp src/rabin_karp.cpp src/kmp.cpp src/text_generator.cpp src/main.cpp -o main.exe

# 🚀 stlish — Modern C++ STL-like Containers

A collection of STL-compatible containers implemented from scratch using modern C++20.

## 📚 Containers

### ✅ Implemented
- **`stlish::vector<T>`** — Dynamic array with move semantics, exception safety
- **`stlish::forward_list<T>`** — Singly-linked list (Phase 1 complete)

### 🚧 In Progress
- `forward_list` — Copy/Move constructors (Phase 2)
- Iterators (Phase 4)

### 📋 Planned
- `list` — Doubly-linked list
- `deque` — Double-ended queue
- `stack`, `queue` — Adapter containers
- `set`, `map` — Tree-based containers
- `unordered_set`, `unordered_map` — Hash-based containers

## 🛠️ Build Requirements

- **CMake** 3.20+
- **C++20** compiler (MSVC 19.28+, GCC 10+, Clang 12+)
- **Ninja** (optional, recommended)

## ⚡ Quick Start

### Clone & Build
```bash
git clone https://github.com/YeghiazarianVahe/stlish.git
cd stlish

# Configure
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

### Windows (PowerShell)
```powershell
# Configure
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Test
ctest --test-dir build --output-on-failure
```

## 📖 Usage Examples

### Vector
```cpp
#include "stlish/vector.hpp"

stlish::vector<int> v;
v.push_back(10);
v.push_back(20);
v.push_back(30);

for (int x : v) {
    std::cout << x << " ";  // 10 20 30
}
```

### Forward List
```cpp
#include "stlish/forward_list.hpp"

stlish::forward_list<std::string> list;
list.push_front("world");
list.push_front("hello");

std::cout << list.front();  // "hello"
```

## 🧪 Testing

All containers are tested with **Catch2** framework.
```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific container tests
ctest --test-dir build -R forward_list
ctest --test-dir build -R vector
```

## 📊 Project Structure
```
stlish/
├── include/stlish/          # Header-only library
│   ├── vector.hpp
│   └── forward_list.hpp
├── tests/                   # Unit tests
│   ├── test_forward_list.cpp
│   └── test_vector_*.cpp
├── examples/                # Usage examples
├── docs/                    # Documentation
└── CMakeLists.txt
```

## 🎓 Learning Goals

This project demonstrates:
- ✅ RAII and Rule of 5
- ✅ Move semantics and perfect forwarding
- ✅ Exception safety guarantees
- ✅ Template metaprogramming
- ✅ STL-compliant API design
- ✅ Modern CMake practices
- ✅ Test-driven development

## 📜 License

MIT License - see [LICENSE](LICENSE) file

## 🤝 Contributing

This is a personal learning project. Feedback and suggestions are welcome via issues!

## 📌 Status

**Current Phase:** Forward List — Phase 2 (Rule of 5)

**Completion:**
- Vector: ~90% (production-ready)
- Forward List: ~30% (basic operations complete)

---

**Author:** Vahe Yeghiazaryan  
**Repository:** [github.com/YeghiazarianVahe/stlish](https://github.com/YeghiazarianVahe/stlish)


MIT License

Copyright (c) 2025 Vahe Yeghiazaryan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
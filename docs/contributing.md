# Contributing to stlish::vector

Welcome! Thank you for your interest in contributing to `stlish::vector`. This project aims to be an educational, high-quality implementation of a dynamic array container, and we're excited to have you here.

---

## 📋 Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Setup](#development-setup)
4. [How to Contribute](#how-to-contribute)
5. [Coding Standards](#coding-standards)
6. [Testing Guidelines](#testing-guidelines)
7. [Documentation Standards](#documentation-standards)
8. [Commit Message Guidelines](#commit-message-guidelines)
9. [Pull Request Process](#pull-request-process)
10. [Project Roadmap](#project-roadmap)

---

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors, regardless of:
- Experience level
- Background
- Identity
- Perspective

### Expected Behavior

✅ Be respectful and constructive in all interactions  
✅ Welcome newcomers and help them learn  
✅ Give and receive feedback graciously  
✅ Focus on what's best for the project and community  
✅ Show empathy towards other community members  

### Unacceptable Behavior

❌ Harassment, discrimination, or offensive comments  
❌ Personal attacks or trolling  
❌ Publishing others' private information  
❌ Disruptive or off-topic discussions  

**Reporting**: If you experience or witness unacceptable behavior, please contact the project maintainers.

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:

- **C++20 compatible compiler**:
  - GCC 10+ or Clang 13+ or MSVC 19.29+
- **CMake 3.20+** (for building tests)
- **Git** for version control
- **Basic understanding** of:
  - C++ templates
  - Memory management
  - STL containers

### Project Structure

```
stlish/
├── include/
|   └── stlish/
|       └── vector.hpp              # Main implementation
├── tests/                  # Unit tests
│   ├── test_constructors.cpp
│   ├── test_modifiers.cpp
│   ├── test_capacity.cpp
│   └── ...
├── docs/                   # Documentation
│   ├── api_map.md
│   ├── complexity.md
│   ├── exception_safety.md
│   ├── invariants.md
│   ├── iterator_invalidation.md
│   └── spec.md
├── examples/               # Usage examples
├── benchmarks/             # Performance benchmarks
├── CMakeLists.txt
└── README.md
```

---

## Development Setup

### 1. Fork and Clone

```bash
# Fork the repository on GitHub, then:
git clone https://github.com/YeghiazarianVahe/stlish.git
cd stlish-vector
```

### 2. Create a Branch

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/issue-number-brief-description
```

**Branch Naming Conventions**:
- `feature/` - New features (e.g., `feature/insert-operation`)
- `fix/` - Bug fixes (e.g., `fix/123-memory-leak`)
- `docs/` - Documentation updates (e.g., `docs/improve-readme`)
- `refactor/` - Code refactoring (e.g., `refactor/simplify-reserve`)
- `test/` - Test additions/improvements (e.g., `test/add-move-semantics`)

### 3. Build the Project

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 4. Run Tests

```bash
ctest --output-on-failure
# or
./tests/vector_tests
```

---

## How to Contribute

### Types of Contributions

We welcome various types of contributions:

1. **Bug Reports** 🐛
2. **Feature Requests** 💡
3. **Code Contributions** 💻
4. **Documentation Improvements** 📚
5. **Performance Optimizations** ⚡
6. **Test Coverage** 🧪
7. **Examples and Tutorials** 📖

---

### 1. Reporting Bugs 🐛

**Before submitting**, check:
- Is this a known issue? Search existing issues first.
- Can you reproduce it consistently?
- Does it happen with the latest version?

**Bug Report Template**:

```markdown
### Description
Clear description of the bug.

### Steps to Reproduce
1. Step one
2. Step two
3. ...

### Expected Behavior
What should happen?

### Actual Behavior
What actually happens?

### Environment
- Compiler: GCC 11.2
- OS: Ubuntu 22.04
- C++ Standard: C++20

### Code Sample
```cpp
// Minimal reproducible example
stlish::vector<int> v;
v.push_back(42);
// Bug occurs here...
```

### Additional Context
Any other relevant information.
```

---

### 2. Requesting Features 💡

**Feature Request Template**:

```markdown
### Feature Description
Clear, concise description of the proposed feature.

### Motivation
Why is this feature needed? What problem does it solve?

### Proposed API
```cpp
// Example of how the feature would be used
v.insert(v.begin() + 2, 42);
```

### Alternatives Considered
What other approaches did you consider?

### Additional Context
Any other relevant information, references, or examples.
```

---

### 3. Contributing Code 💻

#### Small Changes
For typos, small bug fixes, or minor improvements:
1. Make your changes
2. Test locally
3. Submit a pull request

#### Major Changes
For new features or significant refactoring:
1. **Open an issue first** to discuss the change
2. Wait for maintainer feedback
3. Implement after approval
4. Submit a pull request

---

## Coding Standards

### C++ Style Guidelines

We follow a consistent coding style based on modern C++ best practices.

#### 1. Naming Conventions

```cpp
// Classes: PascalCase
class Vector { };

// Functions and variables: snake_case
void push_back(const T& value);
size_t element_count;

// Constants: UPPER_SNAKE_CASE
constexpr size_t MAX_SIZE = 1000;

// Template parameters: PascalCase
template <class T, class Allocator>

// Private members: trailing underscore
class vector {
private:
    size_t size_;
    size_t capacity_;
    T* data_;
};
```

#### 2. Formatting

**Indentation**: 4 spaces (no tabs)

**Braces**: Opening brace on same line
```cpp
void function() {
    if (condition) {
        // code
    }
}
```

**Line Length**: Prefer ≤100 characters, max 120

**Spaces**:
```cpp
// Around operators
int x = a + b;

// After commas
function(arg1, arg2, arg3);

// No space before opening parenthesis
if (condition) { }
for (int i = 0; i < n; ++i) { }
```

#### 3. Code Organization

**Header Guards**: Use `#ifndef` / `#define` / `#endif`
```cpp
#ifndef STLISH_VECTOR_HPP
#define STLISH_VECTOR_HPP

// ... content ...

#endif // STLISH_VECTOR_HPP
```

**Include Order**:
1. Related header (if .cpp file)
2. C++ standard library headers
3. Third-party library headers
4. Project headers

```cpp
#include "vector.hpp"           // 1. Related header

#include <algorithm>            // 2. C++ standard library
#include <memory>
#include <stdexcept>

#include "third_party/lib.hpp"  // 3. Third-party

#include "stlish/utils.hpp"     // 4. Project headers
```

#### 4. Modern C++ Practices

**Use const and constexpr**:
```cpp
// Mark const methods
size_t size() const noexcept;

// Use constexpr for compile-time constants
static constexpr size_t MIN_CAPACITY = 1;
```

**Use noexcept**:
```cpp
// Mark noexcept when appropriate
void swap(vector& other) noexcept;
vector(vector&& other) noexcept;
```

**Use auto judiciously**:
```cpp
// ✅ Good: Type is obvious or verbose
auto it = v.begin();
auto ptr = std::make_unique<Widget>();

// ❌ Avoid: Type is not obvious
auto x = calculate();  // What type is x?
```

**Prefer range-based for loops**:
```cpp
// ✅ Modern
for (const auto& elem : container) {
    // ...
}

// ❌ Old style (use only when necessary)
for (size_t i = 0; i < container.size(); ++i) {
    // ...
}
```

**Use nullptr, not NULL**:
```cpp
T* ptr = nullptr;  // ✅
T* ptr = NULL;     // ❌
```

#### 5. Comments and Documentation

**Doxygen-style comments** for public APIs:
```cpp
/**
 * @brief Adds an element to the end of the vector
 * @param value The element to add
 * @throws std::bad_alloc if allocation fails
 * 
 * If the new size exceeds capacity, the vector is reallocated.
 * All iterators are invalidated if reallocation occurs.
 * 
 * Complexity: Amortized O(1)
 */
void push_back(const T& value);
```

**Inline comments** for complex logic:
```cpp
// Calculate new capacity using 1.5x growth factor
// This balances memory usage and reallocation frequency
const size_t new_cap = capacity_ + (capacity_ / 2);
```

**No redundant comments**:
```cpp
// ❌ Bad: Obvious comment
i++;  // Increment i

// ✅ Good: Explains why, not what
i++;  // Skip the header row
```

#### 6. Error Handling

**Use exceptions** for error conditions:
```cpp
if (idx >= size_) {
    throw std::out_of_range("vector::at: index out of range");
}
```

**Document exception guarantees**:
```cpp
/**
 * @throws std::bad_alloc if memory allocation fails
 * @throws May throw any exception thrown by T's constructor
 * 
 * Exception Safety: Strong guarantee (commit or rollback)
 */
void reserve(size_t new_cap);
```

---

## Testing Guidelines

### Test Framework

We use **Google Test** for unit testing.

### Writing Tests

#### Test File Organization

One test file per major feature:
- `test_constructors.cpp` - All constructor tests
- `test_modifiers.cpp` - push_back, pop_back, etc.
- `test_capacity.cpp` - size, capacity, reserve, etc.
- `test_element_access.cpp` - operator[], at, front, back
- `test_iterators.cpp` - Iterator operations
- `test_exception_safety.cpp` - Exception behavior

#### Test Naming

```cpp
TEST(CategoryName, SpecificBehavior) {
    // Test code
}
```

**Examples**:
```cpp
TEST(VectorConstructors, DefaultConstructorCreatesEmptyVector) {
    stlish::vector<int> v;
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_TRUE(v.empty());
}

TEST(VectorModifiers, PushBackIncreasesSize) {
    stlish::vector<int> v;
    v.push_back(42);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], 42);
}

TEST(VectorExceptionSafety, ReserveProviesStrongGuarantee) {
    // Test exception safety
}
```

#### Test Structure

Use **Arrange-Act-Assert** pattern:

```cpp
TEST(VectorModifiers, PopBackRemovesLastElement) {
    // Arrange
    stlish::vector<int> v = {1, 2, 3, 4, 5};
    
    // Act
    v.pop_back();
    
    // Assert
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v.back(), 4);
}
```

#### Test Coverage Requirements

All contributions should maintain or improve test coverage:

- ✅ **All public methods** must have tests
- ✅ **Edge cases** must be covered:
  - Empty containers
  - Single element
  - Capacity boundaries
  - Large sizes
- ✅ **Error conditions** must be tested:
  - Out of bounds access
  - Allocation failures (where possible)
  - Exception safety
- ✅ **Move semantics** must be verified
- ✅ **Copy semantics** must be verified

#### Example Comprehensive Test

```cpp
TEST(VectorReserve, IncreasesCapacityPreservesElements) {
    // Arrange
    stlish::vector<int> v = {1, 2, 3};
    size_t old_size = v.size();
    size_t old_capacity = v.capacity();
    
    // Act
    v.reserve(100);
    
    // Assert - capacity increased
    EXPECT_GE(v.capacity(), 100);
    EXPECT_GT(v.capacity(), old_capacity);
    
    // Assert - size unchanged
    EXPECT_EQ(v.size(), old_size);
    
    // Assert - elements preserved
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    
    // Assert - can add more without reallocation
    size_t cap_before = v.capacity();
    for (int i = 0; i < 50; ++i) {
        v.push_back(i);
    }
    EXPECT_EQ(v.capacity(), cap_before);  // No reallocation
}
```

### Running Tests

```bash
# Run all tests
ctest

# Run specific test
./tests/vector_tests --gtest_filter=VectorModifiers.*

# Run with verbose output
ctest -V

# Check coverage (if configured)
make coverage
```

---

## Documentation Standards

### When to Update Documentation

Documentation should be updated when:

- Adding new features
- Changing existing behavior
- Fixing bugs that affect documented behavior
- Improving clarity or correctness

### Documentation Files

#### README.md
- Project overview
- Quick start guide
- Installation instructions
- Basic examples

#### API Documentation
Update relevant sections in:
- `api_map.md` - API compatibility table
- `complexity.md` - Time/space complexity analysis
- `exception_safety.md` - Exception guarantees
- `invariants.md` - Class invariants
- `iterator_invalidation.md` - Iterator invalidation rules
- `spec.md` - Complete specification

#### Code Comments
- All public APIs must have Doxygen comments
- Complex algorithms should have explanatory comments
- Tricky edge cases should be documented

### Documentation Style

**Be Clear and Concise**:
```markdown
❌ Bad:
The function does stuff with the vector and makes it bigger.

✅ Good:
Increases the vector's capacity to at least `new_cap` elements.
If `new_cap` is less than or equal to current capacity, does nothing.
```

**Use Examples**:
```markdown
#### Example
```cpp
stlish::vector<int> v;
v.reserve(100);  // Pre-allocate space
for (int i = 0; i < 100; ++i) {
    v.push_back(i);  // No reallocations
}
```
```

**Document Edge Cases**:
```markdown
**Note**: Calling `pop_back()` on an empty vector is undefined behavior.
```

---

## Commit Message Guidelines

### Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Type
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Formatting, missing semicolons, etc.
- `refactor`: Code change that neither fixes a bug nor adds a feature
- `perf`: Performance improvement
- `test`: Adding or updating tests
- `chore`: Maintenance tasks

### Scope
The feature or component affected:
- `vector`: Core vector implementation
- `tests`: Test suite
- `docs`: Documentation
- `build`: Build system

### Subject
- Use imperative mood ("Add feature" not "Added feature")
- Don't capitalize first letter
- No period at the end
- Max 50 characters

### Body
- Wrap at 72 characters
- Explain **what** and **why**, not **how**
- Separate from subject with blank line

### Examples

```
feat(vector): add insert() method

Implement insert() method to add elements at arbitrary positions.
Uses reallocation strategy to maintain amortized O(1) growth.

Closes #42
```

```
fix(vector): prevent iterator invalidation in emplace_back

Previously, emplace_back could invalidate iterators even when
capacity was sufficient. Now properly checks capacity before
construction.

Fixes #78
```

```
docs(readme): add installation instructions

Add step-by-step guide for building and installing the library.
Includes instructions for CMake and manual header installation.
```

```
test(modifiers): add exception safety tests for resize

Add comprehensive tests verifying strong exception guarantee
for resize() when growing within capacity, and basic guarantee
when reallocation is required.
```

---

## Pull Request Process

### Before Submitting

1. ✅ **Code compiles** without warnings
2. ✅ **All tests pass** (`ctest`)
3. ✅ **Code follows style guidelines**
4. ✅ **Documentation is updated**
5. ✅ **Commit messages are clear**
6. ✅ **Branch is up-to-date** with main

### Pull Request Template

```markdown
## Description
Brief description of changes.

## Type of Change
- [ ] Bug fix (non-breaking change fixing an issue)
- [ ] New feature (non-breaking change adding functionality)
- [ ] Breaking change (fix or feature causing existing functionality to break)
- [ ] Documentation update

## Related Issues
Closes #123
Relates to #456

## Testing
Describe testing performed:
- [ ] Unit tests added/updated
- [ ] Manual testing performed
- [ ] All tests pass

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review of code performed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings generated
- [ ] Tests added for new functionality
- [ ] All tests pass

## Screenshots (if applicable)
For visual changes or new examples.
```

### Review Process

1. **Automated Checks**: CI runs tests and checks
2. **Code Review**: Maintainer reviews code
3. **Discussion**: Address any feedback
4. **Approval**: Maintainer approves PR
5. **Merge**: PR is merged into main branch

### After Merge

- Your contribution will be acknowledged in release notes
- Thank you for contributing! 🎉

---

## Project Roadmap

### Current Status

✅ **Implemented**:
- Core vector functionality
- Move semantics
- Exception safety
- Comprehensive documentation

### Planned Features

#### High Priority
- [ ] `insert()` / `erase()` operations
- [ ] Reverse iterators
- [ ] Additional unit tests
- [ ] Performance benchmarks

#### Medium Priority
- [ ] `assign()` operations
- [ ] C++20 three-way comparison (`<=>`)
- [ ] Allocator support
- [ ] Extended documentation examples

#### Low Priority
- [ ] Constexpr support (C++20)
- [ ] Range constructors (C++20 ranges)
- [ ] Additional helper functions

### How to Help

Check our [issue tracker](link) for:
- 🏷️ `good first issue` - Great for newcomers
- 🏷️ `help wanted` - We need assistance
- 🏷️ `documentation` - Docs need improvement
- 🏷️ `enhancement` - New features to implement

---

## Questions?

- 💬 **Discussions**: Use GitHub Discussions for questions
- 🐛 **Issues**: Use GitHub Issues for bugs and features
- 📧 **Email**: Contact maintainers at [email]

---

## Recognition

All contributors will be acknowledged in:
- `CONTRIBUTORS.md` file
- Release notes
- Project README

Thank you for contributing to `stlish::vector`! Your effort helps make this project better for everyone learning about container implementations.

---

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (specify license, e.g., MIT License).

---

*Generated for `stlish::vector` - A guide for contributors*
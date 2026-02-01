# API Map - stlish::vector

## Overview
This document provides a complete mapping between `stlish::vector` and `std::vector` APIs, showing equivalencies, differences, and implementation choices.

---

## 📋 Member Types

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `value_type` | `value_type` | ✅ Identical | Element type `T` |
| `size_type` | `size_type` | ✅ Identical | `std::size_t` |
| `difference_type` | `difference_type` | ✅ Identical | `std::ptrdiff_t` |
| `reference` | `reference` | ✅ Identical | `value_type&` |
| `const_reference` | `const_reference` | ✅ Identical | `const value_type&` |
| `pointer` | `pointer` | ✅ Identical | `value_type*` |
| `const_pointer` | `const_pointer` | ✅ Identical | `const value_type*` |
| `iterator` | `iterator` | ⚠️ Simplified | Raw pointer instead of custom iterator class |
| `const_iterator` | `const_iterator` | ⚠️ Simplified | Const pointer instead of custom iterator class |
| ❌ Not implemented | `reverse_iterator` | ❌ Missing | Not yet implemented |
| ❌ Not implemented | `const_reverse_iterator` | ❌ Missing | Not yet implemented |
| ❌ Not implemented | `allocator_type` | ❌ Missing | Uses direct memory allocation |

---

## 🏗️ Constructors & Destructor

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `vector()` | `vector()` | ✅ Identical | Default constructor |
| `explicit vector(size_type n)` | `explicit vector(size_type n)` | ✅ Identical | N default-constructed elements |
| `vector(size_type n, const T& value)` | `vector(size_type n, const T& value)` | ✅ Identical | N copies of value |
| `vector(const vector& other)` | `vector(const vector& other)` | ✅ Identical | Copy constructor |
| `vector(vector&& other)` | `vector(vector&& other)` | ✅ Identical | Move constructor |
| `vector(InputIt first, InputIt last)` | `vector(InputIt first, InputIt last)` | ⚠️ Different | Uses C++20 concepts instead of SFINAE |
| `vector(std::initializer_list<T>)` | `vector(std::initializer_list<T>)` | ✅ Identical | Initializer list constructor |
| ❌ Not implemented | `explicit vector(const Allocator&)` | ❌ Missing | No custom allocator support |
| `~vector()` | `~vector()` | ✅ Identical | Destructor |

### Implementation Difference - Range Constructor
```cpp
// stlish::vector - Modern C++20 approach
template <class InputIt>
    requires(!std::is_integral_v<InputIt> && std::forward_iterator<InputIt>)
vector(InputIt first, InputIt last);

// std::vector - Traditional SFINAE approach
template <class InputIt, 
          std::enable_if_t<std::is_base_of_v<
              std::input_iterator_tag, 
              typename std::iterator_traits<InputIt>::iterator_category>, int> = 0>
vector(InputIt first, InputIt last);
```

---

## 📝 Assignment Operators

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `operator=(const vector&)` | `operator=(const vector&)` | ✅ Identical | Copy assignment |
| `operator=(vector&&)` | `operator=(vector&&)` | ✅ Identical | Move assignment |
| `operator=(std::initializer_list<T>)` | `operator=(std::initializer_list<T>)` | ✅ Identical | Initializer list assignment |
| ❌ Not implemented | `assign(size_type, const T&)` | ❌ Missing | Not yet implemented |
| ❌ Not implemented | `assign(InputIt, InputIt)` | ❌ Missing | Not yet implemented |
| ❌ Not implemented | `assign(std::initializer_list<T>)` | ❌ Missing | Not yet implemented |
| ❌ Not implemented | `get_allocator()` | ❌ Missing | No allocator support |

---

## 📊 Capacity Functions

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `size()` | `size()` | ✅ Identical | Returns number of elements |
| `capacity()` | `capacity()` | ✅ Identical | Returns allocated capacity |
| `empty()` | `empty()` | ✅ Identical | Checks if container is empty |
| `max_size()` | `max_size()` | ⚠️ Different | Custom calculation, likely similar result |
| `reserve(size_type)` | `reserve(size_type)` | ✅ Identical | Reserves capacity |
| `shrink_to_fit()` | `shrink_to_fit()` | ✅ Identical | Reduces capacity to size |

### max_size() Implementation Detail
```cpp
size_type max_size() const noexcept {
    return std::min(
        static_cast<size_type>(std::numeric_limits<difference_type>::max()),
        static_cast<size_type>(-1) / sizeof(value_type)
    );
}
```
This ensures the maximum size doesn't overflow `difference_type` and fits in addressable memory.

---

## 🎯 Element Access

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `data()` | `data()` | ✅ Identical | Returns pointer to underlying array |
| `operator[](size_type)` | `operator[](size_type)` | ✅ Identical | Unchecked access |
| `at(size_type)` | `at(size_type)` | ✅ Identical | Checked access, throws `out_of_range` |
| `front()` | `front()` | ✅ Identical | Access first element |
| `back()` | `back()` | ✅ Identical | Access last element |

**Safety Note**: Both `operator[]` and element access functions (`front()`, `back()`) have undefined behavior when called on empty vectors. Use `at()` for bounds-checked access.

---

## 🔄 Iterators

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `begin()` | `begin()` | ✅ Identical | Iterator to beginning |
| `end()` | `end()` | ✅ Identical | Iterator to end |
| `cbegin()` | `cbegin()` | ✅ Identical | Const iterator to beginning |
| `cend()` | `cend()` | ✅ Identical | Const iterator to end |
| ❌ Not implemented | `rbegin()` | ❌ Missing | Reverse iterator not implemented |
| ❌ Not implemented | `rend()` | ❌ Missing | Reverse iterator not implemented |
| ❌ Not implemented | `crbegin()` | ❌ Missing | Const reverse iterator not implemented |
| ❌ Not implemented | `crend()` | ❌ Missing | Const reverse iterator not implemented |

**Implementation Note**: Iterators are implemented as raw pointers (`T*` and `const T*`) which is a valid and performant approach, but differs from `std::vector`'s iterator class wrapper.

---

## ✏️ Modifiers

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `clear()` | `clear()` | ✅ Identical | Removes all elements |
| `push_back(const T&)` | `push_back(const T&)` | ✅ Identical | Copy-adds element at end |
| `push_back(T&&)` | `push_back(T&&)` | ✅ Identical | Move-adds element at end |
| `emplace_back(Args&&...)` | `emplace_back(Args&&...)` | ✅ Identical | Constructs element in-place |
| `pop_back()` | `pop_back()` | ✅ Identical | Removes last element |
| `resize(size_type)` | `resize(size_type)` | ✅ Identical | Resizes with default values |
| `resize(size_type, const T&)` | `resize(size_type, const T&)` | ✅ Identical | Resizes with given value |
| `swap(vector&)` | `swap(vector&)` | ✅ Identical | Swaps contents |
| ❌ Not implemented | `insert(...)` | ❌ Missing | All insert overloads |
| ❌ Not implemented | `emplace(...)` | ❌ Missing | In-place insertion |
| ❌ Not implemented | `erase(...)` | ❌ Missing | All erase overloads |

---

## 🔧 Non-member Functions

| stlish::vector | std::vector | Status | Notes |
|----------------|-------------|--------|-------|
| `swap(vector&, vector&)` | `swap(vector&, vector&)` | ✅ Identical | Non-member swap |
| `operator==` | `operator==` | ✅ Identical | Equality comparison |
| `operator!=` | `operator!=` | ✅ Identical | Inequality comparison |
| `operator<` | `operator<` | ✅ Identical | Lexicographical less-than |
| `operator<=` | `operator<=` | ✅ Identical | Lexicographical less-equal |
| `operator>` | `operator>` | ✅ Identical | Lexicographical greater-than |
| `operator>=` | `operator>=` | ✅ Identical | Lexicographical greater-equal |
| ❌ Not implemented | `operator<=>` (C++20) | ❌ Missing | Three-way comparison |
| ❌ Not implemented | `erase(vector&, const T&)` | ❌ Missing | C++20 uniform container erasure |
| ❌ Not implemented | `erase_if(vector&, Pred)` | ❌ Missing | C++20 uniform container erasure |

---

## 📈 Summary Statistics

### ✅ Implemented Features: 32/50 (64%)

**Fully Implemented:**
- All constructors (except allocator-based)
- Rule of Five (copy/move semantics)
- All capacity functions
- All element access functions
- Basic iterators (forward only)
- Core modifiers (push, pop, resize, clear)
- All comparison operators (except `<=>`)

**Not Yet Implemented:**
- Reverse iterators
- Custom allocator support
- Insert/Emplace operations
- Erase operations
- Assign operations
- C++20 three-way comparison

---

## 🎯 Usage Compatibility

### Drop-in Replacement ✅
For basic usage patterns, `stlish::vector` is a **drop-in replacement** for `std::vector`:

```cpp
// All of this works identically
stlish::vector<int> v1;
stlish::vector<int> v2{1, 2, 3, 4, 5};
v1.push_back(42);
v1.emplace_back(43);
int x = v2[0];
int y = v2.at(1);

for (auto& elem : v2) {
    // Range-based for loop works!
}
```

### Known Limitations ⚠️
These patterns will **NOT** work:

```cpp
// ❌ Reverse iteration
for (auto it = v.rbegin(); it != v.rend(); ++it) { }

// ❌ Insert/Erase operations
v.insert(v.begin() + 2, 99);
v.erase(v.begin());

// ❌ Range assign
v.assign(other.begin(), other.end());

// ❌ Custom allocator
stlish::vector<int, MyAllocator> v; // Won't compile
```

---

## 📚 Next Steps

To achieve full `std::vector` compatibility, implement in this order:

1. **Insert/Erase operations** - Most commonly needed
2. **Reverse iterators** - Required for algorithms
3. **Assign operations** - Useful for bulk modifications
4. **Custom allocator support** - Advanced use cases
5. **C++20 features** (`operator<=>`, `erase_if`) - Modern compatibility

---

*Generated for `stlish::vector` implementation - A learning-focused STL reimplementation*
# Complexity Analysis - stlish::vector

## Overview
This document provides detailed time and space complexity analysis for all operations in `stlish::vector`, with explanations of why each operation has its particular complexity.

---

## 📊 Complexity Legend

| Symbol | Meaning |
|--------|---------|
| **O(1)** | Constant time - independent of container size |
| **O(n)** | Linear time - proportional to number of elements |
| **Amortized O(1)** | Average constant time over many operations, but individual ops may be O(n) |

---

## 🏗️ Construction & Assignment

### Constructors

#### Default Constructor
```cpp
vector() noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply initializes three member variables (`size_`, `capacity_`, `data_`) to zero/null. No memory allocation occurs.

```cpp
// Implementation
vector() noexcept 
    : size_(0), capacity_(0), data_(nullptr) {}
```

---

#### Fill Constructor (n elements)
```cpp
explicit vector(size_type n);
```
- **Time**: **O(n)**
- **Space**: **O(n)**
- **Explanation**: Must allocate memory for `n` elements and default-construct each one. Both allocation and construction are O(n).

**Breakdown:**
1. Memory allocation: O(n) - requesting n * sizeof(T) bytes
2. Default construction: O(n) - calling default constructor n times
3. Total: O(n) + O(n) = **O(n)**

---

#### Fill Constructor (n copies)
```cpp
vector(size_type n, const_reference value);
```
- **Time**: **O(n)**
- **Space**: **O(n)**
- **Explanation**: Allocates memory for `n` elements and copy-constructs each from `value`.

**Breakdown:**
1. Memory allocation: O(n)
2. Copy construction: O(n) - copying value n times
3. Total: **O(n)**

---

#### Copy Constructor
```cpp
vector(const vector& other);
```
- **Time**: **O(n)** where n = other.size()
- **Space**: **O(n)**
- **Explanation**: Must deep-copy all elements from the source vector.

**Breakdown:**
1. Memory allocation: O(n)
2. Copy construction of elements: O(n)
3. Total: **O(n)**

```cpp
// Simplified implementation
vector(const vector& other) 
    : size_(other.size_),
      capacity_(other.capacity_),
      data_(allocate_raw(other.capacity_)) {
    uninitialized_copy(data_, other.data_, other.data_ + other.size_);
}
```

---

#### Move Constructor
```cpp
vector(vector&& other) noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply steals the pointer from `other`, no element copying occurs.

**Breakdown:**
1. Copy three pointers: O(1)
2. Reset source vector: O(1)
3. Total: **O(1)**

```cpp
// Implementation
vector(vector&& other) noexcept
    : size_(other.size_),
      capacity_(other.capacity_),
      data_(other.data_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}
```

**Why so fast?** Move semantics transfer ownership instead of copying data!

---

#### Range Constructor
```cpp
template <class InputIt>
    requires(!std::is_integral_v<InputIt> && std::forward_iterator<InputIt>)
vector(InputIt first, InputIt last);
```
- **Time**: **O(n)** where n = distance(first, last)
- **Space**: **O(n)**
- **Explanation**: Must copy all elements from the range [first, last).

**Note**: For forward iterators, we can calculate distance in O(n), then allocate once and copy. For input iterators (not supported here), we'd need multiple reallocations.

---

#### Initializer List Constructor
```cpp
vector(std::initializer_list<value_type> init);
```
- **Time**: **O(n)** where n = init.size()
- **Space**: **O(n)**
- **Explanation**: Equivalent to range constructor - copies all elements from the initializer list.

---

### Assignment Operators

#### Copy Assignment
```cpp
vector& operator=(const vector& other);
```
- **Time**: **O(n + m)** where n = size(), m = other.size()
- **Space**: **O(m)**
- **Explanation**: Must destroy current elements and copy new ones.

**Breakdown:**
1. Destroy current elements: O(n)
2. Allocate new memory (if needed): O(1)
3. Copy construct new elements: O(m)
4. Total: O(n) + O(m) = **O(n + m)**

**Implementation uses copy-and-swap idiom:**
```cpp
vector& operator=(const vector& other) {
    vector tmp(other);  // O(m) - copy construction
    swap(tmp);          // O(1) - swap pointers
    return *this;       // tmp destructor destroys old data: O(n)
}
```

---

#### Move Assignment
```cpp
vector& operator=(vector&& other) noexcept;
```
- **Time**: **O(n)** where n = size()
- **Space**: **O(1)**
- **Explanation**: Must destroy current elements, then steal other's resources.

**Breakdown:**
1. Destroy current elements: O(n)
2. Steal pointers from other: O(1)
3. Total: **O(n)**

---

#### Swap
```cpp
void swap(vector& other) noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply swaps three member variables.

```cpp
void swap(vector& other) noexcept {
    std::swap(data_, other.data_);      // O(1)
    std::swap(size_, other.size_);      // O(1)
    std::swap(capacity_, other.capacity_); // O(1)
}
```

---

## 📏 Capacity Operations

### size()
```cpp
size_type size() const noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply returns the stored `size_` member.

---

### capacity()
```cpp
size_type capacity() const noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply returns the stored `capacity_` member.

---

### empty()
```cpp
bool empty() const noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply compares `size_` with 0.

---

### max_size()
```cpp
size_type max_size() const noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simple arithmetic calculation, no loops.

---

### reserve()
```cpp
void reserve(size_type new_cap);
```
- **Time**: **O(n)** if reallocation occurs, **O(1)** otherwise
- **Space**: **O(new_cap)**
- **Explanation**: If `new_cap > capacity()`, must allocate new memory and move all elements.

**Best Case**: new_cap ≤ capacity() → **O(1)** (no-op)
**Worst Case**: new_cap > capacity() → **O(n)** (reallocation required)

**Breakdown when reallocating:**
1. Allocate new memory: O(new_cap) ≈ O(1) amortized
2. Move/copy n elements: O(n)
3. Destroy old elements: O(n)
4. Deallocate old memory: O(1)
5. Total: **O(n)**

```cpp
void reserve(size_type new_cap) {
    if (new_cap <= capacity_) {
        return;  // O(1) - early exit
    }
    reallocate(new_cap);  // O(n) - must move all elements
}
```

---

### shrink_to_fit()
```cpp
void shrink_to_fit();
```
- **Time**: **O(n)** if shrinking occurs, **O(1)** otherwise
- **Space**: **O(size())**
- **Explanation**: If `size() < capacity()`, must reallocate to smaller buffer and move elements.

**Best Case**: size() == capacity() → **O(1)** (no-op)
**Worst Case**: size() < capacity() → **O(n)** (reallocation required)

---

## 🎯 Element Access

### operator[]
```cpp
reference operator[](size_type idx) noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Direct pointer arithmetic: `data_[idx]` is compiled to `*(data_ + idx)`.

```cpp
reference operator[](size_type idx) noexcept {
    return data_[idx];  // Single pointer dereference
}
```

---

### at()
```cpp
reference at(size_type idx);
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Bounds check + pointer arithmetic, both are constant time.

```cpp
reference at(size_type idx) {
    check_index(idx, size_);  // O(1) - simple comparison
    return data_[idx];        // O(1) - pointer dereference
}
```

**Note**: Throwing an exception doesn't affect complexity - it's still O(1).

---

### front() / back()
```cpp
reference front() noexcept;
reference back() noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Direct pointer access to first/last element.

```cpp
reference front() noexcept { return data_[0]; }
reference back() noexcept { return data_[size_ - 1]; }
```

---

### data()
```cpp
pointer data() noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply returns the `data_` pointer.

---

## 🔄 Iterators

### begin() / end() / cbegin() / cend()
```cpp
iterator begin() noexcept;
iterator end() noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Iterators are pointers, so returning them is a single pointer copy.

```cpp
iterator begin() noexcept { return data_; }
iterator end() noexcept { return data_ + size_; }
```

---

## ✏️ Modifiers

### clear()
```cpp
void clear() noexcept;
```
- **Time**: **O(n)**
- **Space**: **O(1)**
- **Explanation**: Must call destructor for each element.

```cpp
void clear() noexcept {
    destroy_range(data_, data_ + size_);  // O(n) - destroy n elements
    size_ = 0;                            // O(1)
}
```

**Note**: For trivially destructible types (like `int`), compilers can optimize this to O(1) by skipping destructor calls.

---

### push_back()
```cpp
void push_back(const_reference value);
void push_back(value_type&& value);
```
- **Time**: **Amortized O(1)**, worst-case **O(n)**
- **Space**: **O(1)** amortized
- **Explanation**: Usually just constructs at end, but occasionally triggers reallocation.

**Analysis:**

1. **Best Case** (space available): **O(1)**
   - Just construct one element at `data_[size_]`

2. **Worst Case** (reallocation needed): **O(n)**
   - Allocate new buffer: O(1)
   - Move n existing elements: O(n)
   - Construct new element: O(1)
   - Total: O(n)

**Amortized Analysis:**

Vector typically uses **geometric growth** (e.g., doubling capacity):
- Start with capacity 1
- Push 1 element → reallocate to capacity 2 (cost: 1)
- Push 1 element → reallocate to capacity 4 (cost: 2)
- Push 2 elements → reallocate to capacity 8 (cost: 4)
- Push 4 elements → reallocate to capacity 16 (cost: 8)

Total cost for n pushes: 1 + 2 + 4 + 8 + ... + n ≈ **2n**

Average cost per push: 2n / n = **2 = O(1)**

This is why we say **amortized O(1)**!

---

### emplace_back()
```cpp
template <class... Args>
reference emplace_back(Args&&... args);
```
- **Time**: **Amortized O(1)**, worst-case **O(n)**
- **Space**: **O(1)** amortized
- **Explanation**: Same as `push_back()`, but constructs in-place.

**Advantage over push_back**: Avoids creating a temporary object.

```cpp
// push_back - creates temporary
std::string temp = "hello";
v.push_back(temp);  // Copy constructor called

// emplace_back - constructs in-place
v.emplace_back("hello");  // Direct construction, no temporary
```

---

### pop_back()
```cpp
void pop_back() noexcept;
```
- **Time**: **O(1)**
- **Space**: **O(1)**
- **Explanation**: Simply calls destructor on last element and decrements size.

```cpp
void pop_back() noexcept {
    if (size_ == 0) return;  // O(1)
    --size_;                 // O(1)
    data_[size_].~value_type();  // O(1) - single destructor call
}
```

**Note**: Does not deallocate memory (capacity unchanged).

---

### resize()
```cpp
void resize(size_type n);
void resize(size_type n, const_reference value);
```
- **Time**: **O(|n - size()|)** in general, **O(n)** worst-case
- **Space**: **O(n)** if n > capacity()
- **Explanation**: May need to construct new elements or destroy existing ones.

**Case Analysis:**

1. **n < size()**: **O(size() - n)**
   - Destroy (size() - n) elements

2. **n == size()**: **O(1)**
   - No-op

3. **n > size() and n ≤ capacity()**: **O(n - size())**
   - Construct (n - size()) new elements

4. **n > capacity()**: **O(n)**
   - Reallocate: O(size()) to move existing elements
   - Construct: O(n - size()) new elements
   - Total: O(size()) + O(n - size()) = O(n)

```cpp
void resize(size_type n, const_reference value) {
    if (n < size_) {
        destroy_range(data_ + n, data_ + size_);  // O(size - n)
        size_ = n;
    } else if (n > size_) {
        reserve(n);  // O(size) if reallocation needed
        for (size_type i = size_; i < n; ++i) {  // O(n - size)
            ::new (static_cast<void*>(data_ + i)) value_type(value);
        }
        size_ = n;
    }
}
```

---

## 🔍 Comparison Operators

### operator==
```cpp
bool operator==(const vector& lhs, const vector& rhs);
```
- **Time**: **O(n)** where n = min(lhs.size(), rhs.size())
- **Space**: **O(1)**
- **Explanation**: Must compare each element pair-wise until finding a difference.

**Best Case**: Different sizes → **O(1)** (early exit)
**Worst Case**: All elements equal → **O(n)**

```cpp
template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;  // O(1) early exit
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());  // O(n)
}
```

---

### operator<
```cpp
bool operator<(const vector& lhs, const vector& rhs);
```
- **Time**: **O(n)** where n = min(lhs.size(), rhs.size())
- **Space**: **O(1)**
- **Explanation**: Lexicographical comparison examines elements until finding difference.

**Best Case**: First elements differ → **O(1)**
**Worst Case**: All common elements equal → **O(min(n, m))**

---

### Other Comparisons (!=, <=, >, >=)
All implemented in terms of `operator==` and `operator<`, so have the same complexity.

---

## 📈 Memory Growth Strategy

### Capacity Growth
The implementation uses a **geometric growth strategy**:

```cpp
size_type grow_capacity_for(size_type min_cap) const {
    const size_type geometric = capacity_ + (capacity_ / 2);  // 1.5x growth
    return std::max(geometric, min_cap);
}
```

**Why 1.5x instead of 2x?**
- **Memory reuse**: After several reallocations, freed blocks can be reused
- **Cache efficiency**: Smaller growth = better cache utilization
- **Trade-off**: 1.5x requires more reallocations than 2x, but uses less memory

**Growth sequence example** (starting from 1):
1 → 1 → 2 → 3 → 4 → 6 → 9 → 13 → 19 → 28 → 42 → ...

---

## 📊 Complexity Summary Table

| Operation | Average | Worst Case | Notes |
|-----------|---------|------------|-------|
| **Construction** |
| Default constructor | O(1) | O(1) | Just initialize pointers |
| Fill constructor | O(n) | O(n) | Must construct n elements |
| Copy constructor | O(n) | O(n) | Must deep-copy all elements |
| Move constructor | O(1) | O(1) | Just steal pointers |
| Range constructor | O(n) | O(n) | Copy from range |
| **Assignment** |
| Copy assignment | O(n+m) | O(n+m) | Destroy + copy |
| Move assignment | O(n) | O(n) | Destroy current |
| Swap | O(1) | O(1) | Swap pointers |
| **Capacity** |
| size(), capacity(), empty() | O(1) | O(1) | Member access |
| reserve() | O(1) | O(n) | Amortized O(1) |
| shrink_to_fit() | O(1) | O(n) | Only if shrinking |
| **Element Access** |
| operator[], at(), front(), back() | O(1) | O(1) | Direct access |
| data() | O(1) | O(1) | Pointer return |
| **Iterators** |
| begin(), end(), etc. | O(1) | O(1) | Pointer arithmetic |
| **Modifiers** |
| clear() | O(n) | O(n) | Destroy all elements |
| push_back() | **O(1)*** | O(n) | *Amortized |
| emplace_back() | **O(1)*** | O(n) | *Amortized |
| pop_back() | O(1) | O(1) | Single destructor |
| resize() | O(|Δ|) | O(n) | Δ = size change |
| **Comparisons** |
| ==, !=, <, <=, >, >= | O(1) | O(n) | Element-wise compare |

---

## 🎓 Key Takeaways

1. **Amortized Analysis is Crucial**: `push_back()` is O(n) in worst case but **amortized O(1)**, making it efficient for repeated insertions.

2. **Move Semantics are Fast**: Move operations are always O(1) because they transfer ownership instead of copying data.

3. **Reserve() Prevents Reallocations**: If you know the size ahead of time, `reserve()` once to avoid multiple O(n) reallocations.

4. **Capacity Never Shrinks (Except shrink_to_fit())**: `pop_back()`, `resize()`, and `clear()` don't deallocate memory.

5. **Pointer-based Iterators are O(1)**: Unlike more complex iterator implementations, raw pointers allow constant-time operations.

---

*Generated for `stlish::vector` implementation - Understanding the performance characteristics*
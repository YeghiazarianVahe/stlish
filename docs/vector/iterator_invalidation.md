# Iterator Invalidation - stlish::vector

## Overview
This document explains **when and why iterators, pointers, and references** to `stlish::vector` elements become **invalid** (dangling). Understanding iterator invalidation is critical for writing correct code with containers.

---

## 🎯 What is Iterator Invalidation?

An **invalidated iterator** is an iterator that no longer refers to the object it once pointed to. Using an invalidated iterator results in **undefined behavior**.

### Types of Invalidation

1. **Complete Invalidation**: All iterators, pointers, and references become invalid
2. **Partial Invalidation**: Some iterators become invalid, others remain valid
3. **No Invalidation**: All iterators, pointers, and references remain valid

---

## 📋 Quick Reference Table

| Operation | Iterators | Pointers & References | Notes |
|-----------|-----------|----------------------|-------|
| **Construction** | N/A | N/A | No existing iterators |
| **Copy/Move Assignment** | ✅ All invalidated | ✅ All invalidated | New storage |
| **reserve()** | ⚠️ **If reallocates**: All invalidated<br>✅ **Otherwise**: Valid | Same | Check capacity |
| **shrink_to_fit()** | ⚠️ **If shrinks**: All invalidated<br>✅ **Otherwise**: Valid | Same | May or may not shrink |
| **push_back()** | ⚠️ **If reallocates**: All invalidated<br>✅ **Otherwise**: Valid | Same | Reallocation at capacity |
| **emplace_back()** | ⚠️ **If reallocates**: All invalidated<br>✅ **Otherwise**: Valid | Same | Same as push_back |
| **pop_back()** | ⚠️ Last element iterator invalid<br>✅ Others valid | ⚠️ Last element references invalid<br>✅ Others valid | Only affects last element |
| **resize()** | ⚠️ **If grows past capacity**: All invalidated<br>⚠️ **If shrinks**: Beyond new size invalid<br>✅ **Otherwise**: Valid | Same | Complex behavior |
| **clear()** | ✅ All invalidated | ✅ All invalidated | size becomes 0 |
| **swap()** | ✅ All invalidated | ✅ All invalidated | Points to different vector |
| **operator[]** | ✅ Valid | ✅ Valid | Just access, no modification |
| **at()** | ✅ Valid | ✅ Valid | Just access, no modification |

---

## 🔍 Detailed Analysis by Operation

### 1. Construction & Assignment

#### Construction
```cpp
vector<int> v1;               // No iterators yet
vector<int> v2{1, 2, 3};      // No iterators yet
vector<int> v3 = v2;          // No iterators yet
vector<int> v4 = std::move(v2);  // v2's iterators are NOW invalid!
```

**Key Point**: Only move construction invalidates iterators of the **source** vector.

---

#### Copy Assignment
```cpp
vector<int> v1 = {1, 2, 3};
auto it = v1.begin();  // Iterator to v1

vector<int> v2 = {4, 5, 6};
v1 = v2;  // Copy assignment

// ❌ 'it' is now INVALID!
// The old buffer was deallocated
*it;  // UNDEFINED BEHAVIOR
```

**Invalidation**: ✅ **All** iterators, pointers, and references to `v1` are invalidated.

**Why?** Copy-and-swap idiom creates new buffer, destroys old one.

---

#### Move Assignment
```cpp
vector<int> v1 = {1, 2, 3};
auto it1 = v1.begin();  // Iterator to v1

vector<int> v2 = {4, 5, 6};
auto it2 = v2.begin();  // Iterator to v2

v1 = std::move(v2);  // Move assignment

// ❌ 'it1' is now INVALID (v1's old buffer was destroyed)
// ❌ 'it2' is now INVALID (v2's buffer was moved to v1)
*it1;  // UNDEFINED BEHAVIOR
*it2;  // UNDEFINED BEHAVIOR
```

**Invalidation**: ✅ **All** iterators to **both** vectors are invalidated.

---

#### swap()
```cpp
vector<int> v1 = {1, 2, 3};
vector<int> v2 = {4, 5, 6};

auto it1 = v1.begin();  // Points to v1's buffer
auto it2 = v2.begin();  // Points to v2's buffer

v1.swap(v2);  // Swap buffers

// ❌ 'it1' now points to what is v2's buffer!
// ❌ 'it2' now points to what is v1's buffer!
*it1;  // Accesses v2's elements!
*it2;  // Accesses v1's elements!
```

**Invalidation**: ✅ **All** iterators are invalidated (they point to wrong vector).

**Technically**: The iterators still point to valid memory, but they're associated with the wrong vector object. This is considered invalidation.

---

### 2. Capacity Operations

#### reserve() - Conditional Invalidation
```cpp
vector<int> v = {1, 2, 3};
v.reserve(10);  // capacity: 3 → 10

auto it = v.begin();

v.reserve(5);  // No-op, already have capacity >= 5
// ✅ 'it' is STILL VALID

v.reserve(20);  // Reallocation: capacity 10 → 20
// ❌ 'it' is now INVALID
```

**Invalidation**:
- ⚠️ **If reallocation occurs**: All iterators invalidated
- ✅ **If no reallocation**: All iterators remain valid

**How to Check**:
```cpp
size_t old_cap = v.capacity();
v.reserve(new_cap);
if (v.capacity() != old_cap) {
    // Reallocation occurred → iterators invalidated
}
```

**Implementation Detail**:
```cpp
void reserve(size_type new_cap) {
    if (new_cap <= capacity_) {
        return;  // No reallocation → no invalidation
    }
    reallocate(new_cap);  // Reallocation → all iterators invalidated
}
```

---

#### shrink_to_fit() - Conditional Invalidation
```cpp
vector<int> v = {1, 2, 3};
v.reserve(100);  // capacity = 100

auto it = v.begin();

v.shrink_to_fit();  // Tries to shrink capacity to size
// ❌ 'it' is LIKELY invalid (if shrinking occurred)
```

**Invalidation**:
- ⚠️ **If shrinking occurs**: All iterators invalidated
- ✅ **If no shrinking**: All iterators remain valid

**Note**: `shrink_to_fit()` is a **non-binding request**. The implementation may choose not to shrink.

**Safe Pattern**:
```cpp
auto old_cap = v.capacity();
v.shrink_to_fit();
if (v.capacity() != old_cap) {
    // Shrinking occurred → assume all iterators invalidated
}
```

---

### 3. Modifiers

#### push_back() / emplace_back() - Conditional Invalidation

**Case 1: No Reallocation (size < capacity)**
```cpp
vector<int> v = {1, 2, 3};
v.reserve(10);  // capacity = 10, size = 3

auto it = v.begin();
auto end = v.end();

v.push_back(4);  // size = 4, capacity = 10 (no reallocation)

// ✅ 'it' is STILL VALID - points to same memory
// ❌ 'end' is INVALID - end() moved forward!
*it;     // OK: still points to '1'
*end;    // UNDEFINED BEHAVIOR: end was 3, now it's 4
```

**Invalidation**: 
- ✅ Iterators to existing elements remain valid
- ❌ `end()` iterator is invalidated

---

**Case 2: Reallocation (size == capacity)**
```cpp
vector<int> v = {1, 2, 3};  // capacity = 3, size = 3

auto it = v.begin();
auto end = v.end();

v.push_back(4);  // size = 4, capacity grows (reallocation!)

// ❌ 'it' is now INVALID
// ❌ 'end' is now INVALID
*it;   // UNDEFINED BEHAVIOR
*end;  // UNDEFINED BEHAVIOR
```

**Invalidation**: ✅ **All** iterators invalidated.

---

**How to Detect**:
```cpp
auto it = v.begin();
auto old_cap = v.capacity();

v.push_back(42);

if (v.capacity() != old_cap) {
    // Reallocation occurred
    // ❌ 'it' is invalid, don't use it!
    it = v.begin();  // Get new iterator
}
```

---

**Safe Pattern - Reserve First**:
```cpp
vector<int> v;
v.reserve(100);  // Pre-allocate space

for (int i = 0; i < 100; ++i) {
    v.push_back(i);  // No reallocations, iterators stay valid
}
```

---

#### pop_back() - Partial Invalidation
```cpp
vector<int> v = {1, 2, 3, 4, 5};

auto it = v.begin();      // Points to v[0]
auto last = v.end() - 1;  // Points to v[4]

v.pop_back();  // Remove last element

// ✅ 'it' is STILL VALID - points to same location
// ❌ 'last' is INVALID - element was destroyed
*it;    // OK: still points to '1'
*last;  // UNDEFINED BEHAVIOR: element destroyed
```

**Invalidation**:
- ❌ Iterators/pointers/references to the **last element** are invalidated
- ✅ All other iterators remain valid
- ❌ `end()` iterator is invalidated

**Implementation**:
```cpp
void pop_back() noexcept {
    --size_;
    data_[size_].~value_type();  // Destructor called
    // Memory still allocated, but object destroyed
}
```

---

#### resize() - Complex Invalidation

**Case 1: Shrinking (n < size)**
```cpp
vector<int> v = {1, 2, 3, 4, 5};

auto it = v.begin();      // Points to v[0]
auto mid = v.begin() + 2; // Points to v[2]
auto last = v.end() - 1;  // Points to v[4]

v.resize(3);  // Shrink to 3 elements

// ✅ 'it' is STILL VALID - element still exists
// ✅ 'mid' is STILL VALID - element still exists
// ❌ 'last' is INVALID - element was destroyed
*it;    // OK: points to '1'
*mid;   // OK: points to '3'
*last;  // UNDEFINED BEHAVIOR: element destroyed
```

**Invalidation**:
- ❌ Iterators beyond `v[n-1]` are invalidated
- ✅ Iterators within `[0, n)` remain valid
- ❌ `end()` iterator is invalidated

---

**Case 2: Growing Within Capacity (size < n <= capacity)**
```cpp
vector<int> v = {1, 2, 3};
v.reserve(10);  // capacity = 10

auto it = v.begin();
auto end = v.end();

v.resize(5);  // Grow from 3 to 5 elements

// ✅ 'it' is STILL VALID - no reallocation
// ❌ 'end' is INVALID - end() moved
*it;   // OK: points to '1'
*end;  // UNDEFINED BEHAVIOR: end changed
```

**Invalidation**:
- ✅ Iterators to existing elements remain valid
- ❌ `end()` iterator is invalidated

---

**Case 3: Growing Beyond Capacity (n > capacity)**
```cpp
vector<int> v = {1, 2, 3};  // capacity = 3

auto it = v.begin();

v.resize(10);  // Grow to 10 elements → reallocation

// ❌ 'it' is now INVALID
*it;  // UNDEFINED BEHAVIOR
```

**Invalidation**: ✅ **All** iterators invalidated due to reallocation.

---

#### clear() - Complete Invalidation
```cpp
vector<int> v = {1, 2, 3, 4, 5};

auto it = v.begin();

v.clear();  // Destroy all elements, size = 0

// ❌ 'it' is INVALID - all elements destroyed
*it;  // UNDEFINED BEHAVIOR
```

**Invalidation**: ✅ **All** iterators, pointers, and references are invalidated.

**Note**: `capacity()` is **unchanged**, so the memory buffer still exists, but all elements are destroyed.

---

### 4. Element Access (No Invalidation)

#### operator[] / at() / front() / back() / data()
```cpp
vector<int> v = {1, 2, 3};

auto it = v.begin();
int* ptr = &v[0];
int& ref = v.front();

int x = v[1];        // Just reading
v[2] = 42;           // Just writing
int y = v.at(0);     // Bounds-checked read

// ✅ 'it', 'ptr', 'ref' are ALL STILL VALID
// These operations don't modify vector structure
```

**Invalidation**: ✅ **None** - these operations don't change container structure.

---

## 🧪 Testing for Invalidation

### Safe Code Pattern
```cpp
vector<int> v = {1, 2, 3};

for (size_t i = 0; i < v.size(); ++i) {
    // Safe: use index, not iterator
    v.push_back(v[i] * 2);  // Even though push_back may invalidate
}
```

### Unsafe Code Pattern
```cpp
vector<int> v = {1, 2, 3};

for (auto it = v.begin(); it != v.end(); ++it) {
    // ❌ DANGEROUS: push_back may invalidate 'it'
    v.push_back(*it * 2);
}
```

---

## 🛡️ Best Practices

### 1. Reserve Capacity Upfront
```cpp
// ✅ GOOD: No invalidation during insertions
vector<int> v;
v.reserve(1000);

for (int i = 0; i < 1000; ++i) {
    v.push_back(i);  // No reallocation → iterators stay valid
}
```

---

### 2. Avoid Holding Iterators Across Modifying Operations
```cpp
// ❌ BAD
auto it = v.begin();
v.push_back(42);  // May invalidate 'it'
*it;              // Potential UB

// ✅ GOOD
auto it = v.begin();
// ... use 'it' ...
v.push_back(42);
it = v.begin();  // Get fresh iterator
```

---

### 3. Use Indices Instead of Iterators When Modifying
```cpp
// ✅ GOOD: Indices never invalidate
for (size_t i = 0; i < v.size(); ++i) {
    if (condition) {
        v.push_back(v[i]);  // Safe: index always valid
    }
}
```

---

### 4. Check Capacity Before Insertion
```cpp
auto it = v.begin();

if (v.size() < v.capacity()) {
    v.push_back(42);  // Won't reallocate → 'it' stays valid
} else {
    v.push_back(42);  // Will reallocate → 'it' invalid
    it = v.begin();   // Refresh iterator
}
```

---

### 5. Document Invalidation in Your Own Code
```cpp
// Adds elements to vector.
// WARNING: Invalidates all iterators if reallocation occurs!
void add_elements(vector<int>& v) {
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }
}
```

---

## 🐛 Common Bugs

### Bug 1: Iterator Invalidation in Loops
```cpp
// ❌ BUG
vector<int> v = {1, 2, 3};
for (auto it = v.begin(); it != v.end(); ++it) {
    v.push_back(*it * 2);  // Reallocates → 'it' and 'end()' invalidated!
}
// Result: Infinite loop, crashes, or wrong results

// ✅ FIX 1: Use indices
for (size_t i = 0; i < v.size(); ++i) {
    v.push_back(v[i] * 2);
}

// ✅ FIX 2: Reserve space
size_t original_size = v.size();
v.reserve(original_size * 2);
for (size_t i = 0; i < original_size; ++i) {
    v.push_back(v[i] * 2);
}

// ✅ FIX 3: Don't modify while iterating
vector<int> to_add;
for (auto x : v) {
    to_add.push_back(x * 2);
}
v.insert(v.end(), to_add.begin(), to_add.end());  // Not implemented yet!
```

---

### Bug 2: Dangling References After Reallocation
```cpp
// ❌ BUG
vector<std::string> v = {"hello"};
auto& first = v.front();  // Reference to first element

v.push_back("world");  // May reallocate

std::cout << first;  // UNDEFINED BEHAVIOR if reallocated
```

**Fix**:
```cpp
// ✅ FIX 1: Don't hold references across modifications
v.push_back("world");
std::cout << v.front();  // Fresh access

// ✅ FIX 2: Ensure capacity
v.reserve(2);
auto& first = v.front();
v.push_back("world");  // Won't reallocate
std::cout << first;  // Safe
```

---

### Bug 3: Using Invalidated end()
```cpp
// ❌ BUG
vector<int> v = {1, 2, 3};
auto end = v.end();

v.push_back(4);  // end() moved

if (end != v.end()) {  // Comparing invalidated iterator!
    // UNDEFINED BEHAVIOR
}
```

**Fix**:
```cpp
// ✅ FIX: Don't cache end()
while (v.size() < 10) {
    v.push_back(v.size());
    // Use v.end() directly each time
}
```

---

### Bug 4: Pop and Access
```cpp
// ❌ BUG
vector<int> v = {1, 2, 3};
auto last = v.back();
auto it = v.begin() + v.size() - 1;  // Iterator to last element

v.pop_back();

*it = 99;  // UNDEFINED BEHAVIOR: element destroyed
```

**Fix**:
```cpp
// ✅ FIX: Don't hold iterators to elements you're removing
int last = v.back();  // Copy value
v.pop_back();
// 'last' variable still has the value, but iterator would be invalid
```

---

## 📊 Invalidation Summary Table (Detailed)

| Operation | size() | capacity() | Iterators | Pointers & Refs | end() | Notes |
|-----------|--------|------------|-----------|-----------------|-------|-------|
| `vector()` | 0 | 0 | N/A | N/A | N/A | New object |
| `vector(n)` | n | n | N/A | N/A | N/A | New object |
| Copy ctor | Unchanged | May differ | ✅ Source valid<br>❌ Dest N/A | Same | Same | New independent object |
| Move ctor | Target gets | Target gets | ❌ Source invalid<br>❌ Dest N/A | ❌ Source invalid | ❌ Source invalid | Source becomes empty |
| Copy = | Changes | Changes | ❌ Target all invalid | ❌ Target all invalid | ❌ Invalid | New buffer |
| Move = | Changes | Changes | ❌ Both invalid | ❌ Both invalid | ❌ Invalid | Buffers swapped |
| `swap()` | Swapped | Swapped | ❌ Both invalid* | ❌ Both invalid* | ❌ Invalid | *Technically point to wrong vector |
| `reserve(n)` | Unchanged | ≥n | ⚠️ If realloc | ⚠️ If realloc | ⚠️ If realloc | Only if capacity increases |
| `shrink_to_fit()` | Unchanged | May decrease | ⚠️ If shrink | ⚠️ If shrink | ⚠️ If shrink | Non-binding request |
| `push_back()` | +1 | May increase | ⚠️ If realloc<br>❌ end() always | ⚠️ If realloc | ❌ Invalid | Reallocation at capacity |
| `emplace_back()` | +1 | May increase | ⚠️ If realloc<br>❌ end() always | ⚠️ If realloc | ❌ Invalid | Same as push_back |
| `pop_back()` | -1 | Unchanged | ❌ Last element<br>✅ Others | ❌ Last element<br>✅ Others | ❌ Invalid | Element destroyed |
| `resize(n)` | =n | May increase | ⚠️ Complex** | ⚠️ Complex** | ❌ Invalid | **See detailed notes |
| `clear()` | =0 | Unchanged | ❌ All invalid | ❌ All invalid | ❌ Invalid | Elements destroyed |
| `operator[]` | Unchanged | Unchanged | ✅ Valid | ✅ Valid | ✅ Valid | Just access |
| `at()` | Unchanged | Unchanged | ✅ Valid | ✅ Valid | ✅ Valid | Just access |
| `front()` | Unchanged | Unchanged | ✅ Valid | ✅ Valid | ✅ Valid | Just access |
| `back()` | Unchanged | Unchanged | ✅ Valid | ✅ Valid | ✅ Valid | Just access |
| `data()` | Unchanged | Unchanged | ✅ Valid | ✅ Valid | ✅ Valid | Just access |

---

## 🎓 Key Takeaways

1. **Reallocation invalidates everything** - This happens when capacity needs to increase.

2. **end() is fragile** - Almost any modification invalidates `end()`.

3. **Reserve is your friend** - Pre-allocating prevents unexpected invalidation.

4. **Indices are safer than iterators** - When modifying, indices never invalidate.

5. **Don't hold iterators across modifications** - Get fresh iterators after any operation that might reallocate.

6. **Test with sanitizers** - Tools like AddressSanitizer can detect iterator invalidation bugs.

7. **Document invalidation** - When writing functions that take vectors, document whether iterators may be invalidated.

---

*Generated for `stlish::vector` implementation - Understanding iterator lifetime and safety*
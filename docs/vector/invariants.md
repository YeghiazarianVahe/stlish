# Invariants - stlish::vector

## Overview
This document defines and explains the **class invariants** that `stlish::vector` maintains at all times. Understanding invariants is crucial for both implementing and using containers correctly.

---

## 🎯 What is an Invariant?

An **invariant** is a condition that **must always be true** about an object at specific program points:

- ✅ **After construction** (at the end of any constructor)
- ✅ **Before and after** any public method call
- ✅ **Before destruction**

Invariants can be **temporarily violated** during the execution of a method, but must be **restored** before the method returns.

---

## 📜 Core Invariants

`stlish::vector` maintains these invariants:

### I1: Size-Capacity Relationship
```cpp
size_ <= capacity_
```

**Meaning**: The number of elements never exceeds the allocated capacity.

**Why Critical**: Violating this causes buffer overruns (undefined behavior).

**Example Violation**:
```cpp
// ❌ WRONG
void push_back(const T& value) {
    data_[size_] = value;  // May write past allocated memory!
    ++size_;
}

// ✅ CORRECT
void push_back(const T& value) {
    if (size_ == capacity_) {
        reserve(grow_capacity_for(size_ + 1));
    }
    data_[size_] = value;
    ++size_;
}
```

---

### I2: Data Pointer Validity
```cpp
(capacity_ == 0) ⟹ (data_ == nullptr)
(capacity_ > 0)  ⟹ (data_ != nullptr && data points to allocated memory for capacity_ elements)
```

**Meaning**: 
- If capacity is zero, data pointer must be null
- If capacity is non-zero, data pointer must point to valid allocated memory

**Why Critical**: Prevents accessing unallocated memory.

**Example Violation**:
```cpp
// ❌ WRONG - dangling pointer
vector() : size_(0), capacity_(0), data_(new T[10]) {}  // Violates invariant

// ✅ CORRECT
vector() noexcept : size_(0), capacity_(0), data_(nullptr) {}
```

---

### I3: Element Construction Range
```cpp
∀i ∈ [0, size_): data_[i] is a constructed object of type T
∀i ∈ [size_, capacity_): data_[i] is uninitialized raw memory
```

**Meaning**: 
- First `size_` elements are properly constructed objects
- Remaining elements in allocated buffer are uninitialized

**Why Critical**: 
- Accessing `data_[i]` for `i < size_` is safe
- Accessing `data_[i]` for `i >= size_` is undefined behavior
- Destroying or assigning to uninitialized memory is undefined behavior

**Example Violation**:
```cpp
// ❌ WRONG - doesn't construct elements
vector(size_type n) : size_(n), capacity_(n), data_(allocate_raw(n)) {
    // Forgot to construct elements!
}

// User code:
int x = v[0];  // UB! Reading uninitialized memory

// ✅ CORRECT
vector(size_type n) : size_(0), capacity_(n), data_(allocate_raw(n)) {
    for (size_type i = 0; i < n; ++i) {
        ::new (data_ + i) value_type();
        ++size_;
    }
}
```

---

### I4: Destroyed State After Move
```cpp
After move construction/assignment, the moved-from vector must be in a valid state:
- data_ can be nullptr
- size_ == 0
- capacity_ == 0
- The moved-from vector can be safely destroyed or assigned to
```

**Meaning**: Moved-from vectors are left in a valid (empty) state.

**Why Critical**: C++ standard requires moved-from objects to remain destructible and assignable.

**Example Violation**:
```cpp
// ❌ WRONG - leaves moved-from object in invalid state
vector(vector&& other) noexcept 
    : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
    // Forgot to reset 'other'!
}

// Later:
vector<int> v1 = {1, 2, 3};
vector<int> v2 = std::move(v1);
// v1 destructor runs → tries to delete the memory v2 now owns → double free!

// ✅ CORRECT
vector(vector&& other) noexcept 
    : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}
```

---

### I5: Empty Vector Representation
```cpp
(size_ == 0) ⟺ empty()
```

**Meaning**: A vector is empty if and only if its size is zero.

**Why Critical**: Consistency between `empty()` and `size()`.

**Note**: An empty vector can have non-zero capacity:
```cpp
vector<int> v;
v.reserve(100);  // capacity = 100, size = 0 → still empty!
```

---

### I6: Iterator Validity
```cpp
∀ valid iterator it: data_ <= it < data_ + size_
end() iterator: data_ + size_
```

**Meaning**: Iterators point within the valid range `[data_, data_ + size_)`.

**Why Critical**: Ensures iterator operations don't access invalid memory.

**Iterator Invalidation Rules**:
- ✅ **No invalidation**: `push_back()` (if no reallocation), `pop_back()`, swap
- ⚠️ **All invalidated**: reserve (if reallocates), resize (if grows), any operation causing reallocation
- ⚠️ **Partial invalidation**: Not applicable (we don't have insert/erase)

**Example**:
```cpp
vector<int> v = {1, 2, 3};
auto it = v.begin();

v.push_back(4);  // May reallocate!
// 'it' may now be invalid (dangling pointer)

int x = *it;  // Potential UB if reallocation happened!
```

---

### I7: Capacity Growth Pattern
```cpp
If reallocation occurs during push_back(), new_capacity > old_capacity
```

**Meaning**: Capacity never decreases during growth operations, and always increases when reallocation is needed.

**Why Critical**: Ensures amortized O(1) push_back performance.

**Implementation**:
```cpp
size_type grow_capacity_for(size_type min_cap) const {
    const size_type geometric = capacity_ + (capacity_ / 2);  // 1.5x growth
    return std::max(geometric, min_cap);
}
```

**Growth Sequence** (starting from 0):
```
0 → 1 → 2 → 3 → 4 → 6 → 9 → 13 → 19 → 28 → 42 → 63 → ...
```

---

### I8: Self-Assignment Safety
```cpp
v = v;  // Must be safe (no-op or valid)
```

**Meaning**: Self-assignment must not corrupt the object.

**Why Critical**: Copy-and-swap idiom provides this automatically.

**Example**:
```cpp
vector& operator=(const vector& other) {
    vector tmp(other);  // Even if &other == this, we copy 'this'
    swap(tmp);          // Swap with the copy
    return *this;       // tmp (old 'this') is destroyed
}

// Result: self-assignment works correctly!
```

---

## 🔍 Invariant Verification

### How to Check Invariants

We can define a member function to verify all invariants:

```cpp
class vector {
    // ... 
    
#ifdef DEBUG
    bool check_invariants() const {
        // I1: Size-Capacity Relationship
        assert(size_ <= capacity_);
        
        // I2: Data Pointer Validity
        if (capacity_ == 0) {
            assert(data_ == nullptr);
        } else {
            assert(data_ != nullptr);
        }
        
        // I3: Implicit - we can't check if memory is "constructed"
        //     but we trust our implementation maintains this
        
        // I5: Empty Vector
        assert((size_ == 0) == empty());
        
        return true;
    }
#endif
};
```

**Usage**:
```cpp
void push_back(const T& value) {
    assert(check_invariants());  // Pre-condition
    
    // ... implementation ...
    
    assert(check_invariants());  // Post-condition
}
```

---

## 🛠️ Maintaining Invariants During Operations

### Example 1: push_back()

**Initial State** (invariants hold):
```cpp
size_ = 3, capacity_ = 5
data_ = [a, b, c, ?, ?]  // Elements 0-2 constructed, 3-4 uninitialized
```

**Operation**: `push_back(d)`

**Step-by-step** (invariants maintained at each step):

```cpp
void push_back(const T& value) {
    // Step 1: Check if space available
    if (size_ == capacity_) {
        // Invariant I1: Still true (3 <= 5)
        // Need to grow
        
        size_type new_cap = grow_capacity_for(capacity_ + 1);
        // Invariant I7: new_cap > capacity_ (e.g., 8)
        
        reserve(new_cap);
        // After reserve():
        //   data_ = [a, b, c, ?, ?, ?, ?, ?]
        //   size_ = 3, capacity_ = 8
        // Invariants I1, I2, I3 all hold
    }
    
    // Step 2: Construct new element
    ::new (data_ + size_) value_type(value);
    // Memory at data_[3] now holds 'd'
    // But size_ is still 3!
    // Invariant I3 VIOLATED temporarily
    
    // Step 3: Increment size
    ++size_;
    // size_ = 4, capacity_ = 8
    // Invariant I3 restored: data_[0-3] constructed, data_[4-7] uninitialized
    // All invariants hold again
}
```

**Key Insight**: Invariants can be violated **during** a method, but must be restored **before** return.

---

### Example 2: resize()

**Initial State**:
```cpp
size_ = 5, capacity_ = 5
data_ = [a, b, c, d, e]
```

**Operation**: `resize(3)`

**Step-by-step**:

```cpp
void resize(size_type n) {  // n = 3
    if (n < size_) {  // 3 < 5 → true
        // Step 1: Destroy excess elements
        destroy_range(data_ + n, data_ + size_);
        // Elements 3 and 4 destroyed (destructors called)
        // data_ = [a, b, c, ?, ?]  (where ? is now truly uninitialized)
        // size_ is still 5 → Invariant I3 VIOLATED
        
        // Step 2: Update size
        size_ = n;  // size_ = 3
        // Invariant I3 restored
        // size_ = 3, capacity_ = 5
        // data_ = [a, b, c, ?, ?]
        // Invariants hold
    }
}
```

---

### Example 3: Move Constructor

**Source State**:
```cpp
other.size_ = 5, other.capacity_ = 10
other.data_ = [a, b, c, d, e, ?, ?, ?, ?, ?]
```

**Operation**: `vector v(std::move(other))`

**Step-by-step**:

```cpp
vector(vector&& other) noexcept 
    : size_(other.size_),      // this->size_ = 5
      capacity_(other.capacity_),  // this->capacity_ = 10
      data_(other.data_) {     // this->data_ = pointer to [a,b,c,d,e,...]
      
    // At this point:
    //   'this' has all resources
    //   'other' still has pointers to same memory
    // If we stop here, invariant I4 is VIOLATED (other is in invalid state)
    
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
    
    // Now 'other' is in valid (empty) state
    // Invariant I4 satisfied
}
```

**Result**:
```cpp
// After move:
this->size_ = 5, this->capacity_ = 10, this->data_ = [a, b, c, d, e, ...]
other.size_ = 0, other.capacity_ = 0, other.data_ = nullptr

// Both objects satisfy all invariants
```

---

## ⚠️ Common Invariant Violations

### Violation 1: Incrementing Size Before Construction

```cpp
// ❌ WRONG
void push_back(const T& value) {
    if (size_ == capacity_) reserve(grow_capacity_for(size_ + 1));
    
    ++size_;  // SIZE INCREMENTED FIRST
    ::new (data_ + size_ - 1) value_type(value);  // Then constructed
}

// Problem: If construction throws, size_ is wrong!
// Invariant I3 violated: size_ says element exists, but it's not constructed
```

**Fix**: Increment size **after** construction:
```cpp
// ✅ CORRECT
void push_back(const T& value) {
    if (size_ == capacity_) reserve(grow_capacity_for(size_ + 1));
    
    ::new (data_ + size_) value_type(value);  // Construct first
    ++size_;  // Then increment
}
```

---

### Violation 2: Not Resetting Moved-From Object

```cpp
// ❌ WRONG
vector(vector&& other) noexcept 
    : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
    // Forgot to reset 'other'
}

// Result: Double ownership → double delete → crash
```

**Fix**: Always reset moved-from object:
```cpp
// ✅ CORRECT
vector(vector&& other) noexcept 
    : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}
```

---

### Violation 3: Allocating Without Setting Capacity

```cpp
// ❌ WRONG
vector(size_type n) : size_(0), capacity_(0), data_(nullptr) {
    data_ = allocate_raw(n);  // Allocate memory
    // capacity_ is still 0, but data_ is non-null
    // Invariant I2 VIOLATED
    
    for (size_type i = 0; i < n; ++i) {
        ::new (data_ + i) value_type();
        ++size_;
    }
}

// ✅ CORRECT
vector(size_type n) : size_(0), capacity_(n), data_(allocate_raw(n)) {
    // capacity_ set in initializer list
    for (size_type i = 0; i < n; ++i) {
        ::new (data_ + i) value_type();
        ++size_;
    }
}
```

---

### Violation 4: Destroying Uninitialized Memory

```cpp
// ❌ WRONG
void clear() noexcept {
    for (size_type i = 0; i < capacity_; ++i) {  // WRONG: using capacity_
        data_[i].~value_type();  // Destroys uninitialized memory!
    }
    size_ = 0;
}

// ✅ CORRECT
void clear() noexcept {
    for (size_type i = 0; i < size_; ++i) {  // CORRECT: using size_
        data_[i].~value_type();
    }
    size_ = 0;
}
```

---

## 📐 Invariants and Class Design

### Designing with Invariants in Mind

1. **Define invariants first**, before writing code
2. **Document invariants** in comments
3. **Use assertions** in debug builds to check invariants
4. **Consider invariants** when handling exceptions
5. **Test** that invariants hold after each operation

---

### Example: Designing reserve()

**Invariants to maintain**:
- I1: `size_ <= capacity_`
- I2: `data_` pointer validity
- I3: Element construction range

**Design**:
```cpp
void reserve(size_type new_cap) {
    // Pre-condition: invariants hold
    assert(check_invariants());
    
    // If new_cap <= capacity_, do nothing
    // This maintains all invariants trivially
    if (new_cap <= capacity_) {
        return;
    }
    
    // Allocate new buffer
    pointer new_data = allocate_raw(new_cap);
    // Temporary state: old buffer still valid, new buffer allocated
    // Invariants about 'this' still hold
    
    // Move elements to new buffer
    try {
        uninitialized_move(new_data, data_, data_ + size_);
        // Elements now in both buffers (old and new)
        // Still maintaining invariants
    } catch (...) {
        // Move failed: deallocate new buffer, keep old one
        deallocate_raw(new_data, new_cap);
        // Invariants still hold (old state unchanged)
        throw;
    }
    
    // Success: destroy old elements and swap buffers
    destroy_range(data_, data_ + size_);
    deallocate_raw(data_, capacity_);
    data_ = new_data;
    capacity_ = new_cap;
    // size_ unchanged
    
    // Post-condition: invariants hold
    // I1: size_ <= new_cap (since size_ unchanged and new_cap > old capacity_ >= size_)
    // I2: data_ points to newly allocated buffer for capacity_ elements
    // I3: First size_ elements are constructed (we moved them)
    assert(check_invariants());
}
```

---

## 🎯 Benefits of Maintaining Invariants

### 1. Correctness
If invariants always hold, the class is in a valid state.

### 2. Simplifies Reasoning
When calling a method, you know the pre-conditions (invariants) are met.

### 3. Eases Debugging
When something goes wrong, check which invariant is violated.

### 4. Enables Optimization
Knowing invariants allows skipping unnecessary checks.

**Example**:
```cpp
T& operator[](size_type idx) noexcept {
    // We can skip bounds check because:
    // - Invariant I1 guarantees size_ <= capacity_
    // - Invariant I3 guarantees data_[i] is constructed for i < size_
    // User's responsibility to ensure idx < size_
    return data_[idx];
}
```

### 5. Facilitates Formal Verification
Invariants can be expressed in formal specification languages.

---

## 📊 Invariant Summary Table

| Invariant | Formula | Checked By |
|-----------|---------|------------|
| **I1** Size ≤ Capacity | `size_ <= capacity_` | All methods |
| **I2** Pointer Validity | `(capacity_ == 0) ⟹ (data_ == nullptr)` | Constructors, reserve, swap |
| **I3** Construction Range | `data_[i]` constructed ⟺ `i < size_` | Constructors, modifiers |
| **I4** Moved-From State | Moved-from object is valid | Move operations |
| **I5** Empty Iff Size Zero | `size_ == 0 ⟺ empty()` | All methods |
| **I6** Iterator Validity | `data_ <= it < data_ + size_` | All methods |
| **I7** Capacity Growth | Reallocation increases capacity | reserve, push_back |
| **I8** Self-Assignment | `v = v` is safe | Assignment operators |

---

## 🧪 Testing Invariants

### Unit Test Example

```cpp
TEST(VectorTest, InvariantsAfterConstruction) {
    vector<int> v;
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_EQ(v.data(), nullptr);
    EXPECT_TRUE(v.empty());
}

TEST(VectorTest, InvariantsAfterReserve) {
    vector<int> v;
    v.reserve(10);
    
    // I1: size <= capacity
    EXPECT_LE(v.size(), v.capacity());
    
    // I2: capacity > 0 ⟹ data != nullptr
    EXPECT_NE(v.data(), nullptr);
    EXPECT_EQ(v.capacity(), 10);
    
    // I5: empty iff size == 0
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
}

TEST(VectorTest, InvariantsAfterPushBack) {
    vector<int> v;
    for (int i = 0; i < 100; ++i) {
        v.push_back(i);
        
        // Check invariants after each push_back
        EXPECT_LE(v.size(), v.capacity());  // I1
        EXPECT_NE(v.data(), nullptr);        // I2
        EXPECT_EQ(v.size(), i + 1);          // Correct size
        EXPECT_FALSE(v.empty());             // I5
    }
}

TEST(VectorTest, InvariantsAfterMove) {
    vector<int> v1 = {1, 2, 3, 4, 5};
    size_t old_size = v1.size();
    
    vector<int> v2 = std::move(v1);
    
    // v2 has the data
    EXPECT_EQ(v2.size(), old_size);
    EXPECT_LE(v2.size(), v2.capacity());
    
    // v1 is in valid (empty) state
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 0);
    EXPECT_EQ(v1.data(), nullptr);
    EXPECT_TRUE(v1.empty());
    
    // v1 can be destroyed or assigned to
    v1 = {10, 20, 30};  // Should work!
    EXPECT_EQ(v1.size(), 3);
}
```

---

## 🎓 Key Takeaways

1. **Invariants define correctness** - if all invariants hold, the class is in a valid state.

2. **Invariants can be temporarily violated** during method execution, but must be restored before returning.

3. **Exception safety and invariants are linked** - exceptions must not leave the object with violated invariants.

4. **Use assertions in debug builds** to catch invariant violations early.

5. **Design with invariants in mind** - consider invariants when implementing each method.

6. **Document invariants** - make them explicit for maintainers and users.

7. **Test invariants** - unit tests should verify invariants hold after operations.

---

*Generated for `stlish::vector` implementation - Understanding class invariants for correctness*
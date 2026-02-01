# Exception Safety - stlish::vector

## Overview
This document analyzes the exception safety guarantees provided by `stlish::vector`. Understanding exception safety is crucial for writing robust C++ code that behaves correctly even when operations fail.

---

## 📚 Exception Safety Guarantees

C++ defines **four levels** of exception safety:

### 1. No-throw Guarantee (Strongest)
- ✅ **Operation never throws exceptions**
- ✅ **Always succeeds**
- Marked with `noexcept` in modern C++

### 2. Strong Exception Safety
- ✅ **Operation either succeeds completely or has no effect**
- ✅ **"Commit or rollback" semantics**
- If an exception is thrown, the state is unchanged (rolled back)

### 3. Basic Exception Safety
- ✅ **No resource leaks occur**
- ✅ **Object remains in valid state**
- ⚠️ Object state may have changed
- Object can still be destroyed or assigned to

### 4. No Exception Safety (Weakest)
- ❌ **Resource leaks may occur**
- ❌ **Object may be in invalid state**
- ❌ **Undefined behavior possible**

---

## 🎯 stlish::vector Exception Guarantees

### Summary Table

| Operation | Guarantee | Can Throw? | Details |
|-----------|-----------|------------|---------|
| Default constructor | No-throw | ❌ | `noexcept` |
| Fill constructors | Basic | ✅ | T's constructor, bad_alloc |
| Copy constructor | Strong | ✅ | T's copy constructor, bad_alloc |
| Move constructor | No-throw | ❌ | `noexcept` |
| Range constructor | Basic | ✅ | T's constructor, bad_alloc |
| Initializer list constructor | Basic | ✅ | T's constructor, bad_alloc |
| Destructor | No-throw | ❌ | Implicit `noexcept` |
| Copy assignment | Strong | ✅ | Uses copy-and-swap |
| Move assignment | No-throw | ❌ | `noexcept` |
| Swap | No-throw | ❌ | `noexcept` |
| size(), capacity(), empty() | No-throw | ❌ | `noexcept` |
| max_size() | No-throw | ❌ | `noexcept` |
| reserve() | Strong | ✅ | bad_alloc |
| shrink_to_fit() | Strong | ✅ | bad_alloc, T's move/copy |
| operator[] | No-throw | ❌ | `noexcept` |
| at() | Strong | ✅ | out_of_range |
| front(), back() | No-throw | ❌ | `noexcept` |
| data() | No-throw | ❌ | `noexcept` |
| begin(), end() | No-throw | ❌ | `noexcept` |
| clear() | No-throw | ❌ | `noexcept` |
| push_back() | Strong | ✅ | T's constructor, bad_alloc |
| emplace_back() | Strong | ✅ | T's constructor, bad_alloc |
| pop_back() | No-throw | ❌ | `noexcept` |
| resize() | Basic | ✅ | T's constructor, bad_alloc |

---

## 🔍 Detailed Analysis by Category

### 1️⃣ Constructors

#### Default Constructor
```cpp
vector() noexcept;
```

**Guarantee**: **No-throw**

**Why?** 
- Only initializes three member variables to zero/null
- No memory allocation
- No object construction

```cpp
// Implementation
vector() noexcept 
    : size_(0), capacity_(0), data_(nullptr) {}
```

✅ **Cannot fail** - mathematically impossible to throw

---

#### Fill Constructor (n elements)
```cpp
explicit vector(size_type n);
```

**Guarantee**: **Basic Exception Safety**

**Possible Exceptions**:
1. `std::bad_alloc` - if memory allocation fails
2. Exception from `T`'s default constructor

**Exception Behavior**:
```cpp
vector<Widget> v(100);  // What if Widget() throws?
```

If an exception is thrown:
- ✅ Memory is deallocated (no leak)
- ✅ Already-constructed objects are destroyed
- ❌ Vector object is **not** created (constructor failed)

**Implementation Pattern**:
```cpp
explicit vector(size_type n) 
    : size_(0), 
      capacity_(n),
      data_(allocate_raw(n))  // May throw bad_alloc
{
    try {
        for (size_type i = 0; i < n; ++i) {
            ::new (data_ + i) value_type();  // May throw
            ++size_;  // Track successful constructions
        }
    } catch (...) {
        // Cleanup: destroy constructed elements
        destroy_range(data_, data_ + size_);
        deallocate_raw(data_, capacity_);
        throw;  // Re-throw the exception
    }
}
```

**Why Basic and not Strong?**
- There's no "previous state" to restore (object is being constructed)
- The guarantee is: if it fails, no resources leak

---

#### Copy Constructor
```cpp
vector(const vector& other);
```

**Guarantee**: **Strong Exception Safety**

**Possible Exceptions**:
1. `std::bad_alloc` - memory allocation fails
2. Exception from `T`'s copy constructor

**Exception Behavior**:
```cpp
vector<std::string> v1 = {"hello", "world"};
vector<std::string> v2 = v1;  // What if copying throws?
```

If an exception is thrown:
- ✅ `v2` is **not** created (construction failed)
- ✅ `v1` is **unchanged**
- ✅ No resource leaks

**Implementation ensures cleanup**:
```cpp
vector(const vector& other) 
    : size_(0),
      capacity_(other.capacity_),
      data_(allocate_raw(other.capacity_))  // May throw bad_alloc
{
    try {
        for (size_type i = 0; i < other.size_; ++i) {
            ::new (data_ + i) value_type(other.data_[i]);  // May throw
            ++size_;
        }
    } catch (...) {
        destroy_range(data_, data_ + size_);
        deallocate_raw(data_, capacity_);
        throw;
    }
}
```

---

#### Move Constructor
```cpp
vector(vector&& other) noexcept;
```

**Guarantee**: **No-throw**

**Why `noexcept`?**
- Only transfers ownership of pointers
- No memory allocation
- No object construction
- Just copying integers and pointers

```cpp
// Implementation - cannot fail
vector(vector&& other) noexcept
    : size_(other.size_),
      capacity_(other.capacity_),
      data_(other.data_) 
{
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}
```

**Critical for performance**: `noexcept` move constructors enable optimizations in STL algorithms.

---

### 2️⃣ Assignment Operators

#### Copy Assignment
```cpp
vector& operator=(const vector& other);
```

**Guarantee**: **Strong Exception Safety**

**Implementation Strategy**: **Copy-and-Swap Idiom**

```cpp
vector& operator=(const vector& other) {
    vector tmp(other);  // (1) May throw - but current object untouched
    swap(tmp);          // (2) noexcept - always succeeds
    return *this;       // (3) tmp's destructor cleans up old data
}
```

**Why Strong?**
1. **If copy construction fails**: Original vector is unchanged
2. **Swap never fails**: `noexcept` operation
3. **Result**: Either completely succeeds or has no effect

**Example**:
```cpp
vector<std::string> v1 = {"hello"};
vector<std::string> v2 = {"world"};

v1 = v2;  // If this throws during copying, v1 is unchanged!
```

---

#### Move Assignment
```cpp
vector& operator=(vector&& other) noexcept;
```

**Guarantee**: **No-throw**

**Why `noexcept`?**
- Current elements are destroyed (destructors should not throw)
- Pointers are swapped (trivial operation)

```cpp
vector& operator=(vector&& other) noexcept {
    if (this != &other) {
        clear();                    // Destroy current elements (noexcept)
        deallocate_raw(data_, capacity_);  // Deallocate (noexcept)
        
        // Steal resources
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        
        // Reset source
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}
```

**Assumption**: `T`'s destructor does not throw (C++ requirement)

---

### 3️⃣ Capacity Operations

#### reserve()
```cpp
void reserve(size_type new_cap);
```

**Guarantee**: **Strong Exception Safety**

**Possible Exceptions**:
1. `std::bad_alloc` - allocation fails
2. Exception from `T`'s move/copy constructor

**Exception Behavior**:
```cpp
vector<Widget> v = {w1, w2, w3};
v.reserve(100);  // What if allocation fails?
```

If an exception is thrown:
- ✅ Vector remains **unchanged** (old capacity, old elements)
- ✅ No resource leaks

**Implementation Strategy**:
```cpp
void reserve(size_type new_cap) {
    if (new_cap <= capacity_) {
        return;  // No-op, cannot fail
    }
    
    // Allocate new buffer (may throw bad_alloc)
    pointer new_data = allocate_raw(new_cap);
    
    try {
        // Move/copy elements (may throw)
        uninitialized_move(new_data, data_, data_ + size_);
    } catch (...) {
        // Exception during move: deallocate new buffer, keep old data
        deallocate_raw(new_data, new_cap);
        throw;  // Vector state unchanged!
    }
    
    // Success: destroy old elements and swap buffers
    destroy_range(data_, data_ + size_);
    deallocate_raw(data_, capacity_);
    data_ = new_data;
    capacity_ = new_cap;
}
```

**Key Insight**: Old buffer is kept until new buffer is successfully populated.

---

#### shrink_to_fit()
```cpp
void shrink_to_fit();
```

**Guarantee**: **Strong Exception Safety**

**Why?** Uses same strategy as `reserve()` - allocate new buffer, copy/move, then swap.

**Exception Behavior**:
```cpp
vector<int> v = {1, 2, 3};
v.reserve(1000);  // capacity = 1000
v.shrink_to_fit();  // What if reallocation fails?
```

If an exception is thrown:
- ✅ Vector keeps old (larger) capacity
- ✅ All elements remain intact

**Note**: This is a **best-effort** operation. The standard allows `shrink_to_fit()` to do nothing even if it succeeds!

---

### 4️⃣ Element Access

#### operator[]
```cpp
reference operator[](size_type idx) noexcept;
```

**Guarantee**: **No-throw**

**Why?** 
- Simple pointer arithmetic
- **No bounds checking** (undefined behavior if out of range)

```cpp
reference operator[](size_type idx) noexcept {
    return data_[idx];  // Just a pointer dereference
}
```

⚠️ **UB Warning**: Accessing out-of-bounds is **undefined behavior**, not an exception.

---

#### at()
```cpp
reference at(size_type idx);
```

**Guarantee**: **Strong Exception Safety**

**Throws**: `std::out_of_range` if `idx >= size()`

**Exception Behavior**:
```cpp
vector<int> v = {1, 2, 3};
try {
    int x = v.at(10);  // Throws out_of_range
} catch (const std::out_of_range& e) {
    // v is completely unchanged
}
```

**Implementation**:
```cpp
reference at(size_type idx) {
    if (idx >= size_) {
        throw std::out_of_range("vector::at");  // Strong guarantee
    }
    return data_[idx];
}
```

**Why Strong?** The check happens **before** any state change.

---

#### front() / back()
```cpp
reference front() noexcept;
reference back() noexcept;
```

**Guarantee**: **No-throw**

**Why?** Simple pointer access, no bounds checking.

⚠️ **UB Warning**: Calling on empty vector is **undefined behavior**.

---

### 5️⃣ Modifiers

#### clear()
```cpp
void clear() noexcept;
```

**Guarantee**: **No-throw**

**Why?**
- Destructors should not throw (C++ requirement)
- Only modifies `size_`, no allocation

```cpp
void clear() noexcept {
    destroy_range(data_, data_ + size_);  // Destructors must not throw
    size_ = 0;
}
```

**C++ Rule**: Destructors are implicitly `noexcept`. If a destructor throws, `std::terminate()` is called.

---

#### push_back() / emplace_back()
```cpp
void push_back(const_reference value);
template <class... Args>
reference emplace_back(Args&&... args);
```

**Guarantee**: **Strong Exception Safety**

**Possible Exceptions**:
1. `std::bad_alloc` - if reallocation needed
2. Exception from `T`'s constructor

**Exception Behavior**:
```cpp
vector<std::string> v = {"hello"};
v.push_back("world");  // What if allocation or construction fails?
```

If an exception is thrown:
- ✅ Vector is **unchanged** (old size, old elements)
- ✅ No partially-constructed element
- ✅ No resource leaks

**Implementation Strategy**:
```cpp
template <class... Args>
reference emplace_back(Args&&... args) {
    if (size_ == capacity_) {
        // Need to reallocate
        size_type new_cap = grow_capacity_for(capacity_ + 1);
        
        // Strong guarantee from reserve()
        reserve(new_cap);  // If this throws, vector unchanged
    }
    
    // Construct new element (may throw)
    pointer p = ::new (static_cast<void*>(data_ + size_)) 
        value_type(std::forward<Args>(args)...);
    
    ++size_;  // Only increment after successful construction
    return *p;
}
```

**Key Insight**: 
- If reallocation fails → vector unchanged
- If construction fails after reallocation → new buffer has space, but element not counted in size

**What if construction throws AFTER reallocation?**
```cpp
// After successful reserve(), we have:
// - New buffer with all old elements moved
// - Old buffer destroyed
// - size_ and capacity_ updated

// Now construct new element:
::new (data_ + size_) value_type(args...);  // If THIS throws?
```

**Problem**: We've already committed to the new buffer!

**Solution**: In this implementation, the vector is **still valid**:
- ✅ All old elements are intact
- ✅ Memory is not leaked
- ✅ size_ is not incremented
- ❌ **Basic** (not Strong) guarantee if construction fails after reallocation

**True behavior**: 
- **Strong** if no reallocation needed
- **Basic** if reallocation occurs and construction fails

**To achieve Strong in all cases**, we'd need:
```cpp
// Advanced implementation (not shown in vector.hpp)
template <class... Args>
reference emplace_back(Args&&... args) {
    if (size_ == capacity_) {
        size_type new_cap = grow_capacity_for(capacity_ + 1);
        
        // Allocate new buffer
        pointer new_data = allocate_raw(new_cap);
        
        try {
            // Construct NEW element FIRST in new buffer
            ::new (new_data + size_) value_type(std::forward<Args>(args)...);
            
            // Then move old elements
            uninitialized_move(new_data, data_, data_ + size_);
            
        } catch (...) {
            // Failed: destroy and cleanup new buffer, keep old buffer
            if (/* new element was constructed */) {
                (new_data + size_)->~value_type();
            }
            deallocate_raw(new_data, new_cap);
            throw;  // Vector completely unchanged!
        }
        
        // Success: destroy old buffer
        destroy_range(data_, data_ + size_);
        deallocate_raw(data_, capacity_);
        data_ = new_data;
        capacity_ = new_cap;
    } else {
        ::new (data_ + size_) value_type(std::forward<Args>(args)...);
    }
    
    ++size_;
    return data_[size_ - 1];
}
```

---

#### pop_back()
```cpp
void pop_back() noexcept;
```

**Guarantee**: **No-throw**

**Why?**
- Only calls destructor (must not throw)
- Decrements size

```cpp
void pop_back() noexcept {
    if (size_ == 0) return;
    --size_;
    data_[size_].~value_type();  // Destructor must not throw
}
```

---

#### resize()
```cpp
void resize(size_type n);
void resize(size_type n, const_reference value);
```

**Guarantee**: **Basic Exception Safety**

**Possible Exceptions**:
1. `std::bad_alloc` - if capacity increase needed
2. Exception from `T`'s constructor

**Exception Behavior**:
```cpp
vector<Widget> v = {w1, w2, w3};
v.resize(100);  // What if Widget() throws during construction?
```

If an exception is thrown:
- ✅ No resource leaks
- ✅ Vector remains valid
- ⚠️ Size may have changed (some new elements constructed)
- ⚠️ **Not strong guarantee**

**Example**:
```cpp
vector<Widget> v;  // size = 0
v.resize(10);      // Suppose construction fails at element #5
// Result: v.size() = 5, not 0 or 10!
```

**Implementation shows Basic guarantee**:
```cpp
void resize(size_type n, const_reference value) {
    if (n < size_) {
        // Shrinking: always succeeds (destructors don't throw)
        destroy_range(data_ + n, data_ + size_);
        size_ = n;
    } else if (n > size_) {
        reserve(n);  // May throw
        
        for (size_type i = size_; i < n; ++i) {
            ::new (data_ + i) value_type(value);  // May throw!
            ++size_;  // Increment after each successful construction
        }
        // If construction throws, size_ reflects actual count
    }
}
```

**Why not Strong?** 
- After successful `reserve()`, we start constructing elements
- If element #5 throws, we've already modified the vector (elements 0-4 added)
- Can't rollback without potentially throwing exceptions during destruction

---

## 🛡️ Exception Safety Design Patterns

### 1. Copy-and-Swap Idiom
**Used in**: Copy assignment operator

**Pattern**:
```cpp
T& operator=(const T& other) {
    T tmp(other);  // May throw, but 'this' untouched
    swap(tmp);     // noexcept
    return *this;  // tmp's destructor cleans up old data
}
```

**Benefits**:
- ✅ Strong exception safety
- ✅ Self-assignment safe
- ✅ Simple and elegant

---

### 2. Commit-or-Rollback
**Used in**: `reserve()`, `shrink_to_fit()`

**Pattern**:
```cpp
void reserve(size_type new_cap) {
    // Allocate new resources
    pointer new_data = allocate_raw(new_cap);  // May throw
    
    try {
        // Populate new resources
        uninitialized_move(new_data, data_, data_ + size_);  // May throw
    } catch (...) {
        // Rollback: cleanup new resources, keep old
        deallocate_raw(new_data, new_cap);
        throw;
    }
    
    // Commit: destroy old resources, swap to new
    destroy_range(data_, data_ + size_);
    deallocate_raw(data_, capacity_);
    data_ = new_data;
    capacity_ = new_cap;
}
```

**Benefits**:
- ✅ Strong exception safety
- ✅ Old state preserved until success

---

### 3. Incremental State Updates
**Used in**: Constructors, `resize()`

**Pattern**:
```cpp
vector(size_type n) : size_(0), capacity_(n), data_(allocate_raw(n)) {
    try {
        for (size_type i = 0; i < n; ++i) {
            ::new (data_ + i) value_type();
            ++size_;  // Only increment after successful construction
        }
    } catch (...) {
        // size_ reflects actual constructed count
        destroy_range(data_, data_ + size_);
        deallocate_raw(data_, capacity_);
        throw;
    }
}
```

**Benefits**:
- ✅ No resource leaks
- ✅ State always reflects reality
- ⚠️ Only basic guarantee (not strong)

---

## ⚠️ Common Pitfalls

### 1. Assuming `push_back()` is Always Strong
**Reality**: Basic guarantee if reallocation happens and construction throws.

**Example**:
```cpp
class Thrower {
public:
    Thrower() {}
    Thrower(const Thrower&) { throw std::runtime_error("copy failed"); }
};

vector<Thrower> v;
v.reserve(10);  // Capacity = 10
for (int i = 0; i < 10; ++i) {
    v.emplace_back();  // OK, no reallocation
}

Thrower t;
v.push_back(t);  // Reallocation needed, copy constructor throws
// What happens? Vector is valid but state changed!
```

---

### 2. Not Using `noexcept` for Move Operations
**Problem**: Without `noexcept`, `std::vector` will use copy instead of move during reallocation.

**Example**:
```cpp
class Widget {
public:
    Widget(Widget&&) { /* move, may throw */ }
};

std::vector<Widget> v;
v.push_back(w);  // std::vector uses COPY (slower) because move isn't noexcept!
```

**Fix**:
```cpp
class Widget {
public:
    Widget(Widget&&) noexcept { /* move, guaranteed not to throw */ }
};
```

---

### 3. Forgetting Destructors Can't Throw
**Problem**: If a destructor throws during stack unwinding, `std::terminate()` is called.

**Example**:
```cpp
class Bad {
public:
    ~Bad() { throw std::runtime_error("oops"); }  // DON'T DO THIS!
};

vector<Bad> v = {b1, b2, b3};
v.clear();  // Destructor throws → std::terminate() → program aborts!
```

---

## ✅ Best Practices for Users

### 1. Design Exception-Safe Types
```cpp
// Good
class Widget {
public:
    Widget() { /* may throw */ }
    Widget(Widget&&) noexcept { /* must not throw */ }
    ~Widget() noexcept { /* must not throw */ }
};
```

### 2. Use `reserve()` When Size is Known
```cpp
vector<int> v;
v.reserve(1000);  // Pre-allocate once

for (int i = 0; i < 1000; ++i) {
    v.push_back(i);  // No reallocations!
}
```

### 3. Catch Exceptions Appropriately
```cpp
try {
    vector<Widget> v(100);
} catch (const std::bad_alloc&) {
    // Handle allocation failure
} catch (const std::exception&) {
    // Handle Widget construction failure
}
```

### 4. Be Aware of Exception Points
```cpp
vector<std::string> v;

// These can throw:
v.push_back("hello");      // allocation or string construction
v.resize(100);             // allocation or string construction
v.at(0);                   // out_of_range
v.reserve(1000);           // allocation

// These cannot throw:
v.size();
v[0];
v.front();
v.pop_back();
v.clear();
```

---

## 📊 Exception Safety Summary

| Guarantee Level | Operations |
|-----------------|------------|
| **No-throw** | Default ctor, Move ctor, Move assignment, Destructor, Swap, size(), capacity(), empty(), max_size(), operator[], front(), back(), data(), iterators, clear(), pop_back() |
| **Strong** | Copy ctor, Copy assignment, at(), reserve(), shrink_to_fit(), push_back()/emplace_back() (usually) |
| **Basic** | Fill ctors, Range ctor, Initializer list ctor, resize(), push_back()/emplace_back() (if reallocation + throw) |

---

## 🎓 Key Takeaways

1. **`noexcept` is Critical**: Marking move operations as `noexcept` enables optimizations and strong guarantees in STL algorithms.

2. **Strong Guarantee is Hard**: Operations that modify state (like `resize()`) often can only provide basic guarantee.

3. **Copy-and-Swap is Your Friend**: Provides strong guarantee for assignment operators.

4. **Destructors Must Not Throw**: This is a **C++ requirement**, not just good practice.

5. **Test Exception Paths**: Use fault injection to test how your code handles allocation failures and constructor exceptions.

6. **Reserve is Optimization AND Safety**: Pre-allocating prevents unexpected exceptions during `push_back()`.

---

*Generated for `stlish::vector` implementation - Writing exception-safe code*
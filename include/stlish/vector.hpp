#ifndef STLISH_VECTOR_HPP
#define STLISH_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace stlish {

/**
 * @brief A dynamic array implementation similar to std::vector
 * @tparam T The type of elements stored in the vector
 */
template <class T>
class vector {
public:
    // ========================================================================
    // Member Types
    // ========================================================================
    
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;

public:
    // ========================================================================
    // Constructors and Destructor
    // ========================================================================
    
    /**
     * @brief Default constructor - creates an empty vector
     */
    vector() noexcept;
    
    /**
     * @brief Constructs a vector with n default-constructed elements
     * @param n Number of elements to create
     */
    explicit vector(size_type n);
    
    /**
     * @brief Constructs a vector with n copies of value
     * @param n Number of elements to create
     * @param value Value to copy
     */
    vector(size_type n, const_reference value);
    
    /**
     * @brief Copy constructor
     * @param other Vector to copy from
     */
    vector(const vector& other);
    
    /**
     * @brief Move constructor
     * @param other Vector to move from
     */
    vector(vector&& other) noexcept;
    
    /**
     * @brief Range constructor - constructs from iterator range
     * @tparam InputIt Iterator type
     * @param first Beginning of range
     * @param last End of range
     */
    template <class InputIt>
        requires(!std::is_integral_v<InputIt> && std::forward_iterator<InputIt>)
    vector(InputIt first, InputIt last);
    
    /**
     * @brief Initializer list constructor
     * @param init Initializer list
     */
    vector(std::initializer_list<value_type> init);

    /**
     * @brief Destructor - destroys all elements and deallocates memory
     */
    ~vector();

    // ========================================================================
    // Assignment Operators (Rule of Five)
    // ========================================================================
    
    /**
     * @brief Copy assignment operator
     */
    vector& operator=(const vector& other);
    
    /**
     * @brief Move assignment operator
     */
    vector& operator=(vector&& other) noexcept;
    
    /**
     * @brief Initializer list assignment operator
     */
    vector& operator=(std::initializer_list<value_type> init);
    
    /**
     * @brief Swap contents with another vector
     */
    void swap(vector& other) noexcept;

    // ========================================================================
    // Capacity
    // ========================================================================
    
    /**
     * @brief Returns the number of elements
     */
    [[nodiscard]] size_type size() const noexcept;
    
    /**
     * @brief Returns the capacity (allocated storage)
     */
    [[nodiscard]] size_type capacity() const noexcept;
    
    /**
     * @brief Checks if the vector is empty
     */
    [[nodiscard]] bool empty() const noexcept;
    
    /**
     * @brief Returns the maximum possible number of elements
     */
    [[nodiscard]] size_type max_size() const noexcept;
    
    /**
     * @brief Reserves storage for at least new_cap elements
     * @param new_cap New capacity
     */
    void reserve(size_type new_cap);
    
    /**
     * @brief Requests the removal of unused capacity
     */
    void shrink_to_fit();

    // ========================================================================
    // Element Access
    // ========================================================================
    
    /**
     * @brief Direct access to underlying array
     */
    [[nodiscard]] pointer data() noexcept;
    
    /**
     * @brief Direct access to underlying array (const)
     */
    [[nodiscard]] const_pointer data() const noexcept;
    
    /**
     * @brief Access element at index (no bounds checking)
     */
    reference operator[](size_type idx) noexcept;
    
    /**
     * @brief Access element at index (no bounds checking, const)
     */
    const_reference operator[](size_type idx) const noexcept;
    
    /**
     * @brief Access element at index (with bounds checking)
     * @throws std::out_of_range if idx >= size()
     */
    reference at(size_type idx);
    
    /**
     * @brief Access element at index (with bounds checking, const)
     * @throws std::out_of_range if idx >= size()
     */
    const_reference at(size_type idx) const;
    
    /**
     * @brief Access first element
     */
    reference front() noexcept;
    
    /**
     * @brief Access first element (const)
     */
    const_reference front() const noexcept;
    
    /**
     * @brief Access last element
     */
    reference back() noexcept;
    
    /**
     * @brief Access last element (const)
     */
    const_reference back() const noexcept;

    // ========================================================================
    // Iterators
    // ========================================================================
    
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    // ========================================================================
    // Modifiers
    // ========================================================================
    
    /**
     * @brief Removes all elements
     */
    void clear() noexcept;
    
    /**
     * @brief Adds an element to the end (copy)
     */
    void push_back(const_reference value);
    
    /**
     * @brief Adds an element to the end (move)
     */
    void push_back(value_type&& value);
    
    /**
     * @brief Constructs an element in-place at the end
     * @return Reference to the newly constructed element
     */
    template <class... Args>
    reference emplace_back(Args&&... args);
    
    /**
     * @brief Removes the last element
     */
    void pop_back() noexcept;
    
    /**
     * @brief Resizes the container to contain n elements
     */
    void resize(size_type n);
    
    /**
     * @brief Resizes the container to contain n elements with value
     */
    void resize(size_type n, const_reference value);

private:
    // ========================================================================
    // Member Variables
    // ========================================================================
    
    size_type size_;
    size_type capacity_;
    pointer data_;

    // ========================================================================
    // Helper Functions
    // ========================================================================
    
    /**
     * @brief Allocates raw memory for cap elements
     */
    static pointer allocate_raw(size_type cap);
    
    /**
     * @brief Deallocates raw memory
     */
    static void deallocate_raw(pointer ptr, size_type cap) noexcept;
    
    /**
     * @brief Destroys elements in range [first, last)
     */
    static void destroy_range(pointer first, pointer last) noexcept;
    
    /**
     * @brief Copy-constructs elements from [first, last) to dst
     * @return Pointer one past the last constructed element
     */
    template <class InputIt>
    static pointer uninitialized_copy(pointer dst, InputIt first, InputIt last);
    
    /**
     * @brief Move-constructs elements from [first, last) to dst
     * @return Pointer one past the last constructed element
     */
    template <class InputIt>
    static pointer uninitialized_move(pointer dst, InputIt first, InputIt last);
    
    /**
     * @brief Reallocates storage to new_cap
     */
    void reallocate(size_type new_cap);
    
    /**
     * @brief Calculates new capacity for growth
     */
    size_type grow_capacity_for(size_type min_cap) const noexcept;
    
    /**
     * @brief Checks if index is valid, throws if not
     */
    static void check_index(size_type idx, size_type size);
};

// ============================================================================
// Implementation - Helper Functions
// ============================================================================

template <class T>
typename vector<T>::pointer vector<T>::allocate_raw(size_type cap) {
    if (cap == 0) {
        return nullptr;
    }
    
    constexpr size_type max_sz = std::min(
        static_cast<size_type>(std::numeric_limits<difference_type>::max()),
        static_cast<size_type>(-1) / sizeof(value_type)
    );
    
    if (cap > max_sz) {
        throw std::length_error("vector::allocate_raw: requested capacity exceeds max_size()");
    }
    
    return static_cast<pointer>(::operator new(sizeof(value_type) * cap));
}

template <class T>
void vector<T>::deallocate_raw(pointer ptr, size_type cap) noexcept {
    if (cap == 0) {
        return;
    }
    ::operator delete(ptr);
}

template <class T>
void vector<T>::destroy_range(pointer first, pointer last) noexcept {
    while (last != first) {
        --last;
        last->~value_type();
    }
}

template <class T>
template <class InputIt>
typename vector<T>::pointer 
vector<T>::uninitialized_copy(pointer dst, InputIt first, InputIt last) {
    pointer current = dst;
    try {
        for (; first != last; ++first, ++current) {
            ::new (static_cast<void*>(current)) value_type(*first);
        }
        return current;
    } catch (...) {
        destroy_range(dst, current);
        throw;
    }
}

template <class T>
template <class InputIt>
typename vector<T>::pointer 
vector<T>::uninitialized_move(pointer dst, InputIt first, InputIt last) {
    pointer current = dst;
    try {
        for (; first != last; ++first, ++current) {
            ::new (static_cast<void*>(current)) value_type(std::move(*first));
        }
        return current;
    } catch (...) {
        destroy_range(dst, current);
        throw;
    }
}

template <class T>
void vector<T>::reallocate(size_type new_cap) {
    if (new_cap == capacity_) {
        return;
    }
    
    pointer new_data = allocate_raw(new_cap);
    size_type i = 0;
    
    try {
        for (; i < size_; ++i) {
            ::new (static_cast<void*>(new_data + i)) 
                value_type(std::move_if_noexcept(data_[i]));
        }
    } catch (...) {
        destroy_range(new_data, new_data + i);
        deallocate_raw(new_data, new_cap);
        throw;
    }
    
    destroy_range(data_, data_ + size_);
    deallocate_raw(data_, capacity_);
    
    data_ = new_data;
    capacity_ = new_cap;
}

template <class T>
typename vector<T>::size_type 
vector<T>::grow_capacity_for(size_type min_cap) const noexcept {
    if (min_cap > max_size()) {
        return max_size();
    }
    
    const size_type geometric = capacity_ + capacity_ / 2; // 1.5x growth
    
    if (geometric < min_cap) {
        return min_cap;
    }
    
    if (geometric > max_size()) {
        return max_size();
    }
    
    return geometric;
}

template <class T>
void vector<T>::check_index(size_type idx, size_type size) {
    if (idx >= size) {
        throw std::out_of_range("vector::check_index: index out of range");
    }
}

// ============================================================================
// Implementation - Constructors and Destructor
// ============================================================================

template <class T>
vector<T>::vector() noexcept 
    : size_{0}, capacity_{0}, data_{nullptr} {
}

template <class T>
vector<T>::vector(size_type n) 
    : size_{0}, capacity_{0}, data_{nullptr} {
    if (n == 0) {
        return;
    }
    
    pointer p = allocate_raw(n);
    size_type i = 0;
    
    try {
        for (; i < n; ++i) {
            ::new (static_cast<void*>(p + i)) value_type();
        }
    } catch (...) {
        destroy_range(p, p + i);
        deallocate_raw(p, n);
        throw;
    }
    
    data_ = p;
    size_ = n;
    capacity_ = n;
}

template <class T>
vector<T>::vector(size_type n, const_reference value) 
    : size_{0}, capacity_{0}, data_{nullptr} {
    if (n == 0) {
        return;
    }
    
    pointer p = allocate_raw(n);
    size_type i = 0;
    
    try {
        for (; i < n; ++i) {
            ::new (static_cast<void*>(p + i)) value_type(value);
        }
    } catch (...) {
        destroy_range(p, p + i);
        deallocate_raw(p, n);
        throw;
    }
    
    data_ = p;
    size_ = n;
    capacity_ = n;
}

template <class T>
vector<T>::vector(const vector& other) 
    : size_{0}, capacity_{0}, data_{nullptr} {
    if (other.size_ == 0) {
        return;
    }
    
    const size_type n = other.size_;
    pointer p = allocate_raw(n);
    size_type i = 0;
    
    try {
        for (; i < n; ++i) {
            ::new (static_cast<void*>(p + i)) value_type(other.data_[i]);
        }
    } catch (...) {
        destroy_range(p, p + i);
        deallocate_raw(p, n);
        throw;
    }
    
    data_ = p;
    size_ = n;
    capacity_ = n;
}

template <class T>
vector<T>::vector(vector&& other) noexcept 
    : size_{other.size_}, capacity_{other.capacity_}, data_{other.data_} {
    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
}

template <class T>
template <class InputIt>
    requires(!std::is_integral_v<InputIt> && std::forward_iterator<InputIt>)
vector<T>::vector(InputIt first, InputIt last) 
    : size_{0}, capacity_{0}, data_{nullptr} {
    const size_type n = static_cast<size_type>(std::distance(first, last));
    if (n == 0) {
        return;
    }
    
    pointer p = allocate_raw(n);
    
    try {
        uninitialized_copy(p, first, last);
    } catch (...) {
        deallocate_raw(p, n);
        throw;
    }
    
    data_ = p;
    size_ = n;
    capacity_ = n;
}

template <class T>
vector<T>::vector(std::initializer_list<value_type> init) 
    : vector(init.begin(), init.end()) {
}

template <class T>
vector<T>::~vector() {
    destroy_range(data_, data_ + size_);
    deallocate_raw(data_, capacity_);
}

// ============================================================================
// Implementation - Assignment Operators
// ============================================================================

template <class T>
vector<T>& vector<T>::operator=(const vector& other) {
    if (this == &other) {
        return *this;
    }
    
    vector tmp(other);
    swap(tmp);
    return *this;
}

template <class T>
vector<T>& vector<T>::operator=(vector&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    
    destroy_range(data_, data_ + size_);
    deallocate_raw(data_, capacity_);
    
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
    
    return *this;
}

template <class T>
vector<T>& vector<T>::operator=(std::initializer_list<value_type> init) {
    vector tmp(init);
    swap(tmp);
    return *this;
}

template <class T>
void vector<T>::swap(vector& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

// ============================================================================
// Implementation - Capacity
// ============================================================================

template <class T>
typename vector<T>::size_type vector<T>::size() const noexcept {
    return size_;
}

template <class T>
typename vector<T>::size_type vector<T>::capacity() const noexcept {
    return capacity_;
}

template <class T>
bool vector<T>::empty() const noexcept {
    return size_ == 0;
}

template <class T>
typename vector<T>::size_type vector<T>::max_size() const noexcept {
    return std::min(
        static_cast<size_type>(std::numeric_limits<difference_type>::max()),
        static_cast<size_type>(-1) / sizeof(value_type)
    );
}

template <class T>
void vector<T>::reserve(size_type new_cap) {
    if (new_cap <= capacity_) {
        return;
    }
    reallocate(new_cap);
}

template <class T>
void vector<T>::shrink_to_fit() {
    if (size_ == capacity_) {
        return;
    }
    
    if (size_ == 0) {
        deallocate_raw(data_, capacity_);
        data_ = nullptr;
        capacity_ = 0;
        return;
    }
    
    reallocate(size_);
}

// ============================================================================
// Implementation - Element Access
// ============================================================================

template <class T>
typename vector<T>::pointer vector<T>::data() noexcept {
    return data_;
}

template <class T>
typename vector<T>::const_pointer vector<T>::data() const noexcept {
    return data_;
}

template <class T>
typename vector<T>::reference vector<T>::operator[](size_type idx) noexcept {
    return data_[idx];
}

template <class T>
typename vector<T>::const_reference vector<T>::operator[](size_type idx) const noexcept {
    return data_[idx];
}

template <class T>
typename vector<T>::reference vector<T>::at(size_type idx) {
    check_index(idx, size_);
    return data_[idx];
}

template <class T>
typename vector<T>::const_reference vector<T>::at(size_type idx) const {
    check_index(idx, size_);
    return data_[idx];
}

template <class T>
typename vector<T>::reference vector<T>::front() noexcept {
    return data_[0];
}

template <class T>
typename vector<T>::const_reference vector<T>::front() const noexcept {
    return data_[0];
}

template <class T>
typename vector<T>::reference vector<T>::back() noexcept {
    return data_[size_ - 1];
}

template <class T>
typename vector<T>::const_reference vector<T>::back() const noexcept {
    return data_[size_ - 1];
}

// ============================================================================
// Implementation - Iterators
// ============================================================================

template <class T>
typename vector<T>::iterator vector<T>::begin() noexcept {
    return data_;
}

template <class T>
typename vector<T>::const_iterator vector<T>::begin() const noexcept {
    return data_;
}

template <class T>
typename vector<T>::const_iterator vector<T>::cbegin() const noexcept {
    return data_;
}

template <class T>
typename vector<T>::iterator vector<T>::end() noexcept {
    return data_ + size_;
}

template <class T>
typename vector<T>::const_iterator vector<T>::end() const noexcept {
    return data_ + size_;
}

template <class T>
typename vector<T>::const_iterator vector<T>::cend() const noexcept {
    return data_ + size_;
}

// ============================================================================
// Implementation - Modifiers
// ============================================================================

template <class T>
void vector<T>::clear() noexcept {
    destroy_range(data_, data_ + size_);
    size_ = 0;
}

template <class T>
void vector<T>::push_back(const_reference value) {
    emplace_back(value);
}

template <class T>
void vector<T>::push_back(value_type&& value) {
    emplace_back(std::move(value));
}

template <class T>
template <class... Args>
typename vector<T>::reference vector<T>::emplace_back(Args&&... args) {
    if (size_ == capacity_) {
        const size_type new_cap = (capacity_ == 0) ? 1 : grow_capacity_for(capacity_ + 1);
        reserve(new_cap);
    }
    
    pointer p = ::new (static_cast<void*>(data_ + size_)) 
        value_type(std::forward<Args>(args)...);
    ++size_;
    return *p;
}

template <class T>
void vector<T>::pop_back() noexcept {
    if (size_ == 0) return;
    --size_;
    data_[size_].~value_type();
}

template <class T>
void vector<T>::resize(size_type n) {
    if (n < size_) {
        destroy_range(data_ + n, data_ + size_);
        size_ = n;
    } else if (n > size_) {
        reserve(n);
        for (size_type i = size_; i < n; ++i) {
            ::new (static_cast<void*>(data_ + i)) value_type();
        }
        size_ = n;
    }
}

template <class T>
void vector<T>::resize(size_type n, const_reference value) {
    if (n < size_) {
        destroy_range(data_ + n, data_ + size_);
        size_ = n;
    } else if (n > size_) {
        reserve(n);
        for (size_type i = size_; i < n; ++i) {
            ::new (static_cast<void*>(data_ + i)) value_type(value);
        }
        size_ = n;
    }
}

// ============================================================================
// Non-member Functions
// ============================================================================

/**
 * @brief Swaps two vectors
 */
template <class T>
void swap(vector<T>& lhs, vector<T>& rhs) noexcept {
    lhs.swap(rhs);
}

/**
 * @brief Equality comparison
 */
template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
 * @brief Inequality comparison
 */
template <class T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Less-than comparison
 */
template <class T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(), 
        rhs.begin(), rhs.end()
    );
}

/**
 * @brief Less-than-or-equal comparison
 */
template <class T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Greater-than comparison
 */
template <class T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Greater-than-or-equal comparison
 */
template <class T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs < rhs);
}

} // namespace stlish

#endif // STLISH_VECTOR_HPP
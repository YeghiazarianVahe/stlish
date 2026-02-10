#ifndef STLISH_ARRAY_HPP
#define STLISH_ARRAY_HPP

#include <cstddef>
#include <stdexcept>
#include <iterator>
#include <algorithm>

namespace stlish {

template<typename T, std::size_t N>
struct array {
    // ============================================================
    // MEMBER TYPES
    // ============================================================
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // ============================================================
    // DATA MEMBER (public for aggregate initialization)
    // ============================================================
    T _data[N];

    // ============================================================
    // ELEMENT ACCESS (TODO: Implement these)
    // ============================================================
    // reference operator[](size_type pos);
    // const_reference operator[](size_type pos) const;
    // reference at(size_type pos);
    // const_reference at(size_type pos) const;
    // reference front();
    // const_reference front() const;
    // reference back();
    // const_reference back() const;
    // pointer data() noexcept;
    // const_pointer data() const noexcept;

    // ============================================================
    // ITERATORS (TODO: Implement these)
    // ============================================================
    // iterator begin() noexcept;
    // const_iterator begin() const noexcept;
    // const_iterator cbegin() const noexcept;
    // iterator end() noexcept;
    // const_iterator end() const noexcept;
    // const_iterator cend() const noexcept;
    // reverse_iterator rbegin() noexcept;
    // const_reverse_iterator rbegin() const noexcept;
    // const_reverse_iterator crbegin() const noexcept;
    // reverse_iterator rend() noexcept;
    // const_reverse_iterator rend() const noexcept;
    // const_reverse_iterator crend() const noexcept;

    // ============================================================
    // CAPACITY (TODO: Implement these)
    // ============================================================
    // constexpr bool empty() const noexcept;
    // constexpr size_type size() const noexcept;
    // constexpr size_type max_size() const noexcept;

    // ============================================================
    // OPERATIONS (TODO: Implement these)
    // ============================================================
    // void fill(const T& value);
    // void swap(array& other) noexcept(/* conditional */);
};

// ============================================================
// NON-MEMBER FUNCTIONS (TODO: Implement these)
// ============================================================
// Comparison operators
// template<typename T, std::size_t N>
// bool operator==(const array<T, N>& lhs, const array<T, N>& rhs);
// ... other comparison operators

// Specialized swap
// template<typename T, std::size_t N>
// void swap(array<T, N>& lhs, array<T, N>& rhs) noexcept(noexcept(lhs.swap(rhs)));

} // namespace stlish

#endif // STLISH_ARRAY_HPP
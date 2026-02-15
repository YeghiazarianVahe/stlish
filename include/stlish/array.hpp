// file: include/stlish/array.hpp
// ============================================================================
// stlish::array<T, N>
//
// A fixed-size aggregate container with value semantics.
// Simplified from std::array (C++20, [array]).
//
// Complexity:
//   All element access and iterator operations – O(1).
//   fill()  – O(N).
//   swap()  – O(N) (element-wise swap; no heap involved).
//
// Iterator invalidation:
//   Iterators are raw-pointer wrappers. They are never invalidated by any
//   member function. Copies/moves of the array leave iterators into the
//   *original* object valid; they do NOT transfer to the new object.
//
// Exception safety:
//   operator[], front(), back(), data(), begin/end, size, empty – noexcept.
//   at()   – throws std::out_of_range on out-of-bounds access; strong guarantee.
//   fill() – strong guarantee if T::operator= does not throw; otherwise basic.
//   swap() – noexcept iff swapping T is noexcept.
//
// Deviations from std::array:
//   - front() and back() on a zero-length array are statically rejected
//     (std::array leaves them undefined behaviour; we promote UB to a
//     compile-time error via static_assert).
//   - No std::get<I> / std::tuple_size / std::tuple_element specialisations.
//   - No deduction guides (not needed; N is always explicit).
// ============================================================================
#ifndef STLISH_ARRAY_HPP
#define STLISH_ARRAY_HPP

#include "stlish/iterator.hpp"

#include <algorithm>    // std::equal, std::lexicographical_compare_three_way
#include <cstddef>      // std::size_t, std::ptrdiff_t
#include <stdexcept>    // std::out_of_range
#include <utility>      // std::swap

namespace stlish {

template <typename T, std::size_t N>
struct array {
    // =========================================================================
    // MEMBER TYPES
    // =========================================================================
    using value_type             = T;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;

    using reference              = value_type&;
    using const_reference        = const value_type&;

    using pointer                = value_type*;
    using const_pointer          = const value_type*;

    using iterator               = array_iterator<T, false>;
    using const_iterator         = array_iterator<T, true>;

    // Qualify with stlish:: to prevent the alias from shadowing
    // the template name within the struct scope, which causes a hard error
    // on the very next line when the compiler can no longer resolve the template.
    using reverse_iterator       = stlish::reverse_iterator<iterator>;
    using const_reverse_iterator = stlish::reverse_iterator<const_iterator>;

    // =========================================================================
    // DATA MEMBER
    // Public for aggregate initialisation (same as std::array).
    // N==0: one-element dummy array keeps the type a valid aggregate and gives
    // data() a non-null pointer to return, matching typical std::array behaviour.
    // The dummy element is never accessed through any public member function.
    // =========================================================================
    value_type _data[(N == 0) ? 1 : N];

    // =========================================================================
    // CAPACITY
    // =========================================================================

    [[nodiscard]] constexpr size_type size()     const noexcept { return N; }
    [[nodiscard]] constexpr size_type max_size() const noexcept { return N; }
    [[nodiscard]] constexpr bool      empty()    const noexcept { return N == 0; }

    // =========================================================================
    // ELEMENT ACCESS
    // =========================================================================

    // Unchecked access – undefined behaviour if pos >= N (matches std).
    constexpr reference       operator[](size_type pos)       noexcept { return _data[pos]; }
    constexpr const_reference operator[](size_type pos) const noexcept { return _data[pos]; }

    // Checked access – throws std::out_of_range if pos >= N.
    // Both overloads now carry the required diagnostic string.
    // std::out_of_range inherits std::logic_error which has no default constructor.
    constexpr reference at(size_type pos) {
        if (pos >= N) throw std::out_of_range("stlish::array::at: index out of range");
        return _data[pos];
    }

    constexpr const_reference at(size_type pos) const {
        if (pos >= N) throw std::out_of_range("stlish::array::at: index out of range");
        return _data[pos];
    }

    // front() and back() are undefined behaviour for N==0.
    // Promote UB to a compile-time error so the caller sees a clear diagnostic
    // instead of silent memory corruption at runtime.
    constexpr reference front() noexcept {
        static_assert(N > 0, "stlish::array::front called on zero-length array");
        return _data[0];
    }

    constexpr const_reference front() const noexcept {
        static_assert(N > 0, "stlish::array::front called on zero-length array");
        return _data[0];
    }

    constexpr reference back() noexcept {
        static_assert(N > 0, "stlish::array::back called on zero-length array");
        return _data[N - 1];
    }

    constexpr const_reference back() const noexcept {
        static_assert(N > 0, "stlish::array::back called on zero-length array");
        return _data[N - 1];
    }

    constexpr pointer       data()       noexcept { return _data; }
    constexpr const_pointer data() const noexcept { return _data; }

    // =========================================================================
    // ITERATORS
    // =========================================================================

    constexpr iterator       begin()        noexcept { return iterator{_data};        }
    constexpr const_iterator begin()  const noexcept { return const_iterator{_data};  }
    constexpr const_iterator cbegin() const noexcept { return const_iterator{_data};  }

    constexpr iterator       end()        noexcept { return iterator{_data + N};       }
    constexpr const_iterator end()  const noexcept { return const_iterator{_data + N}; }
    constexpr const_iterator cend() const noexcept { return const_iterator{_data + N}; }

    constexpr reverse_iterator       rbegin()        noexcept { return reverse_iterator{end()};              }
    constexpr const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator{end()};        }
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{end()};        }

    constexpr reverse_iterator       rend()        noexcept { return reverse_iterator{begin()};              }
    constexpr const_reverse_iterator rend()  const noexcept { return const_reverse_iterator{begin()};        }
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator{begin()};        }

    // =========================================================================
    // MODIFIERS
    // =========================================================================

    // Assigns value to every element. O(N).
    // Exception safety: if T::operator= throws, elements already assigned are
    // changed and earlier elements are not restored – basic guarantee.
    constexpr void fill(const value_type& value) {
        for (size_type i = 0; i < N; ++i) {
            _data[i] = value;
        }
    }

    // Element-wise swap. noexcept iff swapping T is noexcept.
    constexpr void swap(array& other)
        noexcept(noexcept(std::swap(_data[0], other._data[0])))
    {
        for (size_type i = 0; i < N; ++i) {
            std::swap(_data[i], other._data[i]);
        }
    }
};

// =============================================================================
// NON-MEMBER COMPARISON OPERATORS
// =============================================================================

// Equality: element-wise comparison; O(N).
template <typename T, std::size_t N>
constexpr bool operator==(const array<T, N>& lhs, const array<T, N>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

// Three-way comparison: lexicographic; requires T to be three_way_comparable.
// The return type deduces from T's <=> (strong_ordering / weak_ordering / etc.).
template <typename T, std::size_t N>
constexpr auto operator<=>(const array<T, N>& lhs, const array<T, N>& rhs) {
    return std::lexicographical_compare_three_way(
        lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end()
    );
}

// Non-member swap: participates in ADL so std::swap and algorithms find it.
template <typename T, std::size_t N>
constexpr void swap(array<T, N>& lhs, array<T, N>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace stlish

#endif // STLISH_ARRAY_HPP
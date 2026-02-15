// file: include/stlish/iterator.hpp
// ============================================================================
// stlish::array_iterator<T, IsConst>  — contiguous/random-access iterator
// stlish::reverse_iterator<Iter>      — generic reverse iterator adapter
//
// Complexity: All iterator operations are O(1).
// Exception safety: All operations are noexcept.
// Iterator invalidation: Follows the rules of the underlying container.
//
// Deviations from std:
//   - reverse_iterator does not propagate contiguous_iterator_tag
//     (matches the standard: reversed ranges are not contiguous).
//   - No support for std::move_iterator or std::common_iterator.
// ============================================================================
#ifndef STLISH_ITERATOR_HPP
#define STLISH_ITERATOR_HPP

#include <compare>      // std::strong_ordering, <=>
#include <cstddef>      // std::ptrdiff_t
#include <type_traits>  // std::conditional_t, std::enable_if_t
#include <iterator>     // std::random_access_iterator_tag (tag types only)

namespace stlish {

// ============================================================================
// array_iterator — contiguous random-access iterator
// ============================================================================
// Provides both mutable (IsConst=false) and const (IsConst=true) variants.
// Non-const implicitly converts to const.
// ============================================================================
template <class T, bool IsConst>
class array_iterator {
public:
    // -- iterator_traits typedefs --
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = std::conditional_t<IsConst, const T*, T*>;
    using reference         = std::conditional_t<IsConst, const T&, T&>;
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept  = std::contiguous_iterator_tag;  // C++20

    // -- constructors --
    constexpr array_iterator() noexcept = default;

    constexpr explicit array_iterator(pointer p) noexcept
        : ptr_(p) {}

    /// Converting constructor: non-const → const (SFINAE-guarded).
    template <bool WasConst, class = std::enable_if_t<!WasConst && IsConst>>
    constexpr array_iterator(const array_iterator<T, WasConst>& other) noexcept
        : ptr_(other.base()) {}

    // -- element access --
    constexpr reference operator*()  const noexcept { return *ptr_; }
    constexpr pointer   operator->() const noexcept { return  ptr_; }
    constexpr reference operator[](difference_type n) const noexcept { return ptr_[n]; }

    // -- increment / decrement --
    constexpr array_iterator& operator++()    noexcept { ++ptr_; return *this; }
    constexpr array_iterator  operator++(int) noexcept { auto tmp = *this; ++(*this); return tmp; }
    constexpr array_iterator& operator--()    noexcept { --ptr_; return *this; }
    constexpr array_iterator  operator--(int) noexcept { auto tmp = *this; --(*this); return tmp; }

    // -- arithmetic --
    constexpr array_iterator& operator+=(difference_type n) noexcept { ptr_ += n; return *this; }
    constexpr array_iterator& operator-=(difference_type n) noexcept { ptr_ -= n; return *this; }

    friend constexpr array_iterator operator+(array_iterator it, difference_type n) noexcept { it += n; return it; }
    friend constexpr array_iterator operator+(difference_type n, array_iterator it) noexcept { it += n; return it; }
    friend constexpr array_iterator operator-(array_iterator it, difference_type n) noexcept { it -= n; return it; }

    friend constexpr difference_type operator-(array_iterator a, array_iterator b) noexcept {
        return a.ptr_ - b.ptr_;
    }

    // -- comparison --
    friend constexpr bool operator==(const array_iterator& a, const array_iterator& b) noexcept {
        return a.ptr_ == b.ptr_;
    }
    friend constexpr auto operator<=>(const array_iterator& a, const array_iterator& b) noexcept {
        return a.ptr_ <=> b.ptr_;
    }

    // -- base accessor (needed by reverse_iterator and const conversion) --
    constexpr pointer base() const noexcept { return ptr_; }

private:
    pointer ptr_ = nullptr;
};


// ============================================================================
// reverse_iterator — generic adapter for any bidirectional+ iterator
// ============================================================================
//
// Core invariant:
//   *reverse_iterator(it)  ==  *(it - 1)
//
// This is the "off-by-one" design from the standard. rbegin() wraps end(),
// so dereferencing backs up one position to reach the last real element.
//
// Why this design?
//   It allows rbegin()/rend() to be formed from end()/begin() without
//   needing a "before-begin" sentinel, which wouldn't be valid for pointers.
//
// Iterator category:
//   Same as the wrapped iterator, EXCEPT contiguous_iterator_tag is
//   demoted to random_access_iterator_tag (reversed pointer sequences
//   are not contiguous by the standard's definition).
//
// ============================================================================
namespace detail {

/// Demote contiguous_iterator_tag to random_access_iterator_tag;
/// pass everything else through unchanged.
template <typename Category>
struct demote_contiguous {
    using type = Category;
};

template <>
struct demote_contiguous<std::contiguous_iterator_tag> {
    using type = std::random_access_iterator_tag;
};

template <typename Category>
using demote_contiguous_t = typename demote_contiguous<Category>::type;

} // namespace detail


template <class Iter>
class reverse_iterator {
public:
    // -- iterator_traits typedefs --
    // We extract traits from Iter directly (no std::iterator_traits dependency).
    using iterator_type     = Iter;
    using value_type        = typename Iter::value_type;
    using difference_type   = typename Iter::difference_type;
    using pointer           = typename Iter::pointer;
    using reference         = typename Iter::reference;
    using iterator_category = detail::demote_contiguous_t<typename Iter::iterator_category>;

    // ================================================================
    // CONSTRUCTORS
    // ================================================================

    /// Default: wraps a value-initialized iterator.
    constexpr reverse_iterator() noexcept = default;

    /// Wrap a forward iterator.  *this will dereference to *(current - 1).
    constexpr explicit reverse_iterator(Iter it) noexcept
        : current_(it) {}

    /// Converting constructor: reverse_iterator<non-const> → reverse_iterator<const>.
    /// Works because the underlying Iter must be convertible to UIter.
    template <class UIter>
        requires std::convertible_to<UIter, Iter>
    constexpr reverse_iterator(const reverse_iterator<UIter>& other) noexcept
        : current_(other.base()) {}

    // ================================================================
    // ELEMENT ACCESS
    // ================================================================

    /// Dereference: returns *(base() - 1).
    /// This is the core off-by-one semantic of reverse iterators.
    constexpr reference operator*() const noexcept {
        Iter tmp = current_;
        --tmp;
        return *tmp;
    }

    constexpr pointer operator->() const noexcept {
        Iter tmp = current_;
        --tmp;
        return tmp.operator->();
    }

    /// Random-access subscript: rit[n] == *(rit + n) == *(base() - 1 - n).
    constexpr reference operator[](difference_type n) const noexcept {
        return *(*this + n);
    }

    // ================================================================
    // INCREMENT / DECREMENT  (reversed directions!)
    // ================================================================
    constexpr reverse_iterator& operator++()    noexcept { --current_; return *this; }
    constexpr reverse_iterator  operator++(int) noexcept { auto tmp = *this; --current_; return tmp; }
    constexpr reverse_iterator& operator--()    noexcept { ++current_; return *this; }
    constexpr reverse_iterator  operator--(int) noexcept { auto tmp = *this; ++current_; return tmp; }

    // ================================================================
    // ARITHMETIC  (reversed signs!)
    // ================================================================
    constexpr reverse_iterator& operator+=(difference_type n) noexcept { current_ -= n; return *this; }
    constexpr reverse_iterator& operator-=(difference_type n) noexcept { current_ += n; return *this; }

    friend constexpr reverse_iterator operator+(reverse_iterator it, difference_type n) noexcept {
        it += n; return it;
    }
    friend constexpr reverse_iterator operator+(difference_type n, reverse_iterator it) noexcept {
        it += n; return it;
    }
    friend constexpr reverse_iterator operator-(reverse_iterator it, difference_type n) noexcept {
        it -= n; return it;
    }

    /// Distance between two reverse iterators: note reversed order!
    /// (rbegin - rend) should be positive, but base(rbegin) > base(rend),
    /// so we swap:  a - b  ==  b.base() - a.base().
    friend constexpr difference_type operator-(const reverse_iterator& a,
                                               const reverse_iterator& b) noexcept {
        return b.current_ - a.current_;
    }

    // ================================================================
    // COMPARISON  (reversed ordering!)
    // ================================================================
    // Two reverse iterators compare in the *opposite* order of their bases.
    // a < b in reverse means a.base() > b.base().
    // Using <=> on the bases gives the right result if we flip the operands.

    template <class UIter>
    friend constexpr bool operator==(const reverse_iterator& a,
                                     const reverse_iterator<UIter>& b) noexcept {
        return a.current_ == b.base();
    }

    template <class UIter>
    friend constexpr auto operator<=>(const reverse_iterator& a,
                                      const reverse_iterator<UIter>& b) noexcept {
        // Flip: if a.base > b.base, then a is logically *before* b in reverse,
        // so a < b.  This means we compare b.base <=> a.base.
        return b.base() <=> a.current_;
    }

    // ================================================================
    // BASE ACCESSOR
    // ================================================================
    /// Returns the underlying forward iterator.
    constexpr Iter base() const noexcept { return current_; }

private:
    Iter current_{};
};


// ============================================================================
// Helper: make_reverse_iterator (matches std::make_reverse_iterator)
// ============================================================================
template <class Iter>
constexpr reverse_iterator<Iter> make_reverse_iterator(Iter it) noexcept {
    return reverse_iterator<Iter>(it);
}

} // namespace stlish

#endif // STLISH_ITERATOR_HPP
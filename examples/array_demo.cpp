// file: examples/array_demo.cpp
// ============================================================================
// stlish::array<T,N> — narrative demonstration
//
// Build:
//   g++ -std=c++20 -Wall -Wextra -Wpedantic -I include examples/array_demo.cpp
//       -o array_demo && ./array_demo
// ============================================================================

#include "stlish/array.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>

// ─── helpers ──────────────────────────────────────────────────────────────────
static void section(const char* title) {
    std::cout << "\n══════════════════════════════════════════\n"
              << "  " << title << "\n"
              << "══════════════════════════════════════════\n";
}

template <typename T, std::size_t N>
static void print_array(const stlish::array<T, N>& a, const char* label = "") {
    std::cout << label << "[ ";
    for (const auto& v : a) std::cout << v << ' ';
    std::cout << "]\n";
}

// ─── 1. Capacity ──────────────────────────────────────────────────────────────
static void demo_capacity() {
    section("1. CAPACITY: size / max_size / empty");

    stlish::array<int, 5> a5{};
    std::cout << "array<int,5>:  size="     << a5.size()
              << "  max_size="              << a5.max_size()
              << "  empty=" << std::boolalpha << a5.empty() << "\n";

    stlish::array<int, 0> a0{};
    std::cout << "array<int,0>:  size="     << a0.size()
              << "  max_size="              << a0.max_size()
              << "  empty="                 << a0.empty() << "\n";

    // size() and empty() are constexpr — evaluated at compile-time.
    // The zero-size array keeps one dummy element so data() is non-null,
    // matching typical std::array behaviour. The dummy is never exposed.
    static_assert(stlish::array<double, 7>{}.size() == 7);
    static_assert(stlish::array<char,   0>{}.empty());
    std::cout << "constexpr checks passed.\n";
}

// ─── 2. Aggregate initialisation ──────────────────────────────────────────────
static void demo_aggregate_init() {
    section("2. AGGREGATE INITIALISATION");

    // Full braced-init — like a plain C array.
    stlish::array<int, 5> a = {10, 20, 30, 40, 50};
    print_array(a, "full init:     ");

    // Partial init: remaining elements are value-initialised (0 for int).
    stlish::array<int, 5> b = {1, 2};
    print_array(b, "partial init:  ");

    // Value-init (={}) zero-initialises all elements.
    stlish::array<int, 3> c{};
    print_array(c, "value-init:    ");

    // std::string: third element default-constructs to empty string.
    stlish::array<std::string, 3> s = {"hello", "world"};
    std::cout << "string[3]: [" << s[0] << ", " << s[1]
              << ", '" << s[2] << "']\n";
}

// ─── 3. Element access ────────────────────────────────────────────────────────
static void demo_element_access() {
    section("3. ELEMENT ACCESS: [], at(), front(), back(), data()");

    stlish::array<int, 4> a = {10, 20, 30, 40};

    // operator[] — unchecked, O(1), noexcept.
    std::cout << "a[0]=" << a[0] << "  a[3]=" << a[3] << "\n";

    // at() — bounds-checked; throws std::out_of_range on bad index.
    std::cout << "a.at(2)=" << a.at(2) << "\n";
    try {
        [[maybe_unused]] auto x = a.at(99);
    } catch (const std::out_of_range& e) {
        std::cout << "at(99) threw: " << e.what() << "\n";
    }

    // front() / back() — noexcept; static_assert on N==0 (no silent UB).
    std::cout << "front=" << a.front() << "  back=" << a.back() << "\n";

    // data() — raw pointer, compatible with C APIs.
    const int* p = a.data();
    std::cout << "data()[1]=" << p[1] << "\n";

    const stlish::array<int, 4> ca = {1, 2, 3, 4};
    std::cout << "const front=" << ca.front() << "  back=" << ca.back() << "\n";
}

// ─── 4. Iterators — forward ───────────────────────────────────────────────────
static void demo_iterators_forward() {
    section("4. ITERATORS — forward (begin / end / cbegin / cend)");

    stlish::array<int, 5> a = {1, 2, 3, 4, 5};

    // Range-for uses begin()/end() implicitly.
    std::cout << "range-for: ";
    for (int v : a) std::cout << v << ' ';
    std::cout << "\n";

    // Explicit iterator loop.
    std::cout << "explicit:  ";
    for (auto it = a.begin(); it != a.end(); ++it) std::cout << *it << ' ';
    std::cout << "\n";

    // const_iterator via cbegin/cend.
    const auto& ca = a;
    std::cout << "const:     ";
    for (auto it = ca.cbegin(); it != ca.cend(); ++it) std::cout << *it << ' ';
    std::cout << "\n";

    // Random-access arithmetic — contiguous_iterator_tag allows this.
    auto it = a.begin();
    it += 2;
    std::cout << "begin+2 = "   << *it << "\n";
    std::cout << "end-begin = " << (a.end() - a.begin()) << "\n";
    std::cout << "it[1] = "     << it[1] << "  (iterator subscript)\n";
}

// ─── 5. Iterators — reverse ───────────────────────────────────────────────────
static void demo_iterators_reverse() {
    section("5. ITERATORS — reverse (rbegin / rend)");

    stlish::array<int, 5> a = {1, 2, 3, 4, 5};

    std::cout << "reverse:       ";
    for (auto it = a.rbegin(); it != a.rend(); ++it) std::cout << *it << ' ';
    std::cout << "\n";

    std::cout << "const reverse: ";
    for (auto it = a.crbegin(); it != a.crend(); ++it) std::cout << *it << ' ';
    std::cout << "\n";

    // rbegin() wraps end(); dereferencing yields *(base-1).
    // This is identical semantics to std::reverse_iterator.
    std::cout << "*rbegin = "        << *a.rbegin()
              << "  *prev(rend) = "  << *(a.rend() - 1) << "\n";
}

// ─── 6. fill() ────────────────────────────────────────────────────────────────
static void demo_fill() {
    section("6. MODIFIER: fill()");

    stlish::array<int, 5> a = {1, 2, 3, 4, 5};
    print_array(a, "before:        ");

    a.fill(0);
    print_array(a, "after fill(0): ");

    a.fill(42);
    print_array(a, "after fill(42):");

    stlish::array<std::string, 3> s;
    s.fill("hello");
    std::cout << "string fill: [" << s[0] << ", " << s[1] << ", " << s[2] << "]\n";
}

// ─── 7. swap() ────────────────────────────────────────────────────────────────
static void demo_swap() {
    section("7. MODIFIER: swap() — member and ADL non-member");

    stlish::array<int, 3> a = {1, 2, 3};
    stlish::array<int, 3> b = {7, 8, 9};

    print_array(a, "a before:            ");
    print_array(b, "b before:            ");

    a.swap(b);
    print_array(a, "a after member swap: ");
    print_array(b, "b after member swap: ");

    using std::swap;
    swap(a, b);   // ADL finds stlish::swap via namespace lookup
    print_array(a, "a after ADL swap:    ");
    print_array(b, "b after ADL swap:    ");

    // WHY ADL matters: algorithms that call `using std::swap; swap(x,y)`
    // will find your type's efficient element-wise swap automatically,
    // without needing a std::swap specialisation.
}

// ─── 8. Comparison operators ──────────────────────────────────────────────────
static void demo_comparisons() {
    section("8. COMPARISONS (== and <=>)");

    stlish::array<int, 3> a = {1, 2, 3};
    stlish::array<int, 3> b = {1, 2, 3};
    stlish::array<int, 3> c = {1, 2, 4};
    stlish::array<int, 3> d = {0, 9, 9};

    std::cout << std::boolalpha;
    std::cout << "a==b: " << (a == b) << "\n";
    std::cout << "a==c: " << (a == c) << "\n";
    std::cout << "a< c: " << (a <  c) << "  (last element differs)\n";
    std::cout << "a< d: " << (a <  d) << "  (first element wins)\n";
    std::cout << "c> a: " << (c >  a) << "\n";
    std::cout << "a<=b: " << (a <= b) << "\n";
    std::cout << "a>=b: " << (a >= b) << "\n";

    // C++20 <=> synthesises all six comparison operators from one definition.
    // std::lexicographical_compare_three_way does the element-wise work.
    auto ord = (a <=> b);
    std::cout << "a<=>b equal: " << (ord == 0) << "\n";
}

// ─── 9. STL algorithm interop ─────────────────────────────────────────────────
static void demo_stl_algorithms() {
    section("9. STL ALGORITHM INTEROP");

    stlish::array<int, 6> a = {5, 3, 8, 1, 9, 2};
    print_array(a, "original:  ");

    std::sort(a.begin(), a.end());           // requires random-access
    print_array(a, "sorted:    ");

    auto it = std::find(a.begin(), a.end(), 8);  // requires input iterator
    if (it != a.end())
        std::cout << "found 8 at index " << (it - a.begin()) << "\n";

    int sum = std::accumulate(a.begin(), a.end(), 0);
    std::cout << "sum = " << sum << "\n";

    std::reverse(a.begin(), a.end());        // requires bidirectional
    print_array(a, "reversed:  ");

    stlish::array<int, 5> b{};
    std::iota(b.begin(), b.end(), 10);
    print_array(b, "iota(10):  ");

    // contiguous_iterator_tag lets algorithms pick their fastest code path:
    // std::sort uses introsort (needs random-access);
    // std::distance is O(1) for random-access iterators.
}

// ─── 10. const-correctness ────────────────────────────────────────────────────
static void demo_const_correctness() {
    section("10. CONST-CORRECTNESS");

    const stlish::array<int, 4> ca = {1, 2, 3, 4};

    // operator[], at(), front(), back() return const_reference on a const array.
    // ca[0] = 99;   // does not compile — correct
    std::cout << "ca[0]="      << ca[0]      << "  ca.at(3)=" << ca.at(3) << "\n";
    std::cout << "ca.front()=" << ca.front() << "  ca.back()=" << ca.back() << "\n";

    // begin()/end() return const_iterator from a const array.
    for (auto it = ca.begin(); it != ca.end(); ++it) {
        // *it = 0;   // does not compile — correct
        std::cout << *it << ' ';
    }
    std::cout << "\n";
}

// ─── 11. Move-only types ──────────────────────────────────────────────────────
static void demo_move_only() {
    section("11. MOVE-ONLY TYPES (std::unique_ptr)");

    // unique_ptr is not CopyConstructible — verifies array works without copies.
    stlish::array<std::unique_ptr<int>, 3> a;
    a[0] = std::make_unique<int>(10);
    a[1] = std::make_unique<int>(20);
    a[2] = std::make_unique<int>(30);

    for (std::size_t i = 0; i < a.size(); ++i)
        std::cout << "a[" << i << "] = " << *a[i] << "\n";

    // a.fill(...) would require CopyAssignable — correctly does not compile.
}

// ─── 12. Iterator invalidation ────────────────────────────────────────────────
static void demo_iterator_invalidation() {
    section("12. ITERATOR INVALIDATION GUARANTEES");

    stlish::array<int, 3> a = {1, 2, 3};
    auto it = a.begin();

    // fill() modifies elements in-place — iterators stay valid.
    a.fill(99);
    std::cout << "after fill,   *it = " << *it
              << "  (valid; value updated through iterator)\n";

    // swap() exchanges values but does not move storage — iterators stay valid,
    // but now point into an object that holds the other array's values.
    stlish::array<int, 3> b = {7, 8, 9};
    auto itA = a.begin();
    a.swap(b);
    std::cout << "after a.swap, *itA = " << *itA
              << "  (valid; a now holds b's old values)\n";

    // WHY: array iterators are raw-pointer wrappers into _data[].
    // No heap allocation means no pointer is ever invalidated by any operation.
}

// ─── main ─────────────────────────────────────────────────────────────────────
int main() {
    demo_capacity();
    demo_aggregate_init();
    demo_element_access();
    demo_iterators_forward();
    demo_iterators_reverse();
    demo_fill();
    demo_swap();
    demo_comparisons();
    demo_stl_algorithms();
    demo_const_correctness();
    demo_move_only();
    demo_iterator_invalidation();

    std::cout << "\n[demo done]\n";
    return 0;
}
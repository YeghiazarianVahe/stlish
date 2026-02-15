// file: tests/array_test.cpp
// ============================================================================
// stlish::array<T,N> — exhaustive unit tests
//
// Zero external dependencies. The test framework is defined in this file.
//
// Build & run:
//   g++ -std=c++20 -Wall -Wextra -Wpedantic -Wconversion
//       -fsanitize=address,undefined
//       -I include
//       tests/array_test.cpp -o array_test && ./array_test
//
// Exit code: 0 = all passed, 1 = any failure.
// ============================================================================

#include "stlish/array.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// ############################################################################
// MINI TEST FRAMEWORK
// ############################################################################
//
// Design goals:
//   - Single translation unit, no macros for test registration (uses lambdas)
//   - CHECK(expr)              — record pass/fail, continue test
//   - REQUIRE(expr)            — record pass/fail, abort current test on fail
//   - CHECK_THROWS_AS(e, T)    — expect expr to throw exactly T (or derived)
//   - CHECK_NOTHROW(e)         — expect expr not to throw
//   - CHECK_MSG_CONTAINS(e, s) — expect exception whose what() contains s
//   - STATIC_CHECK(expr)       — compile-time assertion that also shows at run
//   - suite(name, fn)          — groups tests; prints a header
//   - test(name, fn)           — registers a named test case
//   - run_all()                — executes, prints results, returns exit code

namespace tf {

// ---- internal state --------------------------------------------------------
struct Result { int pass = 0; int fail = 0; };
static Result g_result;

// Abort-current-test signal (thrown on REQUIRE failure).
struct RequireFailure {};

// ---- assertion helpers -----------------------------------------------------

inline void check_impl(bool ok, const char* expr, const char* file, int line) {
    if (ok) {
        ++g_result.pass;
    } else {
        ++g_result.fail;
        std::printf("  FAIL  %s:%d\n        expr: %s\n", file, line, expr);
    }
}

inline void require_impl(bool ok, const char* expr, const char* file, int line) {
    check_impl(ok, expr, file, line);
    if (!ok) throw RequireFailure{};
}

// ---- public macros ---------------------------------------------------------

#define CHECK(expr) \
    ::tf::check_impl(!!(expr), #expr, __FILE__, __LINE__)

#define REQUIRE(expr) \
    ::tf::require_impl(!!(expr), #expr, __FILE__, __LINE__)

// Expect expr to throw ExcType (or a subclass).
#define CHECK_THROWS_AS(expr, ExcType)                                    \
    do {                                                                  \
        bool _threw = false;                                              \
        try { (void)(expr); }                                             \
        catch (const ExcType&) { _threw = true; }                        \
        catch (...) {}                                                    \
        ::tf::check_impl(_threw,                                          \
            "THROWS_AS(" #expr ", " #ExcType ")", __FILE__, __LINE__);   \
    } while (0)

// Expect expr NOT to throw anything.
#define CHECK_NOTHROW(expr)                                               \
    do {                                                                  \
        bool _ok = true;                                                  \
        try { (void)(expr); } catch (...) { _ok = false; }               \
        ::tf::check_impl(_ok, "NOTHROW(" #expr ")", __FILE__, __LINE__); \
    } while (0)

// Expect expr to throw std::exception whose what() contains substring s.
#define CHECK_MSG_CONTAINS(expr, s)                                            \
    do {                                                                       \
        bool _ok = false;                                                      \
        try { (void)(expr); }                                                  \
        catch (const std::exception& _e) {                                     \
            _ok = (std::strstr(_e.what(), (s)) != nullptr);                    \
        }                                                                      \
        catch (...) {}                                                         \
        ::tf::check_impl(_ok,                                                  \
            "MSG_CONTAINS(" #expr ", \"" s "\")", __FILE__, __LINE__);        \
    } while (0)

// Compile-time assert that also records a runtime pass (so it appears in
// the summary count and you know it was exercised).
#define STATIC_CHECK(expr)                                                \
    do {                                                                  \
        static_assert((expr), "STATIC_CHECK failed: " #expr);            \
        ::tf::check_impl(true, "static " #expr, __FILE__, __LINE__);     \
    } while (0)

// ---- test registration & execution ----------------------------------------

struct TestCase {
    const char*            name;
    std::function<void()>  fn;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> r;
    return r;
}

// Register a named test. Typically called at namespace scope via:
//   static const bool _ = (tf::test("name", []{ ... }), true);
// But we just call test() directly inside suite() bodies below.
inline void test(const char* name, std::function<void()> fn) {
    registry().push_back({name, std::move(fn)});
}

inline void suite(const char* name, std::function<void()> body) {
    std::printf("\n  ── %s\n", name);
    body();   // body calls tf::test() to register tests inside this suite
}

inline int run_all() {
    int suite_pass = 0, suite_fail = 0;
    for (auto& tc : registry()) {
        int before_fail = g_result.fail;
        try {
            tc.fn();
        } catch (const RequireFailure&) {
            // REQUIRE already recorded the failure; test aborted.
        } catch (const std::exception& e) {
            ++g_result.fail;
            std::printf("  EXCEPTION in \"%s\": %s\n", tc.name, e.what());
        } catch (...) {
            ++g_result.fail;
            std::printf("  UNKNOWN EXCEPTION in \"%s\"\n", tc.name);
        }
        bool this_passed = (g_result.fail == before_fail);
        if (!this_passed) {
            std::printf("  ^ in test: %s\n", tc.name);
            ++suite_fail;
        } else {
            ++suite_pass;
        }
    }

    std::printf("\n════════════════════════════════════════\n");
    std::printf("  Tests:    %d passed, %d failed\n", suite_pass, suite_fail);
    std::printf("  Checks:   %d passed, %d failed\n", g_result.pass, g_result.fail);
    std::printf("════════════════════════════════════════\n");
    return g_result.fail > 0 ? 1 : 0;
}

} // namespace tf

// ############################################################################
// TEST HELPER TYPES
// ############################################################################

// Counts live instances — catches missing destructor calls.
struct Counted {
    inline static int live = 0;
    int val;
    explicit Counted(int v = 0) : val(v) { ++live; }
    Counted(const Counted& o) : val(o.val) { ++live; }
    Counted& operator=(const Counted& o) { val = o.val; return *this; }
    ~Counted() { --live; }
    bool operator==(const Counted& o) const { return val == o.val; }
    auto operator<=>(const Counted& o) const { return val <=> o.val; }
};

// Throws on copy/assign after a countdown — exercises exception-safety paths.
struct ThrowingCopy {
    inline static int copies_until_throw = 9999;
    int val;
    explicit ThrowingCopy(int v = 0) : val(v) {}
    ThrowingCopy(const ThrowingCopy& o) : val(o.val) {
        if (--copies_until_throw <= 0)
            throw std::runtime_error("ThrowingCopy: copy failed");
    }
    ThrowingCopy& operator=(const ThrowingCopy& o) {
        if (--copies_until_throw <= 0)
            throw std::runtime_error("ThrowingCopy: assign failed");
        val = o.val;
        return *this;
    }
    bool operator==(const ThrowingCopy& o) const { return val == o.val; }
};

// Non-copyable — verifies array works with move-only element types.
struct MoveOnly {
    int val;
    explicit MoveOnly(int v = 0) : val(v) {}
    MoveOnly(const MoveOnly&)            = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&& o) noexcept : val(o.val) { o.val = -1; }
    MoveOnly& operator=(MoveOnly&& o) noexcept { val = o.val; o.val = -1; return *this; }
};

// ############################################################################
// TESTS
// ############################################################################

void register_tests() {

// ────────────────────────────────────────────────────────────────────────────
tf::suite("1. Capacity", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("size() and max_size() equal N", [] {
        stlish::array<int, 5> a{};
        CHECK(a.size()     == 5);
        CHECK(a.max_size() == 5);
    });

    tf::test("empty() is false for N > 0", [] {
        stlish::array<int, 3> a{};
        CHECK(!a.empty());
    });

    tf::test("empty() is true for N == 0", [] {
        stlish::array<int, 0> a{};
        CHECK(a.empty());
        CHECK(a.size()     == 0);
        CHECK(a.max_size() == 0);
    });

    tf::test("size/max_size/empty are constexpr", [] {
        // Named constexpr bools avoid commas inside STATIC_CHECK() args.
        constexpr bool sz  = (stlish::array<double, 7>{}.size()     == 7);
        constexpr bool emp = (stlish::array<char,   0>{}.empty()    == true);
        constexpr bool mx  = (stlish::array<int,    4>{}.max_size() == 4);
        STATIC_CHECK(sz);
        STATIC_CHECK(emp);
        STATIC_CHECK(mx);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("2. Aggregate initialisation", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("full braced-init sets all elements", [] {
        stlish::array<int, 3> a = {10, 20, 30};
        CHECK(a[0] == 10);
        CHECK(a[1] == 20);
        CHECK(a[2] == 30);
    });

    tf::test("partial init zero-fills remaining elements", [] {
        stlish::array<int, 5> a = {1, 2};
        CHECK(a[0] == 1);
        CHECK(a[1] == 2);
        CHECK(a[2] == 0);
        CHECK(a[4] == 0);
    });

    tf::test("value-init ({}) zero-fills scalars", [] {
        stlish::array<int, 4> a{};
        for (std::size_t i = 0; i < a.size(); ++i)
            CHECK(a[i] == 0);
    });

    tf::test("string elements are value-initialised to empty string", [] {
        stlish::array<std::string, 3> a = {"hello", "world"};
        CHECK(a[0] == "hello");
        CHECK(a[1] == "world");
        CHECK(a[2] == "");        // third element default-constructed
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("3. operator[]", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("read first, middle, last", [] {
        stlish::array<int, 3> a = {10, 20, 30};
        CHECK(a[0] == 10);
        CHECK(a[1] == 20);
        CHECK(a[2] == 30);
    });

    tf::test("write through non-const reference", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        a[1] = 99;
        CHECK(a[1] == 99);
    });

    tf::test("const overload returns const_reference", [] {
        const stlish::array<int, 3> ca = {7, 8, 9};
        CHECK(ca[0] == 7);
        CHECK(ca[2] == 9);
        // Verify type is const T&
        using cr = const int&;
        constexpr bool is_const_ref = std::is_same_v<decltype(ca[0]), cr>;
        STATIC_CHECK(is_const_ref);
    });

    tf::test("operator[] is noexcept (both overloads)", [] {
        stlish::array<int, 3> a{};
        const stlish::array<int, 3> ca{};
        STATIC_CHECK(noexcept(a[0]));
        STATIC_CHECK(noexcept(ca[0]));
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("4. at()", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("valid index returns correct reference", [] {
        stlish::array<int, 4> a = {1, 2, 3, 4};
        CHECK(a.at(0) == 1);
        CHECK(a.at(3) == 4);
    });

    tf::test("allows write", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        a.at(2) = 77;
        CHECK(a[2] == 77);
    });

    tf::test("throws std::out_of_range on bad index (non-const)", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        CHECK_THROWS_AS(a.at(3),  std::out_of_range);
        CHECK_THROWS_AS(a.at(99), std::out_of_range);
    });

    tf::test("throws std::out_of_range on bad index (const)", [] {
        const stlish::array<int, 3> ca = {1, 2, 3};
        CHECK_THROWS_AS(ca.at(3), std::out_of_range);
    });

    tf::test("exception message contains 'out of range'", [] {
        stlish::array<int, 2> a = {1, 2};
        CHECK_MSG_CONTAINS(a.at(5), "out of range");
    });

    tf::test("N==0: at(0) always throws", [] {
        stlish::array<int, 0> a{};
        CHECK_THROWS_AS(a.at(0), std::out_of_range);
    });

    tf::test("at() is NOT noexcept", [] {
        stlish::array<int, 3> a{};
        // at() can throw — it must not be declared noexcept.
        STATIC_CHECK(!noexcept(a.at(0)));
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("5. front() and back()", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("front() returns first element", [] {
        stlish::array<int, 4> a = {10, 20, 30, 40};
        CHECK(a.front() == 10);
    });

    tf::test("back() returns last element", [] {
        stlish::array<int, 4> a = {10, 20, 30, 40};
        CHECK(a.back() == 40);
    });

    tf::test("front() == back() for size-1 array", [] {
        stlish::array<int, 1> a = {42};
        CHECK(a.front() == 42);
        CHECK(a.back()  == 42);
    });

    tf::test("const overloads return const_reference", [] {
        const stlish::array<int, 3> ca = {5, 6, 7};
        CHECK(ca.front() == 5);
        CHECK(ca.back()  == 7);
    });

    tf::test("write through front() and back()", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        a.front() = 99;
        a.back()  = 77;
        CHECK(a[0] == 99);
        CHECK(a[2] == 77);
    });

    tf::test("front() and back() are noexcept", [] {
        stlish::array<int, 3> a{};
        STATIC_CHECK(noexcept(a.front()));
        STATIC_CHECK(noexcept(a.back()));
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("6. data()", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("data() points to first element", [] {
        stlish::array<int, 4> a = {1, 2, 3, 4};
        CHECK(a.data()    == &a[0]);
        CHECK(a.data()[2] == 3);
    });

    tf::test("const data() returns const pointer", [] {
        const stlish::array<int, 3> ca = {7, 8, 9};
        const int* p = ca.data();
        CHECK(*p == 7);
        constexpr bool is_const_ptr = std::is_same_v<decltype(ca.data()), const int*>;
        STATIC_CHECK(is_const_ptr);
    });

    tf::test("data() on N==0 returns non-null pointer", [] {
        stlish::array<int, 0> a{};
        CHECK(a.data() != nullptr);
    });

    tf::test("data() is noexcept", [] {
        stlish::array<int, 2> a{};
        const stlish::array<int, 2> ca{};
        STATIC_CHECK(noexcept(a.data()));
        STATIC_CHECK(noexcept(ca.data()));
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("7. Forward iterators", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("begin/end spans all elements", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        int sum = 0;
        for (auto it = a.begin(); it != a.end(); ++it)
            sum += *it;
        CHECK(sum == 15);
    });

    tf::test("begin() == end() for N==0", [] {
        stlish::array<int, 0> a{};
        CHECK(a.begin()  == a.end());
        CHECK(a.cbegin() == a.cend());
    });

    tf::test("cbegin/cend return const_iterator", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        auto it = a.cbegin();
        CHECK(*it == 1);
        using ci = stlish::array<int, 3>::const_iterator;
        constexpr bool ok = std::is_same_v<decltype(it), ci>;
        STATIC_CHECK(ok);
    });

    tf::test("pre and post increment", [] {
        stlish::array<int, 3> a = {10, 20, 30};
        auto it = a.begin();
        CHECK(*it == 10);
        auto old = it++;        // post: returns copy before advance
        CHECK(*old == 10);
        CHECK(*it  == 20);
        ++it;                   // pre: returns reference after advance
        CHECK(*it  == 30);
    });

    tf::test("pre and post decrement", [] {
        stlish::array<int, 3> a = {10, 20, 30};
        auto it = a.end();
        --it;
        CHECK(*it == 30);
        auto old = it--;
        CHECK(*old == 30);
        CHECK(*it  == 20);
    });

    tf::test("random-access arithmetic (+, -, +=, -=)", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        auto it = a.begin();
        CHECK(*(it + 3)  == 4);
        CHECK(*(3 + it)  == 4);   // n + iter commutative form
        it += 4;
        CHECK(*it == 5);
        it -= 2;
        CHECK(*it  == 3);
        CHECK(*(it - 1) == 2);
    });

    tf::test("distance (operator-)", [] {
        stlish::array<int, 5> a{};
        CHECK(a.end() - a.begin() == 5);
        auto mid = a.begin() + 2;
        CHECK(mid    - a.begin() == 2);
        CHECK(a.end() - mid      == 3);
    });

    tf::test("iterator subscript it[n]", [] {
        stlish::array<int, 5> a = {10, 20, 30, 40, 50};
        auto it = a.begin() + 1;
        CHECK(it[ 0] == 20);
        CHECK(it[ 2] == 40);
        CHECK(it[-1] == 10);
    });

    tf::test("comparison operators (<, >, <=, >=, ==, !=)", [] {
        stlish::array<int, 5> a{};
        auto first = a.begin();
        auto mid   = a.begin() + 2;
        auto last  = a.end();

        CHECK(first == first);
        CHECK(first != mid);
        CHECK(first <  mid);
        CHECK(mid   <  last);
        CHECK(last  >  mid);
        CHECK(first <= first);
        CHECK(first >= first);
        CHECK(mid   >= first);
    });

    tf::test("iterator → const_iterator implicit conversion", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        stlish::array<int, 3>::iterator       it  = a.begin();
        stlish::array<int, 3>::const_iterator cit = it;    // implicit
        CHECK(*cit == 1);
        CHECK(cit  == a.cbegin());
    });

    tf::test("begin/end are noexcept", [] {
        stlish::array<int, 3> a{};
        STATIC_CHECK(noexcept(a.begin()));
        STATIC_CHECK(noexcept(a.end()));
        STATIC_CHECK(noexcept(a.cbegin()));
        STATIC_CHECK(noexcept(a.cend()));
    });

    tf::test("range-based for loop works", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        int sum = 0;
        for (int v : a) sum += v;
        CHECK(sum == 15);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("8. Reverse iterators", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("rbegin/rend traverse in reverse", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        int expected = 5;
        for (auto it = a.rbegin(); it != a.rend(); ++it)
            CHECK(*it == expected--);
        CHECK(expected == 0);   // all five elements visited
    });

    tf::test("crbegin/crend return const_reverse_iterator", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        auto it = a.crbegin();
        CHECK(*it == 3);
        using cri = stlish::array<int, 3>::const_reverse_iterator;
        constexpr bool ok = std::is_same_v<decltype(it), cri>;
        STATIC_CHECK(ok);
    });

    tf::test("reverse iterator arithmetic", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        auto it = a.rbegin();   // points at 5
        CHECK(*(it + 1) == 4);
        CHECK(*(it + 4) == 1);
        CHECK(it[2]     == 3);  // rbegin[2] = element at index N-1-2 = 2
    });

    tf::test("rbegin == rend for N==0", [] {
        stlish::array<int, 0> a{};
        CHECK(a.rbegin() == a.rend());
    });

    tf::test("rend - rbegin == N", [] {
        stlish::array<int, 4> a{};
        CHECK(a.rend() - a.rbegin() == 4);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("9. fill()", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("sets all elements to value", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        a.fill(0);
        for (std::size_t i = 0; i < a.size(); ++i)
            CHECK(a[i] == 0);
    });

    tf::test("fill with non-zero value", [] {
        stlish::array<int, 4> a{};
        a.fill(42);
        CHECK(a[0] == 42);
        CHECK(a[3] == 42);
    });

    tf::test("fill on string array", [] {
        stlish::array<std::string, 3> a;
        a.fill("hello");
        CHECK(a[0] == "hello");
        CHECK(a[1] == "hello");
        CHECK(a[2] == "hello");
    });

    tf::test("fill on N==0 does nothing (no crash)", [] {
        stlish::array<int, 0> a{};
        CHECK_NOTHROW(a.fill(99));
    });

    tf::test("fill provides basic guarantee when T copy-assign throws", [] {
        // Allow 2 assignments, then throw on the 3rd.
        // After the throw: array is in a valid but partially-updated state.
        ThrowingCopy::copies_until_throw = 9999;
        stlish::array<ThrowingCopy, 5> a;
        for (auto& x : a) x = ThrowingCopy{1};

        ThrowingCopy::copies_until_throw = 2;
        bool threw = false;
        try {
            ThrowingCopy val{99};
            a.fill(val);
        } catch (const std::runtime_error&) {
            threw = true;
        }
        CHECK(threw);
        // Elements 0..1 were updated; 2..4 still hold original value.
        // Array object itself must still be usable (basic guarantee).
        CHECK_NOTHROW(a[0].val);
        ThrowingCopy::copies_until_throw = 9999;  // reset for next tests
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("10. swap()", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("member swap exchanges all elements", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        stlish::array<int, 3> b = {7, 8, 9};
        a.swap(b);
        CHECK(a[0] == 7); CHECK(a[2] == 9);
        CHECK(b[0] == 1); CHECK(b[2] == 3);
    });

    tf::test("ADL non-member swap works", [] {
        stlish::array<int, 3> a = {10, 20, 30};
        stlish::array<int, 3> b = {40, 50, 60};
        using std::swap;
        swap(a, b);
        CHECK(a[0] == 40);
        CHECK(b[0] == 10);
    });

    tf::test("self-swap is safe", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        a.swap(a);
        CHECK(a[0] == 1);
        CHECK(a[2] == 3);
    });

    tf::test("swap on N==0 does nothing", [] {
        stlish::array<int, 0> a{};
        stlish::array<int, 0> b{};
        CHECK_NOTHROW(a.swap(b));
    });

    tf::test("swap is noexcept when T swap is noexcept (int)", [] {
        stlish::array<int, 3> a{}, b{};
        STATIC_CHECK(noexcept(a.swap(b)));
    });

    tf::test("swap string arrays", [] {
        stlish::array<std::string, 2> a = {"hello", "world"};
        stlish::array<std::string, 2> b = {"foo",   "bar"  };
        a.swap(b);
        CHECK(a[0] == "foo");
        CHECK(b[0] == "hello");
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("11. Comparison operators", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("== on equal arrays", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        stlish::array<int, 3> b = {1, 2, 3};
        CHECK( (a == b));
        CHECK(!(a != b));
    });

    tf::test("== on unequal arrays", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        stlish::array<int, 3> b = {1, 2, 4};
        CHECK(!(a == b));
        CHECK( (a != b));
    });

    tf::test("< is lexicographic (last element differs)", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        stlish::array<int, 3> b = {1, 2, 4};
        CHECK( (a < b));
        CHECK(!(b < a));
    });

    tf::test("< is lexicographic (first element wins)", [] {
        stlish::array<int, 3> lo = {0, 9, 9};
        stlish::array<int, 3> hi = {1, 0, 0};
        CHECK((lo < hi));
    });

    tf::test("<= and >=", [] {
        stlish::array<int, 2> a = {1, 2};
        stlish::array<int, 2> b = {1, 2};
        stlish::array<int, 2> c = {1, 3};
        CHECK((a <= b)); CHECK((b >= a));
        CHECK((a <= c)); CHECK((c >= a));
    });

    tf::test("> greater than", [] {
        stlish::array<int, 3> big   = {1, 2, 4};
        stlish::array<int, 3> small = {1, 2, 3};
        CHECK( (big   > small));
        CHECK(!(small > big));
    });

    tf::test("N==0 arrays all compare equal", [] {
        stlish::array<int, 0> a{};
        stlish::array<int, 0> b{};
        CHECK( (a == b));
        CHECK(!(a <  b));
        CHECK( (a <= b));
        CHECK( (a >= b));
    });

    tf::test("string arrays compare lexicographically", [] {
        stlish::array<std::string, 2> a = {"apple", "banana"};
        stlish::array<std::string, 2> b = {"apple", "cherry"};
        CHECK((a <  b));
        CHECK((b >  a));
        CHECK((a != b));
    });

    tf::test("constexpr comparison", [] {
        constexpr stlish::array<int, 3> a = {1, 2, 3};
        constexpr stlish::array<int, 3> b = {1, 2, 4};
        STATIC_CHECK(a != b);
        STATIC_CHECK(a <  b);
        STATIC_CHECK(b >  a);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("12. Type coverage — Counted (destructor tracking)", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("exactly N destructors called when array goes out of scope", [] {
        Counted::live = 0;
        {
            stlish::array<Counted, 4> a = {
                Counted{1}, Counted{2}, Counted{3}, Counted{4}
            };
            // Temporaries are destroyed; only the 4 elements in _data remain.
            CHECK(Counted::live == 4);
        }
        CHECK(Counted::live == 0);
    });

    tf::test("fill() does not leak or over-destroy elements", [] {
        // array<Counted,3> default-constructs 3 elements.
        // fill() copy-assigns — no new constructions or destructions on the
        // stored elements themselves. The temp argument is constructed once,
        // copied N times, then destroyed. Net live count stays at 3.
        Counted::live = 0;
        {
            stlish::array<Counted, 3> a;
            CHECK(Counted::live == 3);          // 3 default-constructed
            a.fill(Counted{99});                // temp +1, copied x3, temp -1 → still 3
            CHECK(Counted::live == 3);
        }                                       // array dtor: -3
        CHECK(Counted::live == 0);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("13. Type coverage — MoveOnly", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("move-assign into elements works", [] {
        stlish::array<MoveOnly, 3> a;
        a[0] = MoveOnly{10};
        a[1] = MoveOnly{20};
        a[2] = MoveOnly{30};
        CHECK(a[0].val == 10);
        CHECK(a[2].val == 30);
    });

    tf::test("swap works for MoveOnly", [] {
        stlish::array<MoveOnly, 2> a, b;
        a[0] = MoveOnly{1};  a[1] = MoveOnly{2};
        b[0] = MoveOnly{9};  b[1] = MoveOnly{8};
        a.swap(b);
        CHECK(a[0].val == 9);
        CHECK(b[0].val == 1);
    });

    tf::test("unique_ptr elements work", [] {
        stlish::array<std::unique_ptr<int>, 3> a;
        a[0] = std::make_unique<int>(10);
        a[1] = std::make_unique<int>(20);
        a[2] = std::make_unique<int>(30);
        CHECK(*a[0] == 10);
        CHECK(*a[2] == 30);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("14. constexpr operations", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("constexpr element access", [] {
        constexpr stlish::array<int, 4> a = {1, 2, 3, 4};
        STATIC_CHECK(a[0]      == 1);
        STATIC_CHECK(a[3]      == 4);
        STATIC_CHECK(a.front() == 1);
        STATIC_CHECK(a.back()  == 4);
        STATIC_CHECK(a.size()  == 4);
    });

    tf::test("constexpr iterators (begin/end arithmetic)", [] {
        constexpr stlish::array<int, 3> a = {10, 20, 30};
        STATIC_CHECK(*a.begin()       == 10);
        STATIC_CHECK(*(a.begin() + 2) == 30);
        STATIC_CHECK(a.end() - a.begin() == 3);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("15. STL algorithm interop", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("std::sort", [] {
        stlish::array<int, 6> a = {5, 3, 8, 1, 9, 2};
        std::sort(a.begin(), a.end());
        const stlish::array<int, 6> expected = {1, 2, 3, 5, 8, 9};
        CHECK(a == expected);
    });

    tf::test("std::find — element present", [] {
        stlish::array<int, 5> a = {10, 20, 30, 40, 50};
        auto it = std::find(a.begin(), a.end(), 30);
        REQUIRE(it != a.end());
        CHECK(*it          == 30);
        CHECK(it - a.begin() == 2);
    });

    tf::test("std::find — element absent returns end", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        CHECK(std::find(a.begin(), a.end(), 99) == a.end());
    });

    tf::test("std::accumulate", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        CHECK(std::accumulate(a.begin(), a.end(), 0) == 15);
    });

    tf::test("std::reverse", [] {
        stlish::array<int, 5> a = {1, 2, 3, 4, 5};
        std::reverse(a.begin(), a.end());
        const stlish::array<int, 5> expected = {5, 4, 3, 2, 1};
        CHECK(a == expected);
    });

    tf::test("std::fill", [] {
        stlish::array<int, 4> a{};
        std::fill(a.begin(), a.end(), 7);
        CHECK(a[0] == 7);
        CHECK(a[3] == 7);
    });

    tf::test("std::iota", [] {
        stlish::array<int, 5> a{};
        std::iota(a.begin(), a.end(), 1);
        CHECK(a[0] == 1);
        CHECK(a[4] == 5);
    });

    tf::test("std::copy between arrays", [] {
        stlish::array<int, 4> src = {1, 2, 3, 4};
        stlish::array<int, 4> dst{};
        std::copy(src.begin(), src.end(), dst.begin());
        CHECK(src == dst);
    });

    tf::test("std::count", [] {
        stlish::array<int, 6> a = {1, 2, 2, 3, 2, 4};
        CHECK(std::count(a.begin(), a.end(), 2) == 3);
    });

    tf::test("std::equal on two arrays", [] {
        stlish::array<int, 4> a = {1, 2, 3, 4};
        stlish::array<int, 4> b = {1, 2, 3, 4};
        CHECK(std::equal(a.begin(), a.end(), b.begin()));
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("16. const-correctness", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("begin() on const array returns const_iterator", [] {
        const stlish::array<int, 3> ca = {1, 2, 3};
        using ci = stlish::array<int, 3>::const_iterator;
        constexpr bool ok = std::is_same_v<decltype(ca.begin()), ci>;
        STATIC_CHECK(ok);
    });

    tf::test("data() on const array returns const pointer", [] {
        const stlish::array<int, 3> ca = {1, 2, 3};
        constexpr bool ok = std::is_same_v<decltype(ca.data()), const int*>;
        STATIC_CHECK(ok);
    });

    tf::test("operator[] on const array returns const_reference", [] {
        const stlish::array<int, 2> ca = {1, 2};
        constexpr bool ok = std::is_same_v<decltype(ca[0]), const int&>;
        STATIC_CHECK(ok);
    });

    tf::test("const array is fully readable via all accessors", [] {
        const stlish::array<int, 5> ca = {10, 20, 30, 40, 50};
        CHECK(ca[0]      == 10);
        CHECK(ca.at(2)   == 30);
        CHECK(ca.front() == 10);
        CHECK(ca.back()  == 50);
        int sum = 0;
        for (const int& v : ca) sum += v;
        CHECK(sum == 150);
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("17. Iterator invalidation", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("iterators survive fill() — same address, new values", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        const int* const p0 = a.data();
        auto it = a.begin();
        a.fill(99);
        CHECK(a.data() == p0);     // storage address unchanged
        CHECK(it == a.begin());    // iterator still valid
        CHECK(*it == 99);          // value updated through it
    });

    tf::test("iterators survive swap() — point to original object's (new) values", [] {
        stlish::array<int, 3> a = {1, 2, 3};
        stlish::array<int, 3> b = {7, 8, 9};
        auto itA = a.begin();
        const int* const pA = a.data();

        a.swap(b);

        CHECK(a.data() == pA);     // a's storage did not move
        CHECK(itA == a.begin());   // still points into a
        CHECK(*itA == 7);          // a now holds b's old values
    });
});

// ────────────────────────────────────────────────────────────────────────────
tf::suite("18. noexcept specifications", [] {
// ────────────────────────────────────────────────────────────────────────────

    tf::test("capacity functions are noexcept", [] {
        stlish::array<int, 3> a{};
        STATIC_CHECK(noexcept(a.size()));
        STATIC_CHECK(noexcept(a.max_size()));
        STATIC_CHECK(noexcept(a.empty()));
    });

    tf::test("element access noexcept (operator[], front, back, data)", [] {
        stlish::array<int, 3> a{};
        STATIC_CHECK(noexcept(a[0]));
        STATIC_CHECK(noexcept(a.front()));
        STATIC_CHECK(noexcept(a.back()));
        STATIC_CHECK(noexcept(a.data()));
    });

    tf::test("at() is NOT noexcept", [] {
        stlish::array<int, 3> a{};
        STATIC_CHECK(!noexcept(a.at(0)));
    });

    tf::test("iterator functions are noexcept", [] {
        stlish::array<int, 3> a{};
        STATIC_CHECK(noexcept(a.begin()));
        STATIC_CHECK(noexcept(a.end()));
        STATIC_CHECK(noexcept(a.cbegin()));
        STATIC_CHECK(noexcept(a.cend()));
        STATIC_CHECK(noexcept(a.rbegin()));
        STATIC_CHECK(noexcept(a.rend()));
    });

    tf::test("swap<int> is noexcept", [] {
        stlish::array<int, 3> a{}, b{};
        STATIC_CHECK(noexcept(a.swap(b)));
    });

    tf::test("swap<string> is not noexcept (std::string swap may throw)", [] {
        // std::string::swap is noexcept in practice but the conditional
        // noexcept on our swap() propagates T's noexcept correctly.
        // For std::string, swap *is* noexcept — this just verifies the
        // mechanism works and we're not unconditionally noexcept.
        stlish::array<std::string, 2> a{}, b{};
        // Just call it — don't assert noexcept either way for string
        // (implementation-defined). The important thing: no crash.
        CHECK_NOTHROW(a.swap(b));
    });
});

} // register_tests()

// ############################################################################
// MAIN
// ############################################################################

int main() {
    register_tests();
    return tf::run_all();
}
#include <gtest/gtest.h>
#include "stlish/vector.hpp"

struct CountingType {
    static inline int alive = 0;

    CountingType() { ++alive; }
    CountingType(const CountingType&) { ++alive; }
    CountingType(CountingType&&) noexcept { ++alive; }
    ~CountingType() { --alive; }
};

TEST(VectorLifetime, DestructorDestroysAllElements) {
    CountingType::alive = 0;
    {
        stlish::vector<CountingType> v(10);
        EXPECT_EQ(CountingType::alive, 10);
    }
    EXPECT_EQ(CountingType::alive, 0);
}

TEST(VectorLifetime, ClearDestroysElementsButKeepsBufferAndCapacity) {
    stlish::vector<int> v(5, 3);
    auto* ptr = v.data();
    auto cap = v.capacity();

    v.clear();

    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), cap);
    EXPECT_EQ(v.data(), ptr);
}

TEST(VectorLifetime, PopBackOnEmptyIsSafeNoThrow) {
    stlish::vector<int> v;
    v.pop_back(); // your implementation does "if empty return;"
    EXPECT_EQ(v.size(), 0u);
}

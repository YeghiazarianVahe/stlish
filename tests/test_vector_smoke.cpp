#include <gtest/gtest.h>
#include "stlish/vector.hpp"

TEST(VectorSmoke, DefaultConstructedIsEmptyAndValid) {
    stlish::vector<int> v;

    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.data(), nullptr);

    EXPECT_EQ(v.begin(), v.end());
    EXPECT_EQ(v.cbegin(), v.cend());
}

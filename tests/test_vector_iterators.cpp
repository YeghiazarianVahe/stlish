#include <gtest/gtest.h>
#include "stlish/vector.hpp"

TEST(VectorIterators, EmptyBeginEqualsEnd) {
    stlish::vector<int> v;
    EXPECT_EQ(v.begin(), v.end());
    EXPECT_EQ(v.cbegin(), v.cend());
}

TEST(VectorIterators, BeginEndMatchDataAndSize) {
    stlish::vector<int> v{1, 2, 3};

    EXPECT_EQ(v.begin(), v.data());
    EXPECT_EQ(v.end(), v.data() + v.size());

    EXPECT_EQ(v.cbegin(), v.data());
    EXPECT_EQ(v.cend(), v.data() + v.size());
}

TEST(VectorIterators, IterateAndSum) {
    stlish::vector<int> v{1, 2, 3, 4};

    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 10);
}

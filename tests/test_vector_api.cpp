#include <gtest/gtest.h>
#include "stlish/vector.hpp"

TEST(VectorAPI, OperatorBracketAndAt) {
    stlish::vector<int> v{10, 20, 30};

    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);

    EXPECT_EQ(v.at(0), 10);
    EXPECT_EQ(v.at(2), 30);
}

TEST(VectorAPI, FrontBack) {
    stlish::vector<int> v{7, 8, 9};
    EXPECT_EQ(v.front(), 7);
    EXPECT_EQ(v.back(), 9);
}

TEST(VectorAPI, EmplaceBackAndPushBackWork) {
    stlish::vector<int> v;

    v.emplace_back(1);
    v.push_back(2);
    v.push_back(3);

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(VectorAPI, ReserveGrowsCapacityAndKeepsValues) {
    stlish::vector<int> v{1, 2, 3};

    const auto old_cap = v.capacity();
    v.reserve(old_cap + 10);

    EXPECT_GE(v.capacity(), old_cap + 10);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

#include <gtest/gtest.h>
#include "stlish/vector.hpp"

TEST(VectorInvariants, SizeNeverExceedsCapacity) {
    stlish::vector<int> v;
    EXPECT_LE(v.size(), v.capacity());

    v.emplace_back(1);
    EXPECT_LE(v.size(), v.capacity());

    v.emplace_back(2);
    v.emplace_back(3);
    EXPECT_LE(v.size(), v.capacity());

    v.reserve(64);
    EXPECT_LE(v.size(), v.capacity());
}

TEST(VectorInvariants, CopyAssignmentDeepCopies) {
    stlish::vector<int> a{1, 2, 3};
    stlish::vector<int> b{9};

    b = a;

    EXPECT_EQ(b.size(), 3u);
    EXPECT_NE(b.data(), a.data());
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
}

TEST(VectorInvariants, MoveAssignmentLeavesSourceEmptyValid) {
    stlish::vector<int> a{1, 2, 3};
    stlish::vector<int> b{9, 9};

    auto* old_ptr = a.data();
    b = std::move(a);

    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.data(), old_ptr);

    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(a.capacity(), 0u);
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.data(), nullptr);
}

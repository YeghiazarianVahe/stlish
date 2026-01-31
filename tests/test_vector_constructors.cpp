#include <gtest/gtest.h>
#include "stlish/vector.hpp"

TEST(VectorCtor, DefaultIsEmpty) {
    stlish::vector<int> v;
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.data(), nullptr);
    EXPECT_EQ(v.begin(), v.end());
}

TEST(VectorCtor, SizeCtorCreatesNDefaultElements) {
    stlish::vector<int> v(5);
    EXPECT_EQ(v.size(), 5u);
    EXPECT_GE(v.capacity(), 5u);
    EXPECT_FALSE(v.empty());
    EXPECT_NE(v.data(), nullptr);
}

TEST(VectorCtor, FillCtorCopiesValue) {
    stlish::vector<int> v(4, 7);
    EXPECT_EQ(v.size(), 4u);
    EXPECT_GE(v.capacity(), 4u);
    ASSERT_NE(v.data(), nullptr);
    for (std::size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v.data()[i], 7);
    }
}

TEST(VectorCtor, CopyCtorDeepCopies) {
    stlish::vector<int> a(3, 9);
    stlish::vector<int> b(a);

    EXPECT_EQ(b.size(), a.size());
    ASSERT_NE(a.data(), nullptr);
    ASSERT_NE(b.data(), nullptr);

    // Buffers should be different (deep copy)
    EXPECT_NE(b.data(), a.data());

    for (std::size_t i = 0; i < a.size(); ++i) {
        EXPECT_EQ(b.data()[i], a.data()[i]);
    }
}

TEST(VectorCtor, MoveCtorStealsAndEmptiesSource) {
    stlish::vector<int> a(3, 1);
    auto* old = a.data();

    stlish::vector<int> b(std::move(a));
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.data(), old);

    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(a.capacity(), 0u);
    EXPECT_EQ(a.data(), nullptr);
    EXPECT_TRUE(a.empty());
}

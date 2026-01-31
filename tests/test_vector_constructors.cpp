#include <gtest/gtest.h>
#include "stlish/vector.hpp"

TEST(VectorCtors, SizeCtorCreatesNDefaultElements) {
    stlish::vector<int> v(5);
    EXPECT_EQ(v.size(), 5u);
    EXPECT_GE(v.capacity(), 5u);
    EXPECT_FALSE(v.empty());
    EXPECT_NE(v.data(), nullptr);
}

TEST(VectorCtors, FillCtorCopiesValue) {
    stlish::vector<int> v(4, 7);
    EXPECT_EQ(v.size(), 4u);
    ASSERT_NE(v.data(), nullptr);

    for (std::size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], 7);
    }
}

TEST(VectorCtors, CopyCtorDeepCopiesBuffer) {
    stlish::vector<int> a(3, 9);
    stlish::vector<int> b(a);

    EXPECT_EQ(b.size(), a.size());
    ASSERT_NE(a.data(), nullptr);
    ASSERT_NE(b.data(), nullptr);
    EXPECT_NE(b.data(), a.data());

    for (std::size_t i = 0; i < a.size(); ++i) {
        EXPECT_EQ(b[i], a[i]);
    }
}

TEST(VectorCtors, MoveCtorStealsAndLeavesSourceEmptyValid) {
    stlish::vector<int> a(3, 1);
    auto* old_ptr = a.data();
    auto old_cap = a.capacity();

    stlish::vector<int> b(std::move(a));

    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.capacity(), old_cap);
    EXPECT_EQ(b.data(), old_ptr);

    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(a.capacity(), 0u);
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.data(), nullptr);
}

TEST(VectorCtors, RangeCtorFromPointerRange) {
    int arr[] = {1, 2, 3, 4};
    stlish::vector<int> v(arr, arr + 4);

    EXPECT_EQ(v.size(), 4u);
    ASSERT_NE(v.data(), nullptr);

    for (std::size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], arr[i]);
    }
}

TEST(VectorCtors, InitListCtor) {
    stlish::vector<int> v{5, 6, 7};

    EXPECT_EQ(v.size(), 3u);
    ASSERT_NE(v.data(), nullptr);

    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[1], 6);
    EXPECT_EQ(v[2], 7);
}

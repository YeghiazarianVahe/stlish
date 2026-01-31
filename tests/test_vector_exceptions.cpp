#include <gtest/gtest.h>
#include <stdexcept>
#include "stlish/vector.hpp"

TEST(VectorExceptions, AtThrowsOutOfRange) {
    stlish::vector<int> v{1, 2, 3};

    EXPECT_THROW((void)v.at(3), std::out_of_range);
    EXPECT_THROW((void)v.at(100), std::out_of_range);
}

TEST(VectorExceptions, AtDoesNotThrowInRange) {
    stlish::vector<int> v{1, 2, 3};

    EXPECT_NO_THROW((void)v.at(0));
    EXPECT_NO_THROW((void)v.at(2));
}

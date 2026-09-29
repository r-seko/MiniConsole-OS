#include <gtest/gtest.h>

TEST(SystemSanityTest, BasicAssertion) {
    EXPECT_EQ(1 + 1, 2);
}
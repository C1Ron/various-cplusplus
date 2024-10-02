#include <gtest/gtest.h>
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Excpect equality
    EXPECT_EQ(7 * 6, 42);
}
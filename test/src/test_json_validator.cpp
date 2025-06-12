#include <gtest/gtest.h>
#include "mock_rule.h"
#include "validator/json_validator.h"

class TestJsonValidator : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {

    }
};

TEST_F(TestJsonValidator, FirstTest) {
    EXPECT_EQ(1, 1);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
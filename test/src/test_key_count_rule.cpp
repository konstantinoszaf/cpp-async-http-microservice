#include <gtest/gtest.h>
#include "infra/validator/key_count_rule.h"

class TestKeyCountRule : public ::testing::Test {
protected:
    json::object obj;
    std::unique_ptr<KeyCountRule> rule;
    void SetUp() override {
        rule = std::make_unique<KeyCountRule>(1, 2);
    }

    void TearDown() override {
        obj = {};
    }
};

TEST_F(TestKeyCountRule, ValidateInvalidObjectWithInputsFromConstructor) {
    std::string expected_err = "JSON must contain at least 1 key, and maximum 2";
    std::string actual_err;
    bool actual = rule->is_valid(obj, actual_err);
    EXPECT_FALSE(actual);
    EXPECT_EQ(actual_err, expected_err);
}

TEST_F(TestKeyCountRule, ValidInputJsonValidation) {
    std::string err;
    obj["key"] = "val";

    bool rc = rule->is_valid(obj, err);
    EXPECT_TRUE(rc);
    EXPECT_TRUE(err.empty());
}

class TestInvalidKeyCountParametrized : public TestKeyCountRule, public ::testing::WithParamInterface<int> {};

INSTANTIATE_TEST_SUITE_P(
    InvalidCounts,
    TestInvalidKeyCountParametrized,
    ::testing::Values(0, 3)
);

TEST_P(TestInvalidKeyCountParametrized, InvalidKeyCount) {
    int count = GetParam();

    for (int i = 0; i < count; ++i) {
        obj[std::string("key") + std::to_string(i)] = i;
    }

    std::string err;

    bool rc = rule->is_valid(obj, err);
    EXPECT_FALSE(rc);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
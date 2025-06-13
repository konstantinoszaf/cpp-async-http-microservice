#include <gtest/gtest.h>
#include <memory>
#include "mock_rule.h"
#include "validator/json_validator.h"
#include "exception/validation_exception.h"

using ::testing::Return;

class TestJsonValidator : public ::testing::Test {
protected:
    std::unique_ptr<JsonValidator> validator;

    void SetUp() override {
        validator = std::make_unique<JsonValidator>();
    }

    void TearDown() override {

    }
};

TEST_F(TestJsonValidator, testAddRule) {
    std::unique_ptr<MockRule> mock_rule = std::make_unique<MockRule>();
    validator->add_rule(std::move(mock_rule));

    EXPECT_EQ(validator->rule_count(), 1);
}

TEST_F(TestJsonValidator, testValidateValidRuleAndObject) {
    std::unique_ptr<MockRule> mock_rule = std::make_unique<MockRule>();
    EXPECT_CALL(*mock_rule, is_valid(::testing::_, ::testing::_)).WillOnce(Return(true));
    validator->add_rule(std::move(mock_rule));

    json::object obj {};
    obj.emplace("some", "value");

    validator->validate(obj);
}

TEST_F(TestJsonValidator, testValidationOnEmptyObjectInput) {
    json::object obj {};

    EXPECT_THROW(validator->validate(obj), URLShortener::exception::ValidationException);
}

TEST_F(TestJsonValidator, testValidationOnInvalidRule) {
    std::unique_ptr<MockRule> mock_rule = std::make_unique<MockRule>();
    EXPECT_CALL(*mock_rule, is_valid(::testing::_, ::testing::_)).WillOnce(Return(false));
    validator->add_rule(std::move(mock_rule));

    json::object obj {};
    obj.emplace("some", "value");

    EXPECT_THROW(validator->validate(obj), URLShortener::exception::ValidationException);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
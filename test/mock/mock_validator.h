#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/validator_interface.h"
#include "mock_rule.h"

class MockValidator : public IValidator {
public:
    MOCK_METHOD(void, validate, (const json::object& j), (override));
    MOCK_METHOD(void, add_rule, (std::unique_ptr<IValidationRule> rule), (override));
};
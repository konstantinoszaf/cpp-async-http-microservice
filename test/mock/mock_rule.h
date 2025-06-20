#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/rule_interface.h"

class MockRule : public IValidationRule {
public:
    MOCK_METHOD(bool, is_valid, (const json::object& obj, std::string& error), (override));
};
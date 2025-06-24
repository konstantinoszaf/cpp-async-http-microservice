#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/parser_interface.h"

class MockParser : public IParser {
public:
    MOCK_METHOD((std::pair<std::string, ProviderType>), parse, (std::string_view), (override));
};
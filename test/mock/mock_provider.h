#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/provider_interface.h"

class MockProvider : public IProvider {
public:
    MOCK_METHOD(async_task<std::string>, shorten, (std::string_view), (override));
    MOCK_METHOD(std::string, create_request_body, (std::string_view), (override));
    MOCK_METHOD(std::string, get_short_url, (std::string_view), (override));
    MOCK_METHOD(std::string, get_error_message, (std::string_view), (override));
};
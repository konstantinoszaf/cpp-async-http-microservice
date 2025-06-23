#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/http_client_interface.h"

class MockHttpClient : public IHttpClient {
public:
    MOCK_METHOD(async_task<Response>, post, (std::string_view body, RequestInfo& data), (override));
};

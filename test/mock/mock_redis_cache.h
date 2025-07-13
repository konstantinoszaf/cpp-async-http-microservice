#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/cache_client_interface.h"
#include "core/ports/task.h"

class MockCache : public ICacheClient {
public:
  MOCK_METHOD(async_task<std::optional<std::string>>, get, (std::string_view), (override));
  MOCK_METHOD(async_task<void>, set, (std::string_view, std::string_view, int), (override));
};

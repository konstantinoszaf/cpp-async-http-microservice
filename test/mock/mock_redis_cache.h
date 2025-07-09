#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/cache_client_interface.h"

class MockCache : public ICacheClient {
public:
  MOCK_METHOD(std::optional<std::string>, get,
              (std::string_view), (override));
  MOCK_METHOD(void, set,
              (std::string_view, std::string_view, int), (override));
};

#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/ports/env_reader_interface.h"

class MockEnvReader : public IEnvReader {
public:
    MOCK_METHOD(std::string, get, (std::string_view), (override));
};
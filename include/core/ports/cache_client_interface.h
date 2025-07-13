#pragma once
#include <string>
#include <string_view>
#include <optional>
#include "core/ports/task.h"

class ICacheClient {
public:
    virtual ~ICacheClient() = default;
    virtual async_task<std::optional<std::string>> get(std::string_view key) = 0;
    virtual async_task<void> set(std::string_view key, std::string_view value, int ttl_seconds = 3600) = 0;
};
#pragma once
#include <string>
#include <string_view>
#include <optional>

class ICacheClient {
public:
    virtual ~ICacheClient() = default;
    virtual std::optional<std::string> get(std::string_view key) = 0;
    virtual void set(std::string_view key, std::string_view value, int ttl_seconds = 3600) = 0;
};
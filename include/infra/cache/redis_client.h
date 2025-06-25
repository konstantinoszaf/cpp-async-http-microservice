#pragma once
#include "core/ports/cache_client_interface.h"
#include <sw/redis++/redis++.h>

namespace Cache{
class Redis : public ICacheClient {
    public:
    Redis(const std::string& uri);
    std::optional<std::string> get(std::string_view key) override;
    void set(std::string_view key, std::string_view value, int ttl) override;
private:
    sw::redis::Redis redis_client;
};
} // namespace Cache
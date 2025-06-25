#include "infra/cache/redis_client.h"

using namespace Cache;

Redis::Redis(const std::string& uri)
: redis_client(uri) {}

std::optional<std::string> Redis::get(std::string_view key) {
    if (key.empty()) return std::nullopt;

    sw::redis::StringView redis_key{ key.data(), key.size() };

    auto result = redis_client.get(redis_key);
    return result ? std::optional<std::string>(*result) : std::nullopt;
}

void Redis::set(std::string_view key, std::string_view value, int ttl) {
    if (key.empty() || value.empty()) return;

    if (ttl <= 0) ttl = 3600;

    sw::redis::StringView redis_key{ key.data(), key.size() };
    sw::redis::StringView redis_val{ value.data(), value.size() };

    redis_client.set(redis_key, redis_val, ttl, sw::redis::UpdateType::ALWAYS);
}
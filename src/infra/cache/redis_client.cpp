#include "infra/cache/redis_client.h"

Cache::Redis::Redis(const std::string &host, int port)
    : redis_client([&] {
        sw::redis::ConnectionOptions opts;
        opts.host = host;
        opts.port = port;
        sw::redis::ConnectionPoolOptions pool_opts;
        return sw::redis::Redis(opts, pool_opts);
    }())
{}

std::optional<std::string> Cache::Redis::get(std::string_view key) {
    if (key.empty()) return std::nullopt;

    sw::redis::StringView redis_key{ key.data(), key.size() };

    auto result = redis_client.get(redis_key);
    return result ? std::optional<std::string>(*result) : std::nullopt;
}

void Cache::Redis::set(std::string_view key, std::string_view value, int ttl) {
    if (key.empty() || value.empty()) return;

    if (ttl <= 0) ttl = 3600;

    sw::redis::StringView redis_key{ key.data(), key.size() };
    sw::redis::StringView redis_val{ value.data(), value.size() };

    redis_client.set(redis_key, redis_val, ttl, sw::redis::UpdateType::ALWAYS);
}
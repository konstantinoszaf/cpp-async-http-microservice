#include "infra/cache/redis_client.h"
#include <boost/redis.hpp>
#include <boost/redis/src.hpp>
#include <boost/asio.hpp>

namespace asio = boost::asio;
namespace redis = boost::redis;

Cache::Redis::Redis(asio::io_context &ioc, std::string host, std::string port)
  : conn_(std::make_shared<redis::connection>(ioc.get_executor())) {
    asio::co_spawn(ioc,
        [conn = conn_,
        host = std::move(host),
        port = std::move(port)]() mutable -> asio::awaitable<void> {
        redis::config cfg;
        cfg.addr.host = std::move(host);
        cfg.addr.port = std::move(port);
        co_await conn->async_run(cfg,
            redis::logger{redis::logger::level::err},
            asio::use_awaitable);
        },
        asio::detached);
}


async_task<std::optional<std::string>> Cache::Redis::get(std::string_view key) {
    if (key.empty()) co_return std::nullopt;

    redis::request req;
    req.push("GET", key);

    redis::response<std::optional<std::string>> resp;

    co_await conn_->async_exec(req, resp);

    co_return std::move(*std::get<0>(resp));
}

async_task<void> Cache::Redis::set(std::string_view key, std::string_view value, int ttl) {
    if (key.empty() || value.empty()) co_return;

    if (ttl <= 0) ttl = 3600;

    redis::request req;
    req.push("SET", key, value, "EX", std::to_string(ttl));

    redis::response<redis::ignore_t> resp;
    co_await conn_->async_exec(req, resp);
}

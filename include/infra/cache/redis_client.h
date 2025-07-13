#include "core/ports/cache_client_interface.h"
#include <boost/redis.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace Cache {
class Redis : public ICacheClient {
public:
    Redis(boost::asio::io_context& ioc, std::string host, std::string port);

    async_task<std::optional<std::string>> get(std::string_view key) override;
    async_task<void> set(std::string_view key,
        std::string_view value,
        int ttl = 3600) override;

private:
    std::shared_ptr<boost::redis::connection> conn_;
};
}; // namespace Cache
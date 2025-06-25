#include "infra/network/dns_resolver.h"

async_task<Endpoints> DnsResolver::resolve(std::string_view host, std::string_view port)
{
    auto ex = co_await asio::this_coro::executor;

    boost::system::error_code ec;
    tcp::resolver resolver(ex);

    auto results = co_await resolver.async_resolve(
        std::string(host),
        std::string(port),
        asio::redirect_error(asio::use_awaitable, ec));

    Endpoints eps;
    if (ec) {
        std::cout << "DNS resolve failed\n"
                    << "  host    = " << host << "\n"
                    << "  port    = " << port << "\n"
                    << "  code    = " << ec.value() << "\n"
                    << "  category= " << ec.category().name() << "\n"
                    << "  message = " << ec.message() << "\n";
        co_return eps;
    }

    for (auto const& entry : results) {
        auto ep = entry.endpoint();
        eps.push_back({
            ep.address().to_string(),
            static_cast<uint16_t>(ep.port())
        });
    }

    co_return eps;
}
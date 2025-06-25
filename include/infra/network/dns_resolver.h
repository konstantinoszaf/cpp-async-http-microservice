#pragma once
#include "core/ports/dns_resolver_interface.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <iostream>
#include <string_view>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class DnsResolver : public IDnsResolver {
public:
    async_task<Endpoints> resolve(std::string_view host, std::string_view port) override;
};
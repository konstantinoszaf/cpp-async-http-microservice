#pragma once
#include "core/domain/types.h"
#include "core/ports/http_client_interface.h"
#include "core/ports/dns_cache_interface.h"
#include "core/ports/dns_resolver_interface.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/awaitable.hpp>
#include <memory>

class HttpClient : public IHttpClient {
public:
    HttpClient(boost::asio::io_context& io_ctx_,
                boost::asio::ssl::context& ssl_ctx_,
                std::unique_ptr<IDnsCache> dns_cache_,
                std::unique_ptr<IDnsResolver> dns_resolver_);
    async_task<Response> post(std::string_view payload, RequestInfo& config) override;
private:
    Response bad_response(std::string_view msg, HTTP::code code);
    async_task<Endpoints> lookup_or_resolve(std::string_view host, std::string_view port);

private:
    boost::asio::io_context& io_ctx;
    boost::asio::ssl::context& ssl_ctx;
    std::unique_ptr<IDnsCache> dns_cache;
    std::unique_ptr<IDnsResolver> dns_resolver;
};
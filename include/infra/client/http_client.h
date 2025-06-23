#pragma once
#include "core/domain/types.h"
#include "core/ports/http_client_interface.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/awaitable.hpp>
#include <memory>

class HttpClient : public IHttpClient {
public:
    HttpClient(boost::asio::io_context& io_ctx_, boost::asio::ssl::context& ssl_ctx_);
    async_task<Response> post(std::string_view payload, RequestInfo& config) override;
private:
    Response bad_response(std::string_view msg, HTTP::code code);
private:
    boost::asio::io_context& io_ctx;
    boost::asio::ssl::context& ssl_ctx;
};
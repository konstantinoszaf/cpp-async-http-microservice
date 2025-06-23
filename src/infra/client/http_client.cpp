#include "adapters/outgoing/boost_http_adapter.h"
#include "infra/client/http_client.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/redirect_error.hpp>
extern "C" {
    #include <openssl/ssl.h>
}
#include <chrono>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

HttpClient::HttpClient(boost::asio::io_context& io_ctx_, boost::asio::ssl::context& ssl_ctx_)
: io_ctx{io_ctx_}, ssl_ctx{ssl_ctx_} {
    ssl_ctx.set_default_verify_paths();
}

async_task<Response> HttpClient::post(std::string_view payload, RequestInfo& config) {
    auto ex = co_await asio::this_coro::executor;

    boost::system::error_code ec;
    tcp::resolver resolver(asio::make_strand(ex));

    auto const results = co_await resolver.async_resolve(config.host, config.port, asio::redirect_error(asio::use_awaitable, ec));

    if (ec) {
        std::cout << "DNS resolve failed\n"
                    << "  host    = " << config.host << "\n"
                    << "  port    = " << config.port << "\n"
                    << "  code    = " << ec.value() << "\n"
                    << "  category= " << ec.category().name() << "\n"
                    << "  message = " << ec.message() << "\n";
        co_return bad_response(ec.message(), HTTP::code::ServiceUnavailable);
    }

    beast::ssl_stream<beast::tcp_stream> stream(asio::make_strand(ex), ssl_ctx);
    stream.set_verify_mode(ssl::verify_peer);
    stream.set_verify_callback(ssl::rfc2818_verification(config.host));

    if (!SSL_set_tlsext_host_name(stream.native_handle(), config.host.c_str())) {
        std::cout << "Got in, host is: " << config.host << '\n';

        boost::system::error_code ec{static_cast<int>(::ERR_get_error()),
                                    asio::error::get_ssl_category()};
        co_return bad_response(ec.message(), HTTP::code::InternalServerError);
    }

    auto& tcp_stream  = stream.next_layer();
    co_await asio::async_connect(
        tcp_stream.socket(),
        results,
        asio::redirect_error(asio::use_awaitable, ec)
    );

    if (ec) {
        std::cout << "Connecting failed " << ec.message() << "\n";
        co_return bad_response(ec.message(), HTTP::code::ServiceUnavailable);
    }

    co_await stream.async_handshake(ssl::stream_base::client, asio::redirect_error(asio::use_awaitable, ec));

    if (ec) {
        std::cout << "Handshake failed.\n";
        co_return bad_response(ec.message(), HTTP::code::BadGateway);
    }

    http::request<http::string_body> req{http::verb::post, config.endpoint, 11};
    req.set(http::field::host, config.host);
    req.set(http::field::accept, "application/json");
    req.set(http::field::content_type, "application/json");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    if (!config.authorization_token.empty()) {
        req.set(http::field::authorization, config.authorization_token);
    }

    req.body() = payload;
    req.prepare_payload();

    co_await http::async_write(stream, req, asio::redirect_error(asio::use_awaitable, ec));

    if (ec) {
        std::cout << "Could not write\n";
        co_return bad_response(ec.message(), HTTP::code::BadGateway);
    }

    beast::flat_buffer buffer;
    http::response<http::string_body> res;

    co_await http::async_read(stream, buffer, res, asio::redirect_error(asio::use_awaitable, ec));

    if (ec && ec != http::error::end_of_stream) {
        std::cout << "Could not read\n";
        co_return bad_response(ec.message(), HTTP::code::BadGateway);
    }

    co_await stream.async_shutdown(asio::redirect_error(asio::use_awaitable, ec));

    if (ec && ec != asio::error::eof && ec != boost::asio::ssl::error::stream_truncated) {
        std::cout << "Could not shut down\n";
        co_return bad_response(res.body(),
                            static_cast<HTTP::code>(res.result_int()));
    }

    co_return BoostHttpAdapter::from_boost(res);
}

Response HttpClient::bad_response(std::string_view msg, HTTP::code code) {
    return Response{code, {{"Content-Type","text/plain"}}, std::string{msg}};
}
#include "adapters/outgoing/boost_http_adapter.h"
#include "infra/client/http_client.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/beast/http/serializer.hpp>
extern "C" {
    #include <openssl/ssl.h>
}
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

HttpClient::HttpClient(boost::asio::io_context& io_ctx_,
                        boost::asio::ssl::context& ssl_ctx_,
                        std::shared_ptr<IDnsCache> dns_cache_)
: io_ctx{io_ctx_}, ssl_ctx{ssl_ctx_}, dns_cache{dns_cache_} {
    ssl_ctx.set_default_verify_paths();
}

async_task<Response> HttpClient::post(std::string_view payload, RequestInfo& config) {
    auto ex = co_await asio::this_coro::executor;

    boost::system::error_code ec;

    auto results = co_await dns_cache->resolve(config.host, config.port);
    if (results.empty()) {
        co_return bad_response("DNS resolve failed", HTTP::code::ServiceUnavailable);
    }

    std::vector<tcp::endpoint> native_res;
    native_res.reserve(results.size());
    for (auto const& r : results) {
        auto addr = boost::asio::ip::make_address(r.address, ec);
        if (ec) {
            continue;
        }
        native_res.emplace_back(addr, r.port);
    }
    if (native_res.empty()) {
        co_return bad_response("No valid IPs", HTTP::code::ServiceUnavailable);
    }

    beast::ssl_stream<beast::tcp_stream> stream(asio::make_strand(ex), ssl_ctx);
    stream.set_verify_mode(ssl::verify_peer);
    stream.set_verify_callback(asio::ssl::host_name_verification(config.host));

    if (!SSL_set_tlsext_host_name(stream.native_handle(), config.host.c_str())) {
        std::cout << "Got in, host is: " << config.host << '\n';

        boost::system::error_code ec{static_cast<int>(::ERR_get_error()),
                                    asio::error::get_ssl_category()};
        co_return bad_response(ec.message(), HTTP::code::InternalServerError);
    }

    auto& tcp_stream  = stream.next_layer();
    co_await asio::async_connect(
        tcp_stream.socket(),
        native_res,
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

    http::request<http::dynamic_body> req{http::verb::post, config.endpoint, 11};
    req.set(http::field::host, config.host);
    req.set(http::field::accept, "application/json");
    req.set(http::field::content_type, "application/json");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    if (!config.authorization_token.empty()) {
        req.set(http::field::authorization, config.authorization_token);
    }

    {
        auto os = boost::beast::ostream(req.body());
        os << payload;
    }

    req.prepare_payload();
    tcp_stream.socket().set_option(boost::asio::ip::tcp::no_delay{true});
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
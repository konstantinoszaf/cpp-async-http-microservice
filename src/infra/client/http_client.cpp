#include "adapters/outgoing/boost_http_adapter.h"
#include "infra/client/http_client.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
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

Response HttpClient::post(std::string_view payload, RequestInfo& config) {
    asio::io_context ioc;
    ssl::context ctx(ssl::context::tlsv13_client);
    ctx.set_default_verify_paths();

    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
    stream.set_verify_mode(ssl::verify_peer);
    stream.set_verify_callback(ssl::rfc2818_verification(config.host));

    if (!SSL_set_tlsext_host_name(stream.native_handle(), config.host.c_str())) {
        std::cout << "Got in, host is: " << config.host << '\n';

        boost::system::error_code ec{static_cast<int>(::ERR_get_error()),
                                    asio::error::get_ssl_category()};
        return bad_response(ec.message(), HTTP::code::InternalServerError);
    }

    beast::error_code ec;
    tcp::resolver resolver(ioc);
    auto& socket = stream.next_layer();

    auto const results = resolver.resolve(config.host, config.port, ec);

    if (ec) {
        std::cout << "[DNS][ERROR] resolve failed\n"
                    << "  host    = " << config.host << "\n"
                    << "  port    = " << config.port << "\n"
                    << "  code    = " << ec.value() << "\n"
                    << "  category= " << ec.category().name() << "\n"
                    << "  message = " << ec.message() << "\n";
        return bad_response(ec.message(), HTTP::code::ServiceUnavailable);
    }

    socket.expires_after(std::chrono::seconds(30));
    socket.connect(results, ec);

    if (ec) {
        return bad_response(ec.message(), HTTP::code::GatewayTimeout);
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

    socket.expires_after(std::chrono::seconds(30));
    stream.handshake(ssl::stream_base::client, ec);

    if (ec) {
        return bad_response(ec.message(), HTTP::code::BadGateway);
    }

    http::write(stream, req, ec);

    if (ec) {
        std::cout << "Could not write\n";
        return bad_response(ec.message(), HTTP::code::BadGateway);
    }

    beast::flat_buffer buffer;
    http::response<http::string_body> res;

    socket.expires_after(std::chrono::seconds(30));
    http::read(stream, buffer, res, ec);

    if (ec && ec != http::error::end_of_stream) {
        std::cout << "Could not read\n";
        return bad_response(ec.message(), HTTP::code::BadGateway);
    }

    stream.shutdown(ec);

    if (ec && ec != asio::error::eof && ec != boost::asio::ssl::error::stream_truncated) {
        std::cout << "Could not shut down\n";
        return bad_response(res.body(),
                            static_cast<HTTP::code>(res.result_int()));
    }

    return BoostHttpAdapter::from_boost(res);
}

Response HttpClient::bad_response(std::string_view msg, HTTP::code code) {
    return Response{code, {{"Content-Type","text/plain"}}, std::string{msg}};
}
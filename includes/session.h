#pragma once
#include "session_interface.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <memory>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

using tcp = asio::ip::tcp;

class Session : public ISession, public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket sock)
      : socket_(std::move(sock)) {}
    void start() override;
private:
    void read();
    void write();
    void handle_request();

    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
};
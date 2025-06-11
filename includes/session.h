#pragma once
#include "session_interface.h"
#include "router_interface.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;

class Session : public ISession, public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket sock, std::shared_ptr<IRouter> router)
      : socket_(std::move(sock)), router_(router) {}
    void read() override;
private:
    void write() override;
    void handle_request() override;

    tcp::socket socket_;
    std::shared_ptr<IRouter> router_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
};
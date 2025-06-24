#pragma once
#include "core/ports/session_interface.h"
#include "core/ports/router_interface.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;

class Session : public ISession {
public:
    Session(tcp::socket sock, std::shared_ptr<IRouter> router)
      : socket_(std::move(sock)), router_(router) {}
    async_task<void> run() override;
private:
    tcp::socket socket_;
    std::shared_ptr<IRouter> router_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
};
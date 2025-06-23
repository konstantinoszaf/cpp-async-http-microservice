#pragma once
#include "core/ports/server_interface.h"
#include "core/ports/router_interface.h"
#include "core/ports/session_interface.h"
#include "core/ports/factory_interface.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "functional"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;

struct ServerSettings {
    unsigned int port;
    ServerSettings(unsigned int port_) : port{port_} {}
};

class Server : public IServer {
public:
    Server(ServerSettings& settings, IFactory& factory_, asio::io_context& io_ctx_)
      : io_context{io_ctx_},
        acceptor(io_context, tcp::endpoint(tcp::v4(), settings.port)), factory{factory_} {}

    void start() override {
        std::cout << "HTTP Server running on port "
                  << acceptor.local_endpoint().port()
                  << "...\n";
        accept();
        io_context.run();
    }

private:
    void accept() {
        acceptor.async_accept(
            [this](boost::system::error_code ec, tcp::socket sock) {
                if (!ec) {
                    auto session = factory.createSession(std::move(sock));
                    session->read();
                }
                accept();
            });
    }

    asio::io_context& io_context;
    tcp::acceptor acceptor;
    IFactory& factory;
};
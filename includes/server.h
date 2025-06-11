#pragma once
#include "server_interface.h"
#include "session.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>

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
    Server(ServerSettings& settings)
        : io_context{},
        acceptor(io_context, tcp::endpoint(tcp::v4(), settings.port)) {}

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
                    std::make_shared<Session>(std::move(sock))->start();
                }
                accept();
            });
    }

    asio::io_context io_context;
    tcp::acceptor acceptor;
};
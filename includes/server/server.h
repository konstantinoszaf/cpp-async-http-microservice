#pragma once
#include "server/server_interface.h"
#include "router/router_interface.h"
#include "session/session_interface.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "functional"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;
using SessionFactory = std::function<std::shared_ptr<ISession>(
        tcp::socket sock,
        std::shared_ptr<IRouter> rtr
    )>;

struct ServerSettings {
    unsigned int port;
    SessionFactory session_factory;
    ServerSettings(unsigned int port_, SessionFactory& factory)
  : port{port_}, session_factory{factory} {}
};

class Server : public IServer {
public:
    Server(ServerSettings& settings, std::shared_ptr<IRouter> router_)
      : io_context{},
        acceptor(io_context, tcp::endpoint(tcp::v4(), settings.port)),
        router(router_),
        create_session{settings.session_factory} {}

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
                    create_session(std::move(sock), router)->read();
                }
                accept();
            });
    }

    asio::io_context io_context;
    tcp::acceptor acceptor;
    std::shared_ptr<IRouter> router;
    SessionFactory create_session;
};
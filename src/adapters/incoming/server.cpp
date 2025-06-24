#include "adapters/incoming/server.h"
#include <iostream>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

Server::Server(const ServerSettings& settings, IFactory& factory_, asio::io_context& io_ctx_)
: io_context{io_ctx_},
  acceptor(io_context, tcp::endpoint(tcp::v4(), settings.port)),
  factory{factory_},
  work_guard{asio::make_work_guard(io_context)},
  signals{io_context, SIGINT, SIGTERM} {}

void Server::start() {
    std::cout << "HTTP Server running on port "
                << acceptor.local_endpoint().port()
                << "...\n";

    setup_signal_handlers();
    accept();
    io_context.run();
}

void Server::stop() {
    std::cout << "Shutting down server...\n";
    boost::system::error_code ec;
    acceptor.close(ec);
    if (ec) {
        std::cout << "Error closing acceptor: " << ec.message() << '\n';
    }

    work_guard.reset();
}

void Server::setup_signal_handlers() {
    signals.async_wait(
        [this](boost::system::error_code ec, int sig) {
            std::cout << "Received " << sig << ", signal: " << ec.message() << '\n';
            this->stop();
        }
    );
}

void Server::accept() {
    acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket sock) {
            if (!ec) {
                asio::co_spawn(
                    io_context,
                    [s = factory.createSession(std::move(sock))]{
                        return s->run();
                    }, asio::detached
                );
            }
            if (acceptor.is_open())
                accept();
        });
}

#pragma once
#include "core/ports/server_interface.h"
#include "core/ports/router_interface.h"
#include "core/ports/session_interface.h"
#include "core/ports/factory_interface.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "functional"

namespace asio = boost::asio;

using tcp = asio::ip::tcp;

struct ServerSettings {
    unsigned int port;
    ServerSettings(unsigned int port_) : port{port_} {}
};

class Server : public IServer {
public:
    Server(const ServerSettings& settings, IFactory& factory_, asio::io_context& io_ctx_);

    void start() override;
    void stop();
private:
    void accept();
    void setup_signal_handlers();

    asio::io_context& io_context;
    tcp::acceptor acceptor;
    IFactory& factory;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard;
    asio::signal_set signals;
};
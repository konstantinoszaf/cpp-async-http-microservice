#include <memory>
#include <iostream>
#include "adapters/incoming/session.h"
#include "adapters/outgoing/boost_http_adapter.h"

async_task<void> Session::run() {
    boost::system::error_code ec;

    co_await http::async_read(socket_, buffer_, req_, asio::redirect_error(asio::use_awaitable, ec));
    if (ec && ec != http::error::end_of_stream) {
        std::cout << "Could not read\n";
        co_return;
    }

    Request request = BoostHttpAdapter::from_boost(req_);
    Response response;
    co_await router_->route(request, response);
    res_ = BoostHttpAdapter::to_boost(response);

    co_await http::async_write(socket_, res_, asio::redirect_error(asio::use_awaitable, ec));
    if (ec && ec != http::error::end_of_stream) {
        std::cout << "Could not read\n";
        co_return;
    }

    socket_.shutdown(tcp::socket::shutdown_send, ec);
    if (ec && ec != asio::error::eof) {
        std::cout << "Could not shut down\n";
    }

    co_return;
}

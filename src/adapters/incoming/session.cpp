#include <memory>
#include <iostream>
#include "adapters/incoming/session.h"
#include "adapters/outgoing/boost_http_adapter.h"

void Session::read() {
    auto self = shared_from_this();
    http::async_read(
        socket_,
        buffer_,
        req_,
        [this, self](beast::error_code ec, std::size_t) {
            if (!ec) {
                boost::asio::co_spawn(
                    socket_.get_executor(),
                    [self]() mutable { return self->handle_request(); },
                    boost::asio::detached
                );
            }
        }
    );
}

async_task<void> Session::handle_request() {
    Request request = BoostHttpAdapter::from_boost(req_);
    Response response;
    co_await router_->route(request, response);

    res_ = BoostHttpAdapter::to_boost(response);
    write();
    co_return;
}

void Session::write() {
    auto self = shared_from_this();
    http::async_write(
        socket_,
        res_,
        [this, self](beast::error_code ec, std::size_t) {
            socket_.shutdown(tcp::socket::shutdown_send, ec);
        }
    );
}
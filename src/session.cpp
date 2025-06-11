#include <memory>
#include <iostream>
#include "session.h"

void Session::read() {
    auto self = shared_from_this();
    http::async_read(
        socket_,
        buffer_,
        req_,
        [this, self](beast::error_code ec, std::size_t) {
            if (!ec) {
                handle_request();
            }
        }
    );
}

void Session::handle_request() {
    router_->route(req_, res_);
    write();
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
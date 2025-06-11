#include <boost/json.hpp>
#include <memory>
#include <iostream>
#include "session.h"

void Session::start() {
    read();
}

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
    json::object response_json;

    if (req_.method() == http::verb::get && req_.target() == "/tiny") {
        response_json["status"] = "Valid Response!";
        res_.result(http::status::ok);
    } else {
        response_json["status"] = "Unknown endpoint.";
        res_.result(http::status::bad_request);
    }

    res_.set(http::field::content_type, "application/json");
    res_.body() = json::serialize(response_json);
    res_.prepare_payload();

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
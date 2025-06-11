#include "router.h"
#include <iostream>

void Router::add_route(http::verb method,
                        const std::string& endpoint,
                        handler_ptr handler) {
    routes_[method][endpoint] = std::move(handler);
}

void Router::route(const http::request<http::string_body>& req,
            http::response<http::string_body>& res) {
    auto handler_map = routes_.find(req.method());
    if (handler_map != routes_.end()) {
        auto handler = handler_map->second.find(std::string(req.target()));
        if (handler != handler_map->second.end()) {
            handler->second->handle(req, res);
            return;
        }
    }

    // defaults to 404
    res.result(http::status::not_found);
    res.set(http::field::content_type, "text/plain");
    res.body() = "Not Found";
    res.prepare_payload();
}
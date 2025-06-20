#include "adapters/incoming/router.h"
#include "core/ports/handler_interface.h"
#include "core/domain/types.h"
#include <iostream>

void Router::add_route(HTTP::method method,
                        const std::string& endpoint,
                        handler_ptr handler)
{
    if (endpoint.empty() || !handler) return;

    switch(method) {
        case HTTP::method::DELETE:
        case HTTP::method::POST:
        case HTTP::method::GET:
        case HTTP::method::PUT:
            routes_[method][endpoint] = std::move(handler);
            break;
        default:
            std::cout << "Unsupported method.\n";
            return;
    }
}

void Router::route(const Request& req, Response& res) {
    auto handler_map = routes_.find(req.method);
    if (handler_map != routes_.end()) {
        auto handler = handler_map->second.find(std::string(req.target));
        if (handler != handler_map->second.end()) {
            handler->second->handle(req, res);
            return;
        }
    }

    // defaults to 404
    res.status_code = HTTP::code::NotFound;
    res.headers["Content-Type"] = "text/plain";
    res.body = "Not Found";
}
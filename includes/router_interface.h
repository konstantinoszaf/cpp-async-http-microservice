#pragma once
#include "handler_interface.h"
#include <boost/beast.hpp>
#include <memory>
#include <string>

namespace http = boost::beast::http;

class IRouter {
public:
    virtual ~IRouter() = default;

    using handler_ptr = std::shared_ptr<IHandler>;
    virtual void add_route(http::verb method,
                            const std::string& endpoint,
                            handler_ptr handler) = 0;
    virtual void route(const http::request<http::string_body>& req,
                        http::response<http::string_body>& res) = 0;
};
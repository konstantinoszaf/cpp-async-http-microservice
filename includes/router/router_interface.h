#pragma once
#include "handler/handler_interface.h"
#include <memory>
#include <string>
#include "types.h"

class IRouter {
public:
    virtual ~IRouter() = default;

    using handler_ptr = std::shared_ptr<IHandler>;
    virtual void add_route(HTTP::method method,
                            const std::string& endpoint,
                            handler_ptr handler) = 0;
    virtual void route(const Request& req, Response& res) = 0;
};
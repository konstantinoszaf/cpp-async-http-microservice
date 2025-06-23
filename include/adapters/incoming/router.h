#pragma once
#include "core/ports/router_interface.h"
#include <map>
#include <memory>
#include <string>

class Router : public IRouter {
public:
    void add_route(HTTP::method method, const std::string& path,
                    handler_ptr handler) override;
    async_task<void> route(const Request& req, Response& res) override;
private:
    std::map<HTTP::method, std::map<std::string, handler_ptr>> routes_;
};
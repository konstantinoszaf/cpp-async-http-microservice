#pragma once
#include "router_interface.h"
#include <map>
#include <memory>
#include <string>

class Router : public IRouter {
public:
    void add_route(http::verb method, const std::string& path,
                    handler_ptr handler) override;
    void route(const http::request<http::string_body>& req,
                http::response<http::string_body>& res) override;
private:
    std::map<http::verb, std::map<std::string, handler_ptr>> routes_;
};
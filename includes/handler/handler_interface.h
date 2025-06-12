#pragma once
#include <boost/beast.hpp>
namespace http = boost::beast::http;

class IHandler {
public:
    virtual ~IHandler() = default;
    virtual void handle(const http::request<http::string_body>& req,
                        http::response<http::string_body>& res) = 0;
};
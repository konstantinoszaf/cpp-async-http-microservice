#pragma once
#include "handler_interface.h"
#include <boost/json.hpp>

namespace json = boost::json;

class TinyHandler : public IHandler {
public:
    void handle(const http::request<http::string_body>& req,
                http::response<http::string_body>& res) override {
        json::object response_json;

        response_json["status"] = "Valid Response!";
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(response_json);
        res.prepare_payload();
    }
};
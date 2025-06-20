// src/BoostHttpAdapter.cpp
#include "http_adapter/boost_http_adapter.h"
#include "types.h"
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>

namespace http = boost::beast::http;

http::request<http::string_body> BoostHttpAdapter::to_boost(const Request& request)
{
    // Convert method string -> verb enum
    http::verb m = method_to_verb(request.method);

    // Build the Beast request (HTTP/1.1)
    http::request<http::string_body> out{m, request.target, 11};
    // Copy headers
    for (auto const& [k,v] : request.headers)
        out.set(k, v);
    // Copy body & finalize Content-Length
    out.body() = request.body;
    out.prepare_payload();
    return out;
}

http::response<http::string_body> BoostHttpAdapter::to_boost(const Response& response)
{
    // Map status code into Beast status
    auto st = code_to_status(response.status_code);

    // Build the Beast response (HTTP/1.1)
    http::response<http::string_body> out{st, 11};
    // Copy headers
    for (auto const& [k,v] : response.headers)
        out.set(k, v);
    // Copy body & finalize Content-Length
    out.body() = response.body;
    out.prepare_payload();
    return out;
}

Request BoostHttpAdapter::from_boost(const http::request<http::string_body>& req)
{
    Request out;
    // Extract method and target
    out.method = verb_to_method(req.method());
    out.target = std::string(req.target());
    // Extract headers
    for (auto const& field : req) {
        out.headers.emplace(
            std::string(field.name_string()),
            std::string(field.value())
        );
    }
    // Extract body
    out.body = req.body();
    return out;
}

Response BoostHttpAdapter::from_boost(const http::response<http::string_body>& resp)
{
    Response out;
    // Extract status code
    out.status_code = status_to_code(resp.result());
    // Extract headers
    for (auto const& field : resp) {
        out.headers.emplace(
            std::string(field.name_string()),
            std::string(field.value())
        );
    }

    // Extract body
    out.body = resp.body();
    return out;
}

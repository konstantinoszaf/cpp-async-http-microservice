#pragma once
#include "core/domain/types.h"
#include <boost/beast.hpp>

namespace http = boost::beast::http;

class BoostHttpAdapter {
public:
    static http::request<http::string_body> to_boost(const Request& request);
    static http::response<http::string_body> to_boost(const Response& response);

    static Request from_boost(const http::request<http::string_body>& request);
    static Response from_boost(const http::response<http::string_body>& response);
private:
    static inline HTTP::method verb_to_method(http::verb v) {
        switch (v) {
        case http::verb::get:     return HTTP::method::GET;
        case http::verb::post:    return HTTP::method::POST;
        case http::verb::delete_: return HTTP::method::DELETE;
        case http::verb::put:     return HTTP::method::PUT;
        default:                  return HTTP::method::MAX;
        }
    }

    static inline http::verb method_to_verb(HTTP::method m) {
        switch (m) {
        case HTTP::method::GET:    return http::verb::get;
        case HTTP::method::POST:   return http::verb::post;
        case HTTP::method::DELETE: return http::verb::delete_;
        case HTTP::method::PUT:    return http::verb::put;
        default:                   return http::verb::unknown;
        }
    }

    static inline HTTP::code status_to_code(http::status s) {
        return static_cast<HTTP::code>(static_cast<unsigned short>(s));
    }

    static inline http::status code_to_status(HTTP::code c) {
        return static_cast<http::status>(static_cast<unsigned short>(c));
    }
};
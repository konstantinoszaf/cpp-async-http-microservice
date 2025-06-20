#pragma once
#include <map>
#include <string>

enum class  ProviderType {
    BITLY   = 0,
    TINYURL = 1
};

namespace HTTP {
enum class code : unsigned short {
    // 2xx: Success
    OK                    = 200,
    Created               = 201,
    Accepted              = 202,
    NoContent             = 204,

    // 3xx: Redirection
    MovedPermanently      = 301,
    Found                 = 302,
    NotModified           = 304,

    // 4xx: Client errors
    BadRequest            = 400,
    Unauthorized          = 401,
    Forbidden             = 403,
    NotFound              = 404,
    MethodNotAllowed      = 405,
    Conflict              = 409,
    PayloadTooLarge       = 413,
    UnsupportedMediaType  = 415,

    // 5xx: Server errors
    InternalServerError   = 500,
    NotImplemented        = 501,
    BadGateway            = 502,
    ServiceUnavailable    = 503,
    GatewayTimeout        = 504
};

enum class method {
    GET,
    POST,
    DELETE,
    PUT,
    MAX
};

} // namespace HTTP

struct Request {
    HTTP::method method;
    std::string target;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct Response {
    HTTP::code status_code;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct RequestInfo {
    std::string host;
    std::string endpoint;
    std::string port;
    std::string authorization_token;
};
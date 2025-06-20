#pragma once
#include <string_view>

struct Response;
struct RequestInfo;

class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual Response post(std::string_view body, RequestInfo& data) = 0;
};
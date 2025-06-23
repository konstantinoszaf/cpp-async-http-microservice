#pragma once
#include <string_view>
#include "task.h"

struct Response;
struct RequestInfo;

class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual async_task<Response> post(std::string_view body, RequestInfo& data) = 0;
};
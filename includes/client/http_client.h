#pragma once
#include "http_client_interface.h"
#include "types.h"

class HttpClient : public IHttpClient {
public:
    Response post(std::string_view payload, RequestInfo& config) override;
private:
    Response bad_response(std::string_view reason, HTTP::code code);
};
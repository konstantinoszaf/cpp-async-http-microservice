#pragma once
#include "core/domain/types.h"
#include "core/ports/http_client_interface.h"

class HttpClient : public IHttpClient {
public:
    Response post(std::string_view payload, RequestInfo& config) override;
private:
    Response bad_response(std::string_view reason, HTTP::code code);
};
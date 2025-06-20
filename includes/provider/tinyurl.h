#pragma once
#include "provider.h"
#include "client/http_client_interface.h"

class TinyURL : public Provider {
public:
    TinyURL(std::shared_ptr<IHttpClient> client,
            std::shared_ptr<IEnvReader> env);
private:
    std::string get_short_url(std::string_view rsp) override;
};
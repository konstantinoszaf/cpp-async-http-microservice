#pragma once
#include "client/http_client_interface.h"
#include "provider.h"
#include "memory"

class Bitly : public Provider {
public:
    Bitly(std::shared_ptr<IHttpClient> client, std::shared_ptr<IEnvReader> env);
private:
    std::string get_short_url(std::string_view payload) override;
};
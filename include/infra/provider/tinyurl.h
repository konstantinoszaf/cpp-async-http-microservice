#pragma once
#include "infra/provider/provider.h"
#include <memory>

class TinyURL : public Provider {
public:
    TinyURL(std::shared_ptr<IHttpClient> client,
            std::shared_ptr<IEnvReader> env,
            std::shared_ptr<ICacheClient> redis_);
protected:
    std::string get_short_url(std::string_view rsp) override;
    std::string get_error_message(std::string_view payload) override;
};
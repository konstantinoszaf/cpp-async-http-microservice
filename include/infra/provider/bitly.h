#pragma once
#include "provider.h"
#include <memory>

class Bitly : public Provider {
public:
    Bitly(std::shared_ptr<IHttpClient> client,
        std::shared_ptr<IEnvReader> env,
        std::shared_ptr<ICacheClient> redis_);
protected:
    std::string get_short_url(std::string_view payload) override;
    std::string get_error_message(std::string_view error_msg) override;
};
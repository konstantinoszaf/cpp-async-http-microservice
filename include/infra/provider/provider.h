#pragma once
#include "core/ports/provider_interface.h"
#include "core/domain/types.h"
#include <memory>

class IHttpClient;
class IEnvReader;

// template class
class Provider : public IProvider {
public:
    Provider(std::shared_ptr<IHttpClient> client_,
            ProviderType type_,
            std::shared_ptr<IEnvReader> env,
            std::string_view jsonKey);
    std::string shorten(std::string_view url) override;
protected:
    virtual std::string get_short_url(std::string_view payload) = 0;
    std::string api_key;
    std::string json_key;
    RequestInfo request_info;

private:
    std::string create_request_body(std::string_view url) override;
    // json::object validate_response_body(std::string_view payload);
    constexpr std::string_view get_env_key(ProviderType type);
    std::shared_ptr<IHttpClient> http_client;
};
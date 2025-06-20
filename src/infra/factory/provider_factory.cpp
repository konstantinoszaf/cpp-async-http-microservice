#include "infra/factory/provider_factory.h"
#include "infra/provider/bitly.h"
#include "infra/provider/tinyurl.h"

ProviderFactory::ProviderFactory(std::shared_ptr<IHttpClient> client_,
                                std::shared_ptr<IEnvReader> env_)
: client{client_}, env{env_}{}

std::shared_ptr<IProvider> ProviderFactory::createProvider(ProviderType type) {
    switch (type) {
    case ProviderType::TINYURL:
        return std::make_shared<TinyURL>(client, env);
    default:
        return std::make_shared<Bitly>(client, env);
    }
}
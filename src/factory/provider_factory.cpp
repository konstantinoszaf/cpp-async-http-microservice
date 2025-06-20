#include "factory/provider_factory.h"
#include "provider/bitly.h"
#include "provider/tinyurl.h"

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
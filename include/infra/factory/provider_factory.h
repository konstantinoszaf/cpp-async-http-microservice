#pragma once
#include "core/ports/provider_factory_interface.h"
#include "core/ports/cache_client_interface.h"

class ProviderFactory : public IProviderFactory {
public:
    ProviderFactory(std::shared_ptr<IHttpClient> client,
                    std::shared_ptr<IEnvReader> env,
                    std::shared_ptr<ICacheClient> redis);
    std::shared_ptr<IProvider> createProvider(ProviderType type) override;

private:
   std::shared_ptr<IHttpClient> client;
   std::shared_ptr<IEnvReader> env;
   std::shared_ptr<ICacheClient> redis;
};
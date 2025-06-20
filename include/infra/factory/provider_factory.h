#pragma once
#include "core/ports/provider_factory_interface.h"

class ProviderFactory : public IProviderFactory {
public:
    ProviderFactory(std::shared_ptr<IHttpClient> client,
                    std::shared_ptr<IEnvReader> env);
    std::shared_ptr<IProvider> createProvider(ProviderType type) override;

private:
   std::shared_ptr<IHttpClient> client;
   std::shared_ptr<IEnvReader> env;
};
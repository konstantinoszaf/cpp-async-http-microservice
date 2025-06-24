#pragma once

#include "core/ports/provider_factory_interface.h"
#include "mock_provider.h"

class MockProviderFactory : public IProviderFactory {
public:
    std::shared_ptr<IProvider> provider;

    MockProviderFactory() {
        provider = std::make_shared<MockProvider>();
    }

    std::shared_ptr<IProvider> createProvider(ProviderType type) override {
        (void)type;
        return provider;
    }
};
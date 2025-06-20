#pragma once
#include "core/ports/factory_interface.h"
#include <boost/asio.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Factory : public IFactory {
public:
    Factory();
    std::shared_ptr<IRouter> getRouter() override;
    std::shared_ptr<ISession> createSession(tcp::socket sock) override;
    std::shared_ptr<IParser> getParser() override;
    std::shared_ptr<IValidator> getValidator() override;
    std::shared_ptr<IProviderFactory> getProviderFactory() override;
    std::shared_ptr<IEnvReader> getEnv() override;
    std::shared_ptr<IHttpClient> getHttpClient() override;
private:
    std::shared_ptr<IRouter> router;
    std::shared_ptr<IParser> parser;
    std::shared_ptr<IValidator> validator;
    std::shared_ptr<IProviderFactory> provider_factory;
    std::shared_ptr<IEnvReader> env_reader;
    std::shared_ptr<IHttpClient> http_client;
};
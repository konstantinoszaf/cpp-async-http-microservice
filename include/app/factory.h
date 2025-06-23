#pragma once
#include "core/ports/factory_interface.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

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
    asio::io_context& getContext();
    asio::ssl::context& getSslContext();
private:
    std::shared_ptr<IRouter> router;
    std::shared_ptr<IParser> parser;
    std::shared_ptr<IValidator> validator;
    std::shared_ptr<IProviderFactory> provider_factory;
    std::shared_ptr<IEnvReader> env_reader;
    std::shared_ptr<IHttpClient> http_client;

    asio::io_context io_ctx_;
    asio::ssl::context ssl_ctx_;
};
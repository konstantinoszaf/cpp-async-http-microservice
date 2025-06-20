#pragma once
#include <memory>
#include <functional>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class IRouter;
class IParser;
class IValidator;
class ISession;
class IProviderFactory;
class IEnvReader;
class IHttpClient;

class IFactory {
public:
    virtual std::shared_ptr<IRouter> getRouter() = 0;
    virtual std::shared_ptr<ISession> createSession(tcp::socket sock) = 0;
    virtual std::shared_ptr<IParser> getParser() = 0;
    virtual std::shared_ptr<IValidator> getValidator() = 0;
    virtual std::shared_ptr<IProviderFactory> getProviderFactory() = 0;
    virtual std::shared_ptr<IEnvReader> getEnv() = 0;
    virtual std::shared_ptr<IHttpClient> getHttpClient() = 0;
};
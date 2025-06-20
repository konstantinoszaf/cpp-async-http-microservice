#include "factory/factory.h"
#include "validator/key_count_rule.h"
#include "router/router.h"
#include "handler/shortly_handler.h"
#include "validator/key_rule.h"
#include "validator/json_validator.h"
#include "parser/json_parser.h"
#include "session/session.h"
#include "environment_reader/env_reader.h"
#include "client/http_client.h"
#include "factory/provider_factory.h"

Factory::Factory() {
    // 1) Validation rules
    std::vector<std::unique_ptr<IValidationRule>> rules;

    //rule to enforce at least one and at most two keys
    rules.push_back(std::make_unique<KeyCountRule>(1, 2));
    // rule to enforce mandatory and optional keys
    rules.push_back(std::make_unique<KeyRule>(
        std::vector<std::string>{"url"}, // mandatory
        std::unordered_set<std::string>{"provider"} // optional
    ));

    // 2) Parser and validator
    validator = std::make_shared<JsonValidator>();
    for (auto& r : rules) validator->add_rule(std::move(r));

    parser = std::make_shared<JsonParser>(validator);

    // 3) create http client
    http_client = std::make_shared<HttpClient>();

    // 4) create environment reader
    env_reader = std::make_shared<EnvReader>();

    // 5) Method that creates the provider
    provider_factory = std::make_shared<ProviderFactory>(http_client, env_reader);

    // 6) Handlers
    router = std::make_shared<Router>();
    router->add_route(HTTP::method::POST, "/shortly", std::make_shared<ShortlyHandler>(parser, provider_factory));
}

std::shared_ptr<IRouter> Factory::getRouter() {
    return router;
}

std::shared_ptr<ISession> Factory::createSession(tcp::socket sock) {
    return std::make_shared<Session>(
        std::move(sock),
        router
    );
}

std::shared_ptr<IParser> Factory::getParser() {
    return parser;
}

std::shared_ptr<IValidator> Factory::getValidator() {
    return validator;
}

std::shared_ptr<IProviderFactory> Factory::getProviderFactory() {
    return provider_factory;
}

std::shared_ptr<IEnvReader> Factory::getEnv() {
    return env_reader;
}

std::shared_ptr<IHttpClient> Factory::getHttpClient() {
    return http_client;
}
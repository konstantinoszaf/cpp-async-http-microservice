#include "infra/provider/provider.h"
#include "infra/exception/exceptions.h"
#include "core/ports/env_reader_interface.h"
#include "core/ports/http_client_interface.h"
#include <memory>
#include <iostream>
#include <boost/json.hpp>
#include <boost/asio/awaitable.hpp>

namespace json = boost::json;

Provider::Provider(std::shared_ptr<IHttpClient> client_,
                    ProviderType type,
                    std::shared_ptr<IEnvReader> env,
                    std::string_view json_key_,
                    std::shared_ptr<ICacheClient> redis_)
: http_client(client_), json_key{json_key_}, redis{redis_} {
    api_key = env->get(get_env_key(type));

    if (api_key.empty()) {
        std::cout << "api key is empty in env\n";
        throw URLShortener::exception::ProviderException(
            "Internal Server Error",
            HTTP::code::InternalServerError
        );
    }
}

std::string_view Provider::get_env_key(ProviderType type) {
    switch (type)
    {
    case ProviderType::TINYURL:
        return "API_KEY_TINYURL";
    case ProviderType::BITLY:
    default:
        return "API_KEY_BITLY";
    }
}

std::string Provider::create_request_body(std::string_view url) {
    json::object payload{};
    payload[json_key] = url;
    return json::serialize(payload);
}

async_task<std::string> Provider::shorten(std::string_view url) {
    if (url.empty()) {
        std::cout << "URL is empty!\n";
        throw URLShortener::exception::ProviderException(
            "url value can not be empty",
            HTTP::code::BadRequest
        );
    }

    auto cached = co_await redis->get(url);
    if (cached) {
        std::cout << "Got a cache hit!";
        co_return std::move(*cached);
    }

    auto body = create_request_body(url);
    auto response = co_await http_client->post(body, request_info);

    if (response.status_code != HTTP::code::OK || response.body.empty()) {
        throw URLShortener::exception::ProviderException(
            get_error_message(response.body),
            response.status_code
        );
    }

    auto shortened_url = get_short_url(response.body);
    co_await redis->set(url, shortened_url);

    co_return shortened_url;
}

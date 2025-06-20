#include "provider/provider.h"
#include "exception/validation_exception.h"
#include <memory>
#include <iostream>
#include <boost/json.hpp>

namespace json = boost::json;

Provider::Provider(std::shared_ptr<IHttpClient> client_,
                    ProviderType type,
                    std::shared_ptr<IEnvReader> env,
                    std::string_view json_key_) 
: http_client(client_), json_key{json_key_} {
    api_key = env->get(get_env_key(type));

    if (api_key.empty()) {
        std::cout << "api key is empty in env\n";
        throw URLShortener::exception::ProviderException(
            "Internal Server Error",
            HTTP::code::InternalServerError
        );
    }
}

constexpr std::string_view Provider::get_env_key(ProviderType type) {
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

// json::object Provider::validate_response_body(std::string_view payload) {
//     json::value j = json::parse(rsp);

//     if (!j.is_object()) {
//         throw URLShortener::exception::ProviderException(
//             "Internal Server Error",
//             HTTP::code::InternalServerError
//         );
//     }

//     return json::object obj = j.as_object();
// }

std::string Provider::shorten(std::string_view url) {
    std::cout << "Hello\n";
    if (url.empty()) {
        std::cout << "URL is empty!\n";
        throw URLShortener::exception::ProviderException(
            "url value can not be empty",
            HTTP::code::BadRequest
        );
    }

    auto body = create_request_body(url);
    std::cout << "request payload is: " << body << '\n';
    std::cout << "from Provider: " << request_info.host << '\n';
    auto response = http_client->post(body, request_info);

    if (response.status_code != HTTP::code::OK || response.body.empty()) {
        throw URLShortener::exception::ProviderException(
            std::string{response.body},
            response.status_code
        );
    }

    return get_short_url(response.body);
}

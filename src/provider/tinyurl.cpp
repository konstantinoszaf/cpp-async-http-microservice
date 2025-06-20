#include "provider/tinyurl.h"
#include "exception/validation_exception.h"
#include <boost/json.hpp>
#include <iostream>

namespace json = boost::json;

TinyURL::TinyURL(std::shared_ptr<IHttpClient> client, std::shared_ptr<IEnvReader> env)
 : Provider(client, ProviderType::TINYURL, env, "url") {
    RequestInfo info {
        "api.tinyurl.com",
        "/create",
        "https",
        "Bearer " + api_key
    };

    request_info = info;
};

std::string TinyURL::get_short_url(std::string_view payload) {
    std::cout << "get_short_url()\n";
    auto j = json::parse(payload);
    const json::object& obj = j.as_object();

    if (!obj.contains("data")) {
        throw URLShortener::exception::ProviderException(
            "Internal Server Error",
            HTTP::code::InternalServerError
        );
    }

    const json::object& data = obj.at("data").as_object();
    if (!data.contains("tiny_url")) {
        throw URLShortener::exception::ProviderException(
            "Internal Server Error",
            HTTP::code::InternalServerError
        );
    }

    std::cout << "leaving get_short_url()\n";
    return data.at("tiny_url").as_string().c_str();
}

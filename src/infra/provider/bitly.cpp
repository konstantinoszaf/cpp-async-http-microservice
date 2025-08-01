#include "infra/provider/bitly.h"
#include "infra/exception/exceptions.h"
#include <boost/json.hpp>
#include <iostream>

namespace json = boost::json;

Bitly::Bitly(std::shared_ptr<IHttpClient> client,
            std::shared_ptr<IEnvReader> env,
            std::shared_ptr<ICacheClient> redis_)
 : Provider(client, ProviderType::BITLY, env, "long_url", redis_) {
    request_info = RequestInfo {
                    "api-ssl.bitly.com",
                    "/v4/shorten",
                    "https",
                    "Bearer " + api_key
                };
};

std::string Bitly::get_short_url(std::string_view payload) {
    auto j = json::parse(payload);
    const json::object& obj = j.as_object();

    if (!obj.contains("link")) {
        std::cout << "Bitly Respone does not contain 'link' key\n";
        throw URLShortener::exception::ProviderException(
            "Internal Server Error",
            HTTP::code::InternalServerError
        );
    }

    return obj.at("link").as_string().c_str();
}

std::string Bitly::get_error_message(std::string_view payload) {
    boost::system::error_code ec;
    auto j = json::parse(payload, ec);

    if(ec || !j.is_object()) return "Unknown error";

    const json::object& obj = j.as_object();

    const auto* error = obj.if_contains("description");
    if (error) return error->as_string().c_str();

    error = obj.if_contains("message");
    if (error) return error->as_string().c_str();

    return "Unknown error";
}


#include "provider/bitly.h"
#include "exception/validation_exception.h"
#include <boost/json.hpp>
#include <iostream>

namespace json = boost::json;

Bitly::Bitly(std::shared_ptr<IHttpClient> client, std::shared_ptr<IEnvReader> env)
 : Provider(client, ProviderType::BITLY, env, "long_url") {
    RequestInfo info {
        "api-ssl.bitly.com",
        "/v4/shorten",
        "https",
        "Bearer " + api_key
    };

    request_info = info;
};

std::string Bitly::get_short_url(std::string_view payload) {
    std::cout << "get_short_url()\n";
    auto j = json::parse(payload);
    const json::object& obj = j.as_object();

    if (!obj.contains("link")) {
        std::cout << "Bitly Respone does not contain 'link' key\n";
        throw URLShortener::exception::ProviderException(
            "Internal Server Error",
            HTTP::code::InternalServerError
        );
    }

    std::cout << "leaving get_short_url()\n";
    return obj.at("link").as_string().c_str();
}

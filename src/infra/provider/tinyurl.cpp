#include "infra/provider/tinyurl.h"
#include "infra/exception/exceptions.h"
#include <boost/json.hpp>
#include <iostream>

namespace json = boost::json;

TinyURL::TinyURL(std::shared_ptr<IHttpClient> client,
                std::shared_ptr<IEnvReader> env,
                std::shared_ptr<ICacheClient> redis_)
 : Provider(client, ProviderType::TINYURL, env, "url", redis_) {
    request_info = RequestInfo{
        "api.tinyurl.com",
        "/create",
        "https",
        std::string{"Bearer "} + api_key
    };
}

std::string TinyURL::get_short_url(std::string_view payload) {
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

    return data.at("tiny_url").as_string().c_str();
}

std::string TinyURL::get_error_message(std::string_view payload) {
    boost::system::error_code ec;
    auto j = json::parse(payload, ec);

    if(ec || !j.is_object()) return "Unknown error";

    const json::object& obj = j.as_object();

    const auto* e = obj.if_contains("errors");
    if (!e || !e->is_array()) return "Unknown error";

    auto const& errors = e->as_array();
    if (errors.empty()) return "Unknown error";

    size_t total = (errors.size() - 1);
    for (const auto& error : errors) total += error.as_string().size();

    std::string out;
    out.reserve(total);

    bool first = true;
    for (const auto& error : errors) {
        if (!first) out.push_back(',');
        first = false;
        const auto& sv = error.as_string();
        out.append(sv.data(), sv.size());
    }

    return (!out.empty()) ? out : "Unknown error";
}
#include "infra/parser/json_parser.h"
#include "infra/exception/exceptions.h"
#include "core/ports/validator_interface.h"
#include <boost/json.hpp>
#include "iostream"

namespace json = boost::json;

JsonParser::JsonParser(std::shared_ptr<IValidator> validator) {
    this->validator = validator;
}

std::pair<std::string, ProviderType> JsonParser::parse(std::string_view j_str) {
    try {
        json::value j;
        try {
            j = json::parse(j_str);
        } catch (...) {
            throw URLShortener::exception::ValidationException("Invalid JSON format");
        }

        if (!j.is_object()) {
            throw URLShortener::exception::ValidationException("JSON is not an object");
        }

        const json::object& obj = j.as_object();
        validator->validate(obj);

        std::string url = obj.at("url").as_string().c_str();
        ProviderType provider = ProviderType::BITLY;
        if (obj.contains("provider")) provider = strToProviderType(obj.at("provider").as_string().c_str());

        return {std::move(url), std::move(provider)};
    }
    catch(const std::exception& e) {
        throw URLShortener::exception::ValidationException(e.what());
    }
}

ProviderType JsonParser::strToProviderType(const std::string& provider_str) {
    if (provider_str.empty()) return ProviderType::BITLY;

    if (provider_str == "tinyurl") return ProviderType::TINYURL;

    return ProviderType::BITLY;
}
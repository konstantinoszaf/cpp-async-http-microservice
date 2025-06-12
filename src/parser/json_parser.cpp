#include "parser/json_parser.h"
#include "exception/validation_exception.h"
#include "validator/json_validator.h"
#include "validator/key_count_rule.h"
#include "validator/key_rule.h"
#include <boost/json.hpp>

namespace json = boost::json;

JsonParser::JsonParser() {
    validator = std::make_unique<JsonValidator>();
    // enforce at least one and at most two keys
    validator->add_rule(std::make_unique<KeyCountRule>(1, 2));
    // enforce the presence of url key
    validator->add_rule(std::make_unique<KeyRule>(std::vector<std::string>{"url"}));
}

std::pair<std::string, std::string> JsonParser::parse(std::string_view j_str) {
    try {
        json::value j = json::parse(j_str);

        if (!j.is_object()) {
            throw URLShortener::exception::ValidationException("JSON is not an object");
        }

        const json::object& obj = j.as_object();
        validator->validate(obj);

        std::string url = obj.at("url").as_string().c_str();
        std::string provider = "bitly";
        if (obj.contains("provider")) provider = obj.at("provider").as_string().c_str();

        return {std::move(url), std::move(provider)};
    }
    catch(const std::exception& e) {
        throw URLShortener::exception::ValidationException(e.what());
    }
}
#pragma once
#include <boost/json.hpp>
#include <string>

namespace json = boost::json;

class IValidationRule {
public:
    virtual ~IValidationRule() = default;
    virtual bool is_valid(const json::object& obj, std::string& error) = 0;
};
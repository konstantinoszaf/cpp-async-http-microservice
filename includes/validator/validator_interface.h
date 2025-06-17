#pragma once
#include "rule_interface.h"
#include <boost/json.hpp>
#include <string>
#include <memory>
#include <vector>

namespace json = boost::json;

class IValidator {
public:
    virtual ~IValidator() = default;
    virtual void validate(const json::object& j) = 0;
    virtual void add_rule(std::unique_ptr<IValidationRule> rule) = 0;
};
#include "infra/validator/json_validator.h"
#include "infra/exception/exceptions.h"
#include "string"
#include <iostream>

void JsonValidator::add_rule(std::unique_ptr<IValidationRule> rule)
{
    if (!rule) {
        std::cout << "Rule is empty\n";
        return;
    }

    rules.push_back(std::move(rule));
}

void JsonValidator::validate(const json::object& obj)
{
    if (obj.empty()) {
        throw URLShortener::exception::ValidationException("Input is empty");
    }

    std::string error_msg;

    for (const auto& rule : rules) {
        if (!rule->is_valid(obj, error_msg)) {
            throw URLShortener::exception::ValidationException(error_msg);
        }
    }
}

std::size_t JsonValidator::rule_count() {
    return rules.size();
}

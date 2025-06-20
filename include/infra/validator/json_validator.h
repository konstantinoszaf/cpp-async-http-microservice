#pragma once
#include "core/ports/validator_interface.h"
#include <memory>
#include <vector>

class IValidationRule;

class JsonValidator : public IValidator {
public:
    void add_rule(std::unique_ptr<IValidationRule> rule) override;
    void validate(const json::object& j) override;
    std::size_t rule_count();
private:
    std::vector<std::unique_ptr<IValidationRule>> rules;
};
#pragma once
#include "validator_interface.h"
#include "rule_interface.h"
#include "memory"
#include "vector"

class JsonValidator : public IValidator {
public:
    void add_rule(std::unique_ptr<IValidationRule> rule) override;
    void validate(const json::object& j) override;
private:
    std::vector<std::unique_ptr<IValidationRule>> rules;
};
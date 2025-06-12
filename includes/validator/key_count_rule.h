#pragma once
#include "rule_interface.h"

class KeyCountRule : public IValidationRule {
public:
    KeyCountRule(std::size_t min_key_num, std::size_t max_key_num)
        :min_{min_key_num}, max_{max_key_num} {}

    bool is_valid(const json::object& obj, std::string& error) override;
private:
    size_t min_;
    size_t max_;
};
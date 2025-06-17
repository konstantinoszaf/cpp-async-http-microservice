#pragma once
#include "rule_interface.h"
#include <boost/json.hpp>
#include <unordered_set>
#include <vector>

namespace json = boost::json;

class KeyRule : public IValidationRule {
public:
    KeyRule(std::vector<std::string> mandatory_keys_,
            std::unordered_set<std::string> optional_keys_);
    bool is_valid(const json::object& obj, std::string& error) override;
private:
    std::vector<std::string> mandatory_keys;
    std::unordered_set<std::string> allowed_keys;
};
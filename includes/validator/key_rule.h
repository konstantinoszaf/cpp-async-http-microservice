#pragma once
#include "rule_interface.h"
#include <boost/json.hpp>
#include <vector>

namespace json = boost::json;

class KeyRule : public IValidationRule {
public:
    KeyRule(std::vector<std::string> keys_) : keys{keys_} {}
    bool is_valid(const json::object& obj, std::string& error) override;
private:
    std::vector<std::string> keys;
};
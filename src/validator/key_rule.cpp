#include "validator/key_rule.h"


KeyRule::KeyRule(std::vector<std::string> mandatory_keys_,
                std::unordered_set<std::string> optional_keys_)
 : mandatory_keys{mandatory_keys_} {
    allowed_keys.insert(mandatory_keys.begin(), mandatory_keys.end());
    allowed_keys.insert(optional_keys_.begin(), optional_keys_.end());
}

bool KeyRule::is_valid(const json::object& obj, std::string& error) {
    for (const auto& key : mandatory_keys) {
        if (!obj.contains(key)) {
            error = "JSON must contain key " + key;
            return false;
        }
    }

    for (const auto& [key, value] : obj) {
        if (!allowed_keys.contains(key)) {
            error = "Key " + std::string(key) + " is not allowed.";
            return false;
        }
    }

    return true;
}
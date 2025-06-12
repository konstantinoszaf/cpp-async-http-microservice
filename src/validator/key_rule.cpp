#include "validator/key_rule.h"

bool KeyRule::is_valid(const json::object& obj, std::string& error) {
    for (const auto& key : keys) {
        if (!obj.contains(key)) {
            error = "JSON must contain key " + key;
            return false;
        }
    }

    return true;
}
#include "infra/validator/key_count_rule.h"
#include <sstream>

bool KeyCountRule::is_valid(const json::object& obj, std::string& error)
{
    if (obj.size() < min_ || obj.size() > max_) {
        std::stringstream ss;
        ss << "JSON must contain at least " << min_ 
           << " key, and maximum " << max_;
        error = ss.str();
        return false;
    }

    return true;
}
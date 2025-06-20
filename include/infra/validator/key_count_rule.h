#pragma once
#include "core/ports/rule_interface.h"

class KeyCountRule : public IValidationRule {
public:
    KeyCountRule(std::size_t min_key_num, std::size_t max_key_num)
        : min_{0}, max_{3} {
            set_min(min_key_num);
            set_max(max_key_num);
        }

    bool is_valid(const json::object& obj, std::string& error) override;

    void set_min(std::size_t min) {
        min_ = sanitize(min, 1);
    }

    void set_max(std::size_t max) {
        max_ = sanitize(max, 2);
    }
private:
    // random numbers
    std::size_t sanitize(std::size_t val, std::size_t dv) {
        return (val > 0 && val <= 2) ? val : dv;
    }

    std::size_t min_;
    std::size_t max_;
};
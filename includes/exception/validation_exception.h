#pragma once
#include <stdexcept>
#include <string>

namespace URLShortener {
namespace exception {

class ValidationException : public std::exception {
    std::string reason;
public:
    ValidationException(std::string reason_) : reason{reason_} {}

    const char* what() const noexcept override {
        return reason.c_str();
    }
};

} // namespace exception
} // namespace URLShortener
#pragma once
#include <stdexcept>
#include <string>
#include <string_view>
#include "core/domain/types.h"

namespace URLShortener {
namespace exception {

class ValidationException : public std::exception {
    std::string reason;
public:
    ValidationException(std::string_view reason_) : reason{reason_} {}

    const char* what() const noexcept override {
        return reason.c_str();
    }
};

class ProviderException : public std::exception {
    std::string reason;
    HTTP::code status_code;
public:
    ProviderException(std::string_view reason_, HTTP::code status_) : reason{reason_}, status_code{status_} {}

    const char* what() const noexcept override {
        return reason.c_str();
    }

    HTTP::code code() const noexcept {
        return status_code;
    }
};

} // namespace exception
} // namespace URLShortener
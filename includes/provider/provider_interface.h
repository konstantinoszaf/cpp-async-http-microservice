#pragma once
#include <string>
#include <string_view>
#include "types.h"

class IProvider {
public:
    virtual ~IProvider() noexcept = default;
    virtual std::string shorten(std::string_view url) = 0;
    virtual std::string create_request_body(std::string_view url) = 0;
    virtual std::string get_short_url(std::string_view payload) = 0;
};
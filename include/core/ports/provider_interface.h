#pragma once
#include <string>
#include <string_view>
#include "core/domain/types.h"
#include "task.h"

class IProvider {
public:
    virtual ~IProvider() noexcept = default;
    virtual async_task<std::string> shorten(std::string_view url) = 0;
    virtual std::string create_request_body(std::string_view url) = 0;
    virtual std::string get_short_url(std::string_view payload) = 0;
};
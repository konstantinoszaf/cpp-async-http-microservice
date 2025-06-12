#pragma once
#include <string>
#include <string_view>

class IParser {
public:
    virtual ~IParser() = default;
    virtual std::pair<std::string, std::string> parse(std::string_view j_str) = 0;
};
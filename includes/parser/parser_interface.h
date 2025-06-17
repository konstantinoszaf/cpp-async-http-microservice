#pragma once
#include <string>
#include <string_view>

enum class ProviderType;

class IParser {
public:
    virtual ~IParser() = default;
    virtual std::pair<std::string, ProviderType> parse(std::string_view j_str) = 0;
};
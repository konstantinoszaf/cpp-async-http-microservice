#pragma once
#include <string>
#include <string_view>

class IEnvReader {
public:
    virtual ~IEnvReader() = default;
    virtual std::string get(std::string_view key) = 0;
};
#pragma once
#include "core/ports/env_reader_interface.h"
#include <cstdlib>

class EnvReader : public IEnvReader {
public:
    std::string get(std::string_view key) override {
        const char* env_p = std::getenv(key.data());
        return (env_p) ? env_p : "";
    }
};

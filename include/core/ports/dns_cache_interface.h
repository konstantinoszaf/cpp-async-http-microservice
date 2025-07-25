#pragma once

#include "core/domain/types.h"
#include <string_view>
#include <vector>

using Endpoints = std::vector<Endpoint>;

class IDnsCache {
public:
    virtual ~IDnsCache() = default;

    virtual Endpoints find_entry(std::string_view domain, std::string_view port) = 0;
    virtual void update_or_add_entry(std::string_view domain, std::string_view port, Endpoints& endpoints) = 0;
};

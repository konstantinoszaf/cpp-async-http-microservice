#pragma once

#include "core/domain/types.h"
#include "core/ports/task.h"
#include <string_view>
#include <vector>

using Endpoints = std::vector<Endpoint>;

class IDnsCache {
public:
    virtual ~IDnsCache() = default;

    /// Resolve host:port → a list of Endpoint.  Empty on error.
    virtual async_task<Endpoints>
    resolve(std::string_view host,
            std::string_view port) = 0;
};

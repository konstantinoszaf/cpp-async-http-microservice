// include/core/ports/dns_resolver_interface.h
#pragma once

#include "core/domain/types.h"    // ← need the complete Endpoint
#include "task.h"
#include <string_view>
#include <vector>

using Endpoints = std::vector<Endpoint>;

class IDnsResolver {
public:
    virtual ~IDnsResolver() = default;

    /// Async resolve host:port → list of Endpoint.  Empty on error.
    virtual async_task<Endpoints>
    resolve(std::string_view host,
            std::string_view port) = 0;
};

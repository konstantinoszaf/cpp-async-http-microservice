#pragma once
#include "core/domain/types.h"
#include "core/ports/task.h"

class IHandler {
public:
    virtual ~IHandler() = default;
    virtual async_task<void> handle(const Request& req, Response& res) = 0;
};
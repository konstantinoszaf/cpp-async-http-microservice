#pragma once
#include "core/ports/task.h"

class ISession {
public:
    virtual ~ISession() = default;
    virtual void read() = 0;
    virtual void write() = 0;
    virtual async_task<void> handle_request() = 0;
};
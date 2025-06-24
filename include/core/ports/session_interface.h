#pragma once
#include "core/ports/task.h"

class ISession {
public:
    virtual ~ISession() = default;
    virtual async_task<void> run() = 0;
};
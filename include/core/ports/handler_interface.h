#pragma once
#include "core/domain/types.h"

class IHandler {
public:
    virtual ~IHandler() = default;
    virtual void handle(const Request& req, Response& res) = 0;
};
#pragma once

class ISession {
public:
    virtual ~ISession() = default;
    virtual void read() = 0;
    virtual void write() = 0;
    virtual void handle_request() = 0;
};
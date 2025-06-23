#pragma once
#include "core/ports/handler_interface.h"
#include "core/domain/types.h"
#include <memory>

class IParser;
class IProviderFactory;

class ShortlyHandler : public IHandler {
public:
    ShortlyHandler(std::shared_ptr<IParser> parser, std::shared_ptr<IProviderFactory> provider);
    async_task<void> handle(const Request& req, Response& res) override;
private:
    std::shared_ptr<IParser> json_parser;
    std::shared_ptr<IProviderFactory> provider_factory;
};

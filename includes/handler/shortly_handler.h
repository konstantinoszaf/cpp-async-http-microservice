#pragma once
#include "parser/parser_interface.h"
#include "handler_interface.h"
#include "provider/provider_interface.h"
#include "factory/provider_factory.h"
#include "types.h"

class ShortlyHandler : public IHandler {
public:
    ShortlyHandler(std::shared_ptr<IParser> parser, std::shared_ptr<IProviderFactory> provider);
    void handle(const Request& req, Response& res) override;
private:
    std::shared_ptr<IParser> json_parser;
    std::shared_ptr<IProviderFactory> provider_factory;
};

#pragma once
#include "core/domain/types.h"
#include "core/ports/parser_interface.h"
#include <memory>

class IValidator;

class JsonParser : public IParser {
public:
    JsonParser(std::shared_ptr<IValidator> validator);
    std::pair<std::string, ProviderType> parse(std::string_view j_str) override;
private:
    ProviderType strToProviderType(const std::string& provider_str);
    std::shared_ptr<IValidator> validator;
};
#pragma once
#include "parser_interface.h"
#include "validator/validator_interface.h"
#include "factory/factory_interface.h"
#include <memory>

class IValidator;
enum class  ProviderType {
    BITLY   = 0,
    TINYURL = 1
};

class JsonParser : public IParser {
public:
    JsonParser(std::shared_ptr<IValidator> validator);
    std::pair<std::string, ProviderType> parse(std::string_view j_str) override;
private:
    ProviderType strToProviderType(const std::string& provider_str);
    std::shared_ptr<IValidator> validator;
};
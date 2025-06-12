#pragma once
#include "parser_interface.h"
#include "validator/validator_interface.h"
#include <memory>

class JsonParser : public IParser {
public:
    JsonParser();
    std::pair<std::string, std::string> parse(std::string_view j_str) override;
private:
    std::unique_ptr<IValidator> validator;
};
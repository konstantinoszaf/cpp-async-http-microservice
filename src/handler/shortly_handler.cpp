#include "handler/shortly_handler.h"
#include "factory/factory.h"
#include "exception/validation_exception.h"
#include "parser/json_parser.h"
#include <iostream>

ShortlyHandler::ShortlyHandler(std::shared_ptr<IParser> parser) {
    json_parser = parser;
}

void ShortlyHandler::handle(const http::request<http::string_body>& req,
                            http::response<http::string_body>& res) {
    try {
        if (req.body().empty()) throw URLShortener::exception::ValidationException("Request body is empty");

        std::cout << req.body() << '\n';
        auto [url, provider] = json_parser->parse(req.body());

        std::cout << "url: " << url << ", provider: " << static_cast<unsigned int>(provider) << '\n';
        // auto provider_class = factory.getProvider(provider)
        json::object response_json;

        response_json["status"] = "Valid Response!";
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(response_json);
        res.prepare_payload();
    } catch (const URLShortener::exception::ValidationException& e) {
        std::cout << "Error: " << e.what() << '\n';
    } catch (const boost::system::error_code e) {
        std:: cout << "Boost error: " << e.message() << '\n';
    }
}
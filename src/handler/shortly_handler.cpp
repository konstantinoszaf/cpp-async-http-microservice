#include "handler/shortly_handler.h"
#include "exception/validation_exception.h"
#include "parser/json_parser.h"
#include <iostream>

ShortlyHandler::ShortlyHandler() {
    json_parser = std::make_unique<JsonParser>();
}

void ShortlyHandler::handle(const http::request<http::string_body>& req,
                            http::response<http::string_body>& res) {
    try {
        if (req.body().empty()) throw std::runtime_error("Request body is empty");

        std::cout << req.body() << '\n';

        auto [url, provider] = json_parser->parse(req.body());

        // auto provider = Providers::getProvider(req.body())
        std::cout << "url: " << url << ", provider: " << provider << '\n';
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
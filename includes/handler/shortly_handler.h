#pragma once
#include "handler_interface.h"
#include <boost/json.hpp>

namespace json = boost::json;

class ShortlyHandler : public IHandler {
public:
    void handle(const http::request<http::string_body>& req,
                http::response<http::string_body>& res) override {

        try {
            if (req.body().empty()) throw std::runtime_error("Request body is empty");

            std::cout << req.body() << '\n';

            json::value jv = json::parse(req.body());
            json::object const& obj = jv.as_object();

            std::string key = obj.at("key").as_string().c_str();
            std::string val = obj.at("val").as_string().c_str();
            // auto provider = Providers::getProvider(req.body())
            std::cout << "key: " << key << ", val: " << val << '\n';
            json::object response_json;

            response_json["status"] = "Valid Response!";
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = json::serialize(response_json);
            res.prepare_payload();
        } catch (const std::runtime_error& e) {
            std::cout << "Error: " << e.what() << '\n';
        } catch (const boost::json::system_error& e) {
            std:: cout << "Boost error: " << e.message() << '\n';
        }
    }
};

/**
 * TINYURL
 * 
 curl -X 'POST' \
  'https://api.tinyurl.com/create' \
  -H 'accept: application/json' \
  -H 'Authorization: Bearer U8ecENf0tMiE09QIezSL2aIrB6EdzGAjX7o8tJpEfFsYIIEqTyoIECQJZNwj' \
  -H 'Content-Type: application/json' \
  -d '{
  "url": "https://www.google.com" }'

curl -X 'POST' 'https://api.tinyurl.com/create?api_token=TMP_TOKEN'  \
-H 'accept: application/json' \
-H 'Authorization: Bearer BEARER' \
-H 'Content-Type: application/json' \
-d \
'{
  "url": "https://www.example.com/my-really-long-link-that-I-need-to-shorten/84378949"
}'


curl -vvv -X 'POST' https://api-ssl.bitly.com/v4/shorten \
-H 'Authorization: Bearer 1a02f1ab7543c77ea3a524af5ea84f0f146f4d13' \
-H 'Content-Type: application/json' \
-X POST \
-d '{
  "long_url": "https://dev.bitly.com"
}'
 */
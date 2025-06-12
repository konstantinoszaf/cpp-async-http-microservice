#pragma once
#include "handler_interface.h"
#include <boost/beast.hpp>
#include "parser/parser_interface.h"

namespace http = boost::beast::http;

class ShortlyHandler : public IHandler {
public:
    void handle(const http::request<http::string_body>& req,
                http::response<http::string_body>& res) override;
    ShortlyHandler();
private:
    std::unique_ptr<IParser> json_parser;
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
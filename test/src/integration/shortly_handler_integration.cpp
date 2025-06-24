#include "core/usecases/shortly_handler.h"
#include "infra/parser/json_parser.h"
#include "infra/provider/provider.h"
#include "infra/exception/exceptions.h"
#include "infra/factory/provider_factory.h"
#include "infra/validator/json_validator.h"
#include "infra/validator/key_count_rule.h"
#include "infra/validator/key_rule.h"
#include "core/ports/http_client_interface.h"
#include "core/ports/env_reader_interface.h"

#include <boost/json.hpp>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <gtest/gtest.h>

using namespace URLShortener;
namespace json = boost::json;

class StubHttpClient : public IHttpClient {
public:
    Response next_response;
    void set_response(Response resp) { next_response = std::move(resp); }

    boost::asio::awaitable<Response> post(
        std::string_view body,
        RequestInfo& info) override {
            co_return next_response;
        }
};

class StubEnv: public IEnvReader {
public:
    std::string get(std::string_view) override { return "dummy-key"; }
};

class IntegrationTest : public ::testing::Test {
protected:
    boost::asio::io_context ioc;
    std::shared_ptr<JsonParser> parser;
    std::shared_ptr<StubEnv> env;
    std::shared_ptr<StubHttpClient> http_client;
    std::shared_ptr<ProviderFactory> factory;
    std::shared_ptr<ShortlyHandler> handler;

    void SetUp() override {
        std::vector<std::unique_ptr<IValidationRule>> rules;
        rules.push_back(std::make_unique<KeyCountRule>(1, 2));
        rules.push_back(std::make_unique<KeyRule>(
            std::vector<std::string>{"url"}, // mandatory
            std::unordered_set<std::string>{"provider"} // optional
        ));

        auto validator = std::make_shared<JsonValidator>();
        for (auto& r : rules) validator->add_rule(std::move(r));

        parser = std::make_shared<JsonParser>(validator);
        env = std::make_shared<StubEnv>();
        http_client = std::make_shared<StubHttpClient>();
        factory = std::make_shared<ProviderFactory>(http_client, env);
        handler = std::make_shared<ShortlyHandler>(parser, factory);
    }

    Response run_handler(std::string_view payload) {
        Request req;
        req.body = payload;
        Response resp;
        boost::asio::co_spawn(ioc,
            [&, this]() -> boost::asio::awaitable<void> {
                co_await handler->handle(req, resp);
                co_return;
            },
            boost::asio::detached
        );
        ioc.run();
        return resp;
    }
};

// happy path: stub HTTP returns 200 + JSON with short URL
TEST_F(IntegrationTest, HappyPath) {
  // simulate provider API:
  json::object api_json;
  api_json["link"] = "short.ly/xyz";
  http_client->set_response({ HTTP::code::OK, {}, json::serialize(api_json) });

  // real JSON parser will extract url + provider:
  auto body = R"({"url":"http://example.com","provider":"bitly"})";
  auto resp = run_handler(body);

  EXPECT_EQ(resp.status_code, HTTP::code::OK);
  EXPECT_EQ(resp.headers["Content-Type"], "application/json");

  auto out = json::parse(resp.body).as_object();
  EXPECT_EQ(out["result"].as_int64(), 0);
  EXPECT_EQ(out["reason"].as_string(), "");
  auto data = out["data"].as_object();
  EXPECT_EQ(data["shortened"].as_string(), "short.ly/xyz");
  EXPECT_EQ(data["url"].as_string(), "http://example.com");
}

// provider-side HTTP error -> ProviderException → mapping to HTTP
TEST_F(IntegrationTest, UpstreamError) {
  http_client->set_response({ HTTP::code::ServiceUnavailable, {}, "down for maintenance" });

  auto resp = run_handler(R"({"url":"u","provider":"bitly"})");
  EXPECT_EQ(resp.status_code, HTTP::code::ServiceUnavailable);

  auto out = json::parse(resp.body).as_object();
  EXPECT_EQ(out["result"].as_int64(), 1);
  EXPECT_EQ(out["reason"].as_string(), "down for maintenance");
}

// empty URL passed into provider -> validation ProviderException
TEST_F(IntegrationTest, ValidationInProvider) {
  // If request.body maps to an empty URL, JSON parser would throw or provider would:
  http_client->set_response({ HTTP::code::OK, {}, R"({"link":""})" });
  // But better: trigger empty-URL logic directly by passing empty url string
  auto resp = run_handler(R"({"url":"","provider":"bitly"})");

  EXPECT_EQ(resp.status_code, HTTP::code::BadRequest);
  auto out = json::parse(resp.body).as_object();
  EXPECT_EQ(out["reason"].as_string(), "url value can not be empty");
}

// invalid JSON in request → parser throws → 400 BadRequest
TEST_F(IntegrationTest, BadJsonRequest) {
  auto resp = run_handler("not valid json");
  EXPECT_EQ(resp.status_code, HTTP::code::BadRequest);
  auto out = json::parse(resp.body).as_object();
  EXPECT_EQ(out["reason"].as_string(), "Invalid JSON format");
}
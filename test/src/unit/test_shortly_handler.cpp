#include "mock_provider.h"
#include "mock_provider_factory.h"
#include "mock_parser.h"
#include "core/usecases/shortly_handler.h"
#include "infra/exception/exceptions.h"
#include <boost/json.hpp>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <memory>

namespace json = boost::json;

class TestShortlyHandler : public ::testing::Test {
public:
    std::shared_ptr<MockParser> parser;
    std::shared_ptr<MockProviderFactory> factory;
    std::shared_ptr<MockProvider> provider;
    std::shared_ptr<ShortlyHandler> shortly;
    void SetUp() override {
        parser = std::make_shared<MockParser>();
        factory = std::make_shared<MockProviderFactory>();
        provider = std::dynamic_pointer_cast<MockProvider>(factory->createProvider(ProviderType::MAX/*unused*/));
        shortly = std::make_shared<ShortlyHandler>(parser, factory);
    }
};

TEST_F(TestShortlyHandler, EmptyBody) {
    Request  req{ .body = "" };
    Response resp;

    boost::asio::io_context ioc;
    boost::asio::co_spawn(ioc, [&]() -> boost::asio::awaitable<void> {
        co_await shortly->handle(req, resp);

        EXPECT_EQ(resp.status_code, HTTP::code::BadRequest);
        EXPECT_EQ(resp.headers["Content-Type"], "application/json");

        // {"result":1,"reason":"Request body is empty"}
        json::object expect;
        expect["result"] = 1;
        expect["reason"] = "Request body is empty";
        EXPECT_EQ(resp.body, json::serialize(expect));

        co_return;
    }, boost::asio::detached);
    ioc.run();
}

TEST_F(TestShortlyHandler, ParserThrowsValidation) {
    Request  req{ .body = "non-empty" };
    Response resp;

    EXPECT_CALL(*parser, parse(::testing::_))
        .WillOnce(::testing::Throw(
            URLShortener::exception::ValidationException{"bad json"}));

    boost::asio::io_context ioc;
    boost::asio::co_spawn(ioc, [&]() -> boost::asio::awaitable<void> {
        co_await shortly->handle(req, resp);

        EXPECT_EQ(resp.status_code, HTTP::code::BadRequest);
        EXPECT_EQ(resp.headers["Content-Type"], "application/json");

        json::object expect;
        expect["result"] = 1;
        expect["reason"] = "bad json";
        EXPECT_EQ(resp.body, json::serialize(expect));

        co_return;
    }, boost::asio::detached);
    ioc.run();
}

TEST_F(TestShortlyHandler, ProviderShortenThrowsBadRequest) {
    Request  req{ .body = R"({"url":"some","provider":"bitly"})" };
    Response resp;

    EXPECT_CALL(*parser, parse(::testing::_))
        .WillOnce(::testing::Return(std::make_pair(std::string{"some"}, ProviderType::BITLY)));

    // simulate Provider::shorten throwing on empty‐URL validation
    EXPECT_CALL(*provider, shorten(::testing::_))
        .WillOnce(::testing::Invoke([](std::string_view) -> async_task<std::string> {
            throw URLShortener::exception::ProviderException(
                "url value can not be empty",
                HTTP::code::BadRequest
            );
            co_return std::string{};
        }));

    boost::asio::io_context ioc;
    boost::asio::co_spawn(ioc, [&]() -> boost::asio::awaitable<void> {
        co_await shortly->handle(req, resp);

        EXPECT_EQ(resp.status_code, HTTP::code::BadRequest);
        EXPECT_EQ(resp.headers["Content-Type"], "application/json");

        json::object expect;
        expect["result"] = 1;
        expect["reason"] = "url value can not be empty";
        EXPECT_EQ(resp.body, json::serialize(expect));

        co_return;
    }, boost::asio::detached);
    ioc.run();
}

TEST_F(TestShortlyHandler, ProviderShortenThrowsServiceUnavailable) {
    Request  req{ .body = R"({"url":"u","provider":"bitly"})" };
    Response resp;

    EXPECT_CALL(*parser, parse(::testing::_))
        .WillOnce(::testing::Return(std::make_pair(std::string{"u"}, ProviderType::BITLY)));

    EXPECT_CALL(*provider, shorten(::testing::_))
        .WillOnce(::testing::Invoke([](std::string_view) -> async_task<std::string> {
            throw URLShortener::exception::ProviderException(
                "upstream fail",
                HTTP::code::ServiceUnavailable
            );
            co_return std::string{};
        }));

    boost::asio::io_context ioc;
    boost::asio::co_spawn(ioc, [&]() -> boost::asio::awaitable<void> {
        co_await shortly->handle(req, resp);

        EXPECT_EQ(resp.status_code, HTTP::code::ServiceUnavailable);
        EXPECT_EQ(resp.headers["Content-Type"], "application/json");

        json::object expect;
        expect["result"] = 1;
        expect["reason"] = "upstream fail";
        EXPECT_EQ(resp.body, json::serialize(expect));

        co_return;
    }, boost::asio::detached);
    ioc.run();
}

TEST_F(TestShortlyHandler, HappyPath) {
    std::string input_url = "www.test.gr";
    std::string short_url = "short.ly";

    
    json::object data_object;
    data_object["url"] = input_url;
    data_object["shortened"] = short_url;

    json::object expected_body;
    expected_body["result"] = 0;
    expected_body["reason"] = "";
    expected_body["data"] = std::move(data_object);

    EXPECT_CALL(*parser, parse(::testing::_))
    .WillOnce(::testing::Return(
        std::make_pair(input_url, ProviderType::BITLY)
    ));

    EXPECT_CALL(*provider, shorten(::testing::Eq(input_url)))
    .WillOnce(::testing::Invoke([short_url](std::string_view) -> async_task<std::string> {
        co_return short_url;
    }));

    Request request { .body = "not empty string" };
    Response response;

    boost::asio::io_context ioc;
    boost::asio::co_spawn(ioc, [&]() -> boost::asio::awaitable<void> {
        co_await shortly->handle(request, response);

        EXPECT_EQ(response.status_code, HTTP::code::OK);
        EXPECT_EQ(response.headers["Content-Type"], "application/json");
        EXPECT_EQ(response.body, json::serialize(expected_body));
        co_return;
    }, boost::asio::detached);

    ioc.run();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>
#include "parser/json_parser.h"
#include "exception/validation_exception.h"
#include "mock_validator.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Throw;

using URLShortener::exception::ValidationException;

class JsonParserTest : public ::testing::Test {
protected:
    std::shared_ptr<MockValidator> validator;
    std::shared_ptr<JsonParser> parser;
    void SetUp() override {
        validator = std::make_shared<MockValidator>();
        parser = std::make_shared<JsonParser>(validator);
    }
};

TEST_F(JsonParserTest, ParseValidJsonWithoutProvider) {
    EXPECT_CALL(*validator, validate(_)).Times(1);
    auto [url, provider] = parser->parse(R"({ "url":"http://example.com" })");
    EXPECT_EQ(url, "http://example.com");
    EXPECT_EQ(provider, ProviderType::BITLY);
}

TEST_F(JsonParserTest, ParseValidJsonWithTinyurlProvider) {
    EXPECT_CALL(*validator, validate(_)).Times(1);
    auto [url, provider] = parser->parse(R"({ "url":"abc", "provider":"tinyurl" })");
    EXPECT_EQ(url, "abc");
    EXPECT_EQ(provider, ProviderType::TINYURL);
}

TEST_F(JsonParserTest, UnknownProviderDefaultsToBitly) {
    EXPECT_CALL(*validator, validate(_)).Times(1);
    auto [url, provider] = parser->parse(R"({ "url":"xyz", "provider":"unknown" })");
    EXPECT_EQ(url, "xyz");
    EXPECT_EQ(provider, ProviderType::BITLY);
}

// --- invalid JSON syntax/object shape: parse fails before validate() ---

TEST_F(JsonParserTest, ThrowsOnNonObjectJson) {
    EXPECT_CALL(*validator, validate(_)).Times(0);
    EXPECT_THROW(parser->parse(R"( [1,2,3] )"), ValidationException);
}

TEST_F(JsonParserTest, ThrowsOnMalformedJson) {
    EXPECT_CALL(*validator, validate(_)).Times(0);
    EXPECT_THROW(parser->parse(R"( { "url":"u", )"), ValidationException);
}

// --- validation‐triggered failures: have validator throw ---

TEST_F(JsonParserTest, ThrowsOnTooFewKeys) {
    EXPECT_CALL(*validator, validate(_))
        .WillOnce(Throw(ValidationException("too few keys")));
    EXPECT_THROW(parser->parse(R"( {} )"), ValidationException);
}

TEST_F(JsonParserTest, ThrowsOnTooManyKeys) {
    EXPECT_CALL(*validator, validate(_))
        .WillOnce(Throw(ValidationException("too many keys")));
    EXPECT_THROW(parser->parse(
        R"( { "url":"u", "provider":"p", "extra":"x" } )"
    ), ValidationException);
}

TEST_F(JsonParserTest, ThrowsOnMissingUrlKey) {
    EXPECT_CALL(*validator, validate(_))
        .WillOnce(Throw(ValidationException("missing url")));
    EXPECT_THROW(parser->parse(R"( { "provider":"tinyurl" } )"), ValidationException);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

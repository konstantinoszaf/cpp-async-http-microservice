#include "mock_http_client.h"
#include "mock_env_reader.h"
#include "mock_redis_cache.h"
#include "infra/provider/tinyurl.h"

class TestableTinyURL : public TinyURL {
public:
    using TinyURL::TinyURL;
    using TinyURL::get_error_message;
};

class TestBitly : public ::testing::Test {
public:
    std::shared_ptr<MockHttpClient> http_client;
    std::shared_ptr<MockEnvReader> env;
    std::shared_ptr<MockCache> redis;
    Response response;
    void SetUp() override {
        env = std::make_shared<MockEnvReader>();
        EXPECT_CALL(*env, get(::testing::_))
        .WillRepeatedly(::testing::Return("TOKEN"));
        http_client = std::make_shared<MockHttpClient>();
        redis = std::make_shared<MockCache>();
    }

    void TearDown() override {}
};
class TinyURLErrorTest
  : public ::testing::TestWithParam<std::pair<std::string, std::string>> {
protected:
    std::shared_ptr<MockHttpClient>  http_client;
    std::shared_ptr<MockEnvReader>   env;
    std::shared_ptr<MockCache> redis;
public:
    void SetUp() override {
        env = std::make_shared<MockEnvReader>();
        EXPECT_CALL(*env, get(::testing::_))
        .WillRepeatedly(::testing::Return("TOKEN"));
        http_client = std::make_shared<MockHttpClient>();
        redis = std::make_shared<MockCache>();
    }
};

TEST_P(TinyURLErrorTest, ParsesErrorMessages) {
    auto const& [payload, expected] = GetParam();
    TestableTinyURL provider{http_client, env, redis};
    EXPECT_EQ(expected, provider.get_error_message(payload));
}

INSTANTIATE_TEST_SUITE_P(
    VariousErrorScenarios,
    TinyURLErrorTest,
    ::testing::Values(
      std::make_pair(
        R"({"data":[],"code":5,"errors":["The URL field is required."]})",
        "The URL field is required."
      ),
      std::make_pair(
        R"({"data":[],"code":0,"errors":[]})",
        "Unknown error"
      ),
      std::make_pair(
        R"({"data":[],"code":1})",
        "Unknown error"
      ),
      std::make_pair(
        R"({"errors":["Foo failed","Bar failed"]})",
        "Foo failed,Bar failed"
      )
    )
);

// test/src/test_key_rule.cpp

#include <gtest/gtest.h>
#include <boost/json.hpp>
#include "infra/validator/key_rule.h"

namespace json = boost::json;

// Fixture for KeyRule tests
class TestKeyRule : public ::testing::Test {
protected:
    void SetUp() override {
        // mandatory: "id", "name"; optional: "age", "country"
        mandatory = {"id", "name"};
        optional  = {"age", "country"};
        rule = std::make_unique<KeyRule>(mandatory, optional);
    }

    std::vector<std::string> mandatory;
    std::unordered_set<std::string> optional;
    std::unique_ptr<KeyRule> rule;
};

// 1) Missing a mandatory key should fail
TEST_F(TestKeyRule, MissingMandatoryKey) {
    json::object obj;
    obj["id"] = 123;                   // missing "name"
    std::string err;
    EXPECT_FALSE(rule->is_valid(obj, err));
    EXPECT_EQ(err, "JSON must contain key name");
}

// 2) Contains an unexpected key should fail
TEST_F(TestKeyRule, UnexpectedKey) {
    json::object obj;
    obj["id"]      = 1;
    obj["name"]    = "Alice";
    obj["extra"]   = "oops";           // not allowed
    std::string err;
    EXPECT_FALSE(rule->is_valid(obj, err));
    EXPECT_EQ(err, "Key extra is not allowed.");
}

// 3) Exactly mandatory keys only should pass
TEST_F(TestKeyRule, OnlyMandatoryKeys) {
    json::object obj;
    obj["id"]   = 42;
    obj["name"] = "Bob";
    std::string err;
    EXPECT_TRUE(rule->is_valid(obj, err));
    EXPECT_TRUE(err.empty());
}

// 4) Mandatory + optional keys should pass
TEST_F(TestKeyRule, MandatoryAndOptional) {
    json::object obj;
    obj["id"]      = 42;
    obj["name"]    = "Carol";
    obj["age"]     = 30;
    obj["country"] = "USA";
    std::string err;
    EXPECT_TRUE(rule->is_valid(obj, err));
    EXPECT_TRUE(err.empty());
}

// 5) Missing all keys should report first missing mandatory key
TEST_F(TestKeyRule, MissingAllMandatoryKeys) {
    json::object obj;                  // completely empty
    std::string err;
    EXPECT_FALSE(rule->is_valid(obj, err));
    // Since we check "id" first in the set, err should mention "id"
    EXPECT_EQ(err, "JSON must contain key id");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

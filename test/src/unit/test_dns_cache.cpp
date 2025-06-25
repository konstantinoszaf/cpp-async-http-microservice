#include "infra/network/dns_cache.h"
#include "core/ports/dns_resolver_interface.h"

#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_future.hpp>
#include <chrono>
#include <thread>

using namespace std::chrono;

struct FakeResolver : IDnsResolver {
  int calls = 0;
  async_task<Endpoints> resolve(std::string_view host,
                                std::string_view port) override
  {
    ++calls;
    Endpoints eps;
    unsigned base = static_cast<unsigned>(std::stoi(std::string(port)));
    eps.push_back(Endpoint{std::string(host), base + calls});
    co_return eps;
  }
};

struct DnsCacheTest : testing::Test {
    boost::asio::io_context ioc;
    std::shared_ptr<FakeResolver> fake = std::make_shared<FakeResolver>();

    DnsLRUCache cache{ fake, /*max_size=*/2, seconds{1} };

    template<typename T>
    T run_awaitable(boost::asio::awaitable<T> a) {
        auto fut = boost::asio::co_spawn(ioc,
        std::move(a),
        boost::asio::use_future
        );
        ioc.run();
        ioc.restart();
        return fut.get();
    }
};

TEST_F(DnsCacheTest, MissThenHit) {
    // First call -> miss, fake->calls becomes 1
    auto r1 = run_awaitable(cache.resolve("h","1000"));
    EXPECT_EQ(r1.size(), 1);
    EXPECT_EQ(r1[0].port, 1000 + 1);
    EXPECT_EQ(fake->calls, 1);

    // Second call within TTL -> hit, fake->calls remains 1
    auto r2 = run_awaitable(cache.resolve("h","1000"));
    EXPECT_EQ(r2[0].port, 1001);
    EXPECT_EQ(fake->calls, 1);
}

TEST_F(DnsCacheTest, EvictLRUWhenFull) {
    // Fill two slots
    run_awaitable(cache.resolve("a","1")); EXPECT_EQ(fake->calls,1);
    run_awaitable(cache.resolve("b","1")); EXPECT_EQ(fake->calls,2);

    // Both still cached
    run_awaitable(cache.resolve("a","1")); EXPECT_EQ(fake->calls,2);
    run_awaitable(cache.resolve("b","1")); EXPECT_EQ(fake->calls,2);

    // Insert third distinct key → evict LRU ("a:1")
    run_awaitable(cache.resolve("c","1")); EXPECT_EQ(fake->calls,3);

    // "a:1" should be gone
    run_awaitable(cache.resolve("a","1")); EXPECT_EQ(fake->calls,4);
}

TEST_F(DnsCacheTest, ConcurrentResolveShared) {
    // Kick off two concurrent resolves for the same key
    auto fut1 = boost::asio::co_spawn(ioc,
        cache.resolve("x","3000"), boost::asio::use_future);
    auto fut2 = boost::asio::co_spawn(ioc,
        cache.resolve("x","3000"), boost::asio::use_future);

    ioc.run();
    ioc.restart();

    auto e1 = fut1.get();
    auto e2 = fut2.get();

    // Both get the same endpoint, only one underlying call
    EXPECT_EQ(e1.size(),1);
    EXPECT_EQ(e1[0].port, 3001);
    EXPECT_EQ(e2.size(),1);
    EXPECT_EQ(e2[0].port, 3001);
    EXPECT_EQ(fake->calls,1);
}

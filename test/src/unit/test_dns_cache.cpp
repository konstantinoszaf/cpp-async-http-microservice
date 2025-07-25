#include "infra/cache/dns_cache.h"
#include <gtest/gtest.h>

static Endpoints makeEndpoints(std::initializer_list<Endpoint> list) {
    return Endpoints(list);
}

TEST(DnsCacheTest, FindEntryReturnsEmptyWhenNotPresent) {
    DnsCache cache;
    // No entry added, should return empty Endpoints
    Endpoints result = cache.find_entry("example.com", "80");
    EXPECT_TRUE(result.empty());
}

TEST(DnsCacheTest, UpdateOrAddAddsNewEntry) {
    DnsCache cache;
    // Add two endpoints with specific address and port
    Endpoints eps = makeEndpoints({
        Endpoint{"192.168.0.1", 443},
        Endpoint{"192.168.0.2", 443}
    });

    cache.update_or_add_entry("example.com", "443", eps);
    Endpoints found = cache.find_entry("example.com", "443");

    EXPECT_EQ(found.size(), 2u);
    // Verify contents
    EXPECT_EQ(found[0].address, "192.168.0.1");
    EXPECT_EQ(found[0].port, 443u);
    EXPECT_EQ(found[1].address, "192.168.0.2");
    EXPECT_EQ(found[1].port, 443u);
}

TEST(DnsCacheTest, UpdateOrAddOverwritesExistingEntry) {
    DnsCache cache;
    // Initial endpoints
    Endpoints first = makeEndpoints({ Endpoint{"10.0.0.1", 53} });
    cache.update_or_add_entry("example.org", "53", first);
    Endpoints found1 = cache.find_entry("example.org", "53");
    EXPECT_EQ(found1.size(), 1u);
    EXPECT_EQ(found1[0].address, "10.0.0.1");
    EXPECT_EQ(found1[0].port, 53u);

    // Overwrite with new endpoints
    Endpoints second = makeEndpoints({
        Endpoint{"10.0.0.2", 53},
        Endpoint{"10.0.0.3", 53}
    });
    cache.update_or_add_entry("example.org", "53", second);
    Endpoints found2 = cache.find_entry("example.org", "53");
    EXPECT_EQ(found2.size(), 2u);
    EXPECT_EQ(found2[0].address, "10.0.0.2");
    EXPECT_EQ(found2[0].port, 53u);
    EXPECT_EQ(found2[1].address, "10.0.0.3");
    EXPECT_EQ(found2[1].port, 53u);
}

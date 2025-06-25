#pragma once

#include "core/ports/dns_cache_interface.h"
#include "core/ports/dns_resolver_interface.h"
#include <chrono>
#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <memory>

class DnsLRUCache : public IDnsCache {
public:
    DnsLRUCache(std::shared_ptr<IDnsResolver> resolver,
                size_t max_size,
                std::chrono::seconds ttl);

    async_task<Endpoints>
    resolve(std::string_view host,
            std::string_view port) override;

private:
    struct Entry {
        Endpoints endpoints;
        std::chrono::steady_clock::time_point expires;
        std::list<std::string>::iterator lru_it;
    };

    std::shared_ptr<IDnsResolver> resolver_;
    size_t max_size_;
    std::chrono::seconds ttl_;

    std::mutex mtx_;
    std::unordered_map<std::string, Entry> cache_;
    std::list<std::string> lru_;
};

// infra/network/dns_cache.cpp
#include "infra/network/dns_cache.h"

using namespace std::chrono;

DnsLRUCache::DnsLRUCache(std::shared_ptr<IDnsResolver> resolver,
                         size_t max_size,
                         seconds ttl)
    : resolver_(std::move(resolver))
    , max_size_(max_size)
    , ttl_(ttl)
{}

async_task<Endpoints>
DnsLRUCache::resolve(std::string_view host, std::string_view port) {
    auto key = std::string(host) + ":" + std::string(port);
    auto now = steady_clock::now();

    // 1) Fast‐path cache hit
    {
        std::lock_guard lock(mtx_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            if (it->second.expires > now) {
                // Move to front of LRU list
                lru_.splice(lru_.begin(), lru_, it->second.lru_it);
                co_return it->second.endpoints;
            }
            // expired → evict
            lru_.erase(it->second.lru_it);
            cache_.erase(it);
        }
    }

    // 2) Miss, call underlying resolver
    auto eps = co_await resolver_->resolve(host, port);

    // 3) Insert (with eviction if at capacity)
    {
        std::lock_guard lock(mtx_);
        if (cache_.size() >= max_size_) {
            auto old_key = lru_.back();
            lru_.pop_back();
            cache_.erase(old_key);
        }

        lru_.push_front(key);
        cache_[key] = Entry{
            std::move(eps),        // take ownership of the endpoints
            now + ttl_,            // set new expiration
            lru_.begin()           // iterator into the front of lru_
        };
    }

    co_return cache_[key].endpoints;
}

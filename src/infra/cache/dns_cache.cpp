#include "infra/cache/dns_cache.h"

Endpoints DnsCache::find_entry(std::string_view domain, std::string_view port) {
    std::shared_lock<std::shared_mutex> lk(entry_mtx);
    auto key = std::string(domain) + ":" + std::string(port);
    const auto it = cache_.find(key);

    return (it == cache_.end()) ? Endpoints() : it->second;
}

void DnsCache::update_or_add_entry(std::string_view domain, std::string_view port, Endpoints& endpoints) {
    std::lock_guard<std::shared_mutex> lk(entry_mtx);
    auto key = std::string(domain) + ":" + std::string(port);
    cache_[key] = std::move(endpoints);
}

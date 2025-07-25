#pragma once

#include "core/ports/dns_cache_interface.h"
#include <shared_mutex>
#include <string>
#include <unordered_map>

class DnsCache : public IDnsCache {
public:
    DnsCache() = default;

    Endpoints find_entry(std::string_view domain, std::string_view port) override;
    void update_or_add_entry(std::string_view domain, std::string_view port, Endpoints& endpoints) override;
private:
    std::shared_mutex entry_mtx;
    std::unordered_map<std::string, Endpoints> cache_;
};

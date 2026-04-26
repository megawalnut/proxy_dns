#include "../../headers/DNS/dnsCache.h"

void DNSCache::put(const Cache::Key& key, const Cache::Record& rec) {
    Cache::Record tmp = rec;
    tmp.created = std::chrono::steady_clock::now();

    // if record is exists -> replace
    m_records[key] = tmp;
}

std::optional<Cache::Record> DNSCache::get(const Cache::Key& key) {
    auto it = m_records.find(key);
    if(it == m_records.end()) {
        return std::nullopt;
    }

    if(isExpired(it->second)) {
        m_records.erase(it);
        return std::nullopt;
    }

    return it->second;
}

bool DNSCache::isExpired(const Cache::Record& rec) const {
    return std::chrono::steady_clock::now() - rec.created > 
           std::chrono::seconds(rec.ttl);   // if now > last valid time 
};
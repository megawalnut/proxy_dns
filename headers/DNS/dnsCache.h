#ifndef DNSCACHE_H
#define DNSCACHE_H

#include <ldns/ldns.h>
#include <unordered_map>
#include <optional>

#include "../utils.h"

using namespace Utils;

class DNSCache final {
public:
    struct RHash {
        std::size_t operator()(const Cache::Key& key) const {
            std::size_t h1 = std::hash<std::string>()(key.domain);
            std::size_t h2 = std::hash<uint32_t>()(static_cast<uint32_t>(key.type));
            return h1 ^ (h2 << 1);
        }
    };

public:
    void put(const Cache::Record& rec);
    std::optional<Cache::Record> get(const Cache::Key& key);

private:
    bool isExpired(const Cache::Record& rec) const;

private:
    std::unordered_map<Cache::Key, Cache::Record, RHash> m_records;
};

#endif // DNSCACHE_H
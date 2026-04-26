#include "../../headers/Types/a.h"

#include <iostream>
#include <string>

#include "../../headers/DNS/dnsDispatcher.h"

A::A(DNSCache& cache) : m_cache(cache) {}

bool A::handle(const ldns_rr* rr) const {
    if(!rr) return false;

    Cache::Key key;

    // get type
    const ldns_rr_type type = ldns_rr_get_type(rr);
    if(type != ldns_rr_type::LDNS_RR_TYPE_A) {
        std::cerr << "A::handle: Invalid type" << static_cast<uint32_t>(type) << std::endl;
        return false;
    }
    
    // get domain
    const ldns_rdf* domain = ldns_rr_owner(rr);
    if(!domain) return false;

    char* c_name = ldns_rdf2str(domain);
    if (!c_name) return false;

    std::string name(c_name);
    free(c_name);

    if (!name.empty() && name.back() == '.') {
        name.pop_back();
    }

    // create key & find cache record
    key.domain = name;
    key.type = DNSDispatcher::maptype(type);

    auto rec = m_cache.get(key);
    if(rec) {
        std::cout << "A::handle: Find in cache";
        return true;
    }
    
    std::cout << "A::handle: not found in cache";
    return false;
}
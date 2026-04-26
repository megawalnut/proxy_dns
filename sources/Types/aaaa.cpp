#include "../../headers/Types/aaaa.h"

#include <iostream>

AAAA::AAAA(DNSCache& cache) : m_cache(cache) {}

bool AAAA::handle(const ldns_rr* rr) const {
    
}
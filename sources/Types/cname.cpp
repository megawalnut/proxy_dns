#include "../../headers/Types/cname.h"

#include <iostream>

CNAME::CNAME(DNSCache& cache) : m_cache(cache) {}

bool CNAME::handle(const ldns_rr* rr) const {
    
}
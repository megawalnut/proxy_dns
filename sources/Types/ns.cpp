#include "../../headers/Types/ns.h"

#include <iostream>

NS::NS(DNSCache& cache) : m_cache(cache) {}

bool NS::handle(const ldns_rr* rr) const {
    
}
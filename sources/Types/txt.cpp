#include "../../headers/Types/txt.h"

#include <iostream>

TXT::TXT(DNSCache& cache) : m_cache(cache) {}

bool TXT::handle(const ldns_rr* rr) const {
    
}
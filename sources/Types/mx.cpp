#include "../../headers/Types/mx.h"

#include <iostream>

MX::MX(DNSCache& cache) : m_cache(cache) {}

bool MX::handle(const ldns_rr* rr) const {
    
}
#ifndef AAAA_H
#define AAAA_H

#include "iType.h"
#include "../DNS/dnsCache.h"

class AAAA final : public IType {
public:
    AAAA(DNSCache& cache);
    bool handle(const ldns_rr* rr) const override;

private:
    DNSCache& m_cache;
};

#endif // AAAA_H
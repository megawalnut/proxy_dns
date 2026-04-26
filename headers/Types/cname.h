#ifndef CNAME_H
#define CNAME_H

#include "iType.h"
#include "../DNS/dnsCache.h"

class CNAME final : public IType {
public:
    CNAME(DNSCache& cache);
    bool handle(const ldns_rr* rr) const override;

private:
    DNSCache& m_cache;
};

#endif // CNAME_H
#ifndef MX_H
#define MX_H

#include "iType.h"
#include "../DNS/dnsCache.h"

class MX final : public IType {
public:
    MX(DNSCache& cache);
    bool handle(const ldns_rr* rr) const override;

private:
    DNSCache& m_cache;
};

#endif // MX_H
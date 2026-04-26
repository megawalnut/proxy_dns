#ifndef A_H
#define A_H

#include "iType.h"
#include "../DNS/dnsCache.h"

class A final : public IType {
public:
    A(DNSCache& cache);
    bool handle(const ldns_rr* rr) const override;

private:
    DNSCache& m_cache;
};

#endif // A_H
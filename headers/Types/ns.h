#ifndef NS_H
#define NS_H

#include "iType.h"
#include "../DNS/dnsCache.h"

class NS final : public IType {
public:
    NS(DNSCache& cache);
    bool handle(const ldns_rr* rr) const override;

private:
    DNSCache& m_cache;
};

#endif // NS_H
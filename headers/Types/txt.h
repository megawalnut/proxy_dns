#ifndef TXT_H
#define TXT_H

#include "iType.h"
#include "../DNS/dnsCache.h"

class TXT final : public IType {
public:
    TXT(DNSCache& cache);
    bool handle(const ldns_rr* rr) const override;

private:
    DNSCache& m_cache;
};

#endif // TXT_H
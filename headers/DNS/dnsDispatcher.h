#ifndef DNSDISPATCHER_H
#define DNSDISPATCHER_H

#include <ldns/ldns.h>
#include <map>

#include "dnsParser.h"
#include "../Types/iType.h"
#include "../Types/a.h"
#include "../Types/aaaa.h"
#include "../Types/mx.h"
#include "../Types/txt.h"
#include "../Types/cname.h"
#include "../Types/ns.h"

class DNSDispatcher final {
public:
    enum class DNSCachingTypes : uint32_t {
        Unknown = 0,
        A,
        AAAA,
        MX,
        TXT,
        CNAME,
        NS
    };

    DNSDispatcher();

    void dispatch(const DNSParser::DNSptr& pkt) const;

private:
    static inline DNSCachingTypes maptype(ldns_rr_type type);

private:
    std::map<DNSCachingTypes, std::unique_ptr<IType>> m_handlers;
};

#endif // DNSDISPATCHER_H
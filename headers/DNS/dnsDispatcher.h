#ifndef DNSDISPATCHER_H
#define DNSDISPATCHER_H

#include <ldns/ldns.h>
#include <map>

#include "../utils.h"
#include "dnsParser.h"
#include "dnsCache.h"
#include "dnsResolve.h"

using namespace Utils;

class DNSDispatcher final {
public:
    DNSDispatcher();

    void dispatch(const DNSParser::DNSptr& pkt);
    //static inline DNS::Types maptype(ldns_rr_type type);

private:
    std::optional<Cache::Record> lookupCache(const ldns_rr* rr);

private:
    //std::map<DNS::Types, std::unique_ptr<IType>> m_handlers;
    DNSCache m_cache;
    DNSResolve m_resolve;
};

#endif // DNSDISPATCHER_H
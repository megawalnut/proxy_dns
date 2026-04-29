#ifndef DNSDISPATCHER_H
#define DNSDISPATCHER_H

#include <ldns/ldns.h>
#include <map>

#include "dnsParser.h"
#include "dnsResolver.h"
#include "dnsCache.h"
#include "../utils.h"

using namespace Utils;

class DNSDispatcher final {
public:
    DNSDispatcher(DNSCache& cache, DNSResolver& resolver);
    DNSParser::DNSPtr dispatch(const  DNSParser::DNSPtr& packet);

private:
    static inline DNS::Types maptype(ldns_rr_type type);
    static inline ldns_rr_type mapBack(DNS::Types type);
    static ldns_rr* buildRr(const Cache::Record& rr);
    static bool isAnswer(const DNSParser::DNSPtr& pkt);
    
    std::vector<ldns_rr> getQuestionRRs(const DNSParser::DNSPtr& pkt) const;
    std::vector<Cache::Record> lookupCache(const std::vector<ldns_rr>& rr);
    bool addToCache(const std::vector<ldns_rr>& rrs);
    DNSParser::DNSPtr createAnswer(const std::vector<Cache::Record>& rrs) const;
    
    
private:
    DNSCache& m_cache;
    DNSResolver& m_resolver;
};

#endif // DNSDISPATCHER_H
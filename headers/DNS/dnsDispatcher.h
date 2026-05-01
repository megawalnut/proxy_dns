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
    
    // main method
    DNSParser::DNSPtr dispatch(const  DNSParser::DNSPtr& packet);

private:
    // interacting with cache
    std::vector<Cache::Record> lookupCache(const std::vector<ldns_rr*>& rr) const;
    bool addToCache(const std::vector<ldns_rr*>& rrs);
    
    // forming packet
    DNSParser::DNSPtr basePacket(const DNSParser::DNSPtr& clientPacket) const;
    DNSParser::DNSPtr makeErrPacket(const DNSParser::DNSPtr& basePacket,
                                    ldns_pkt_rcode  rcode) const;
    DNSParser::DNSPtr makeSucPacket(const DNSParser::DNSPtr& basePacket,
                                    const std::vector<Cache::Record>& rrs) const;

    // additional for forming
    static ldns_rr* buildRr(const Cache::Record& rr);
    static bool addRData(ldns_rr* newRr, const Cache::Record& rr);
    std::vector<ldns_rr*> getQuestionRRs(const DNSParser::DNSPtr& pkt) const;
    std::vector<ldns_rr*> getAnswerRRs(const DNSParser::DNSPtr& pkt) const;

    // other
    static inline DNS::Types fromDNSType(ldns_rr_type type);
    static inline ldns_rr_type toDNSType(DNS::Types type);
    static inline DNS::ClassTypes fromDNSClassType(ldns_rr_class type);
    static inline ldns_rr_class toDNSClassType(DNS::ClassTypes type);
    static bool isAnswer(const DNSParser::DNSPtr& pkt);    

private:
    DNSCache& m_cache;
    DNSResolver& m_resolver;
};

#endif // DNSDISPATCHER_H
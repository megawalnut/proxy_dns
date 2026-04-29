#ifndef DNSDISPATCHER_H
#define DNSDISPATCHER_H

#include <ldns/ldns.h>
#include <map>

#include "../utils.h"
#include "dnsParser.h"
#include "dnsResolver.h"
#include "dnsCache.h"

using namespace Utils;

class DNSDispatcher final {
public:
    DNSDispatcher(DNSParser& parser, DNSCache& cache, DNSResolver& resolver);
    void dispatch(const uint8_t* packet, std::size_t size);

private:
    static inline DNS::Types maptype(ldns_rr_type type);
    
    bool parsePacket(const uint8_t* packet, std::size_t size) const;
    std::optional<Cache::Record> lookupCache(const ldns_rr* rr);
    ldns_pkt createAnswer() const;
    void addQuestions() const;
    bool addToCache();
    
    bool isResponce() const;
    
private:
    //std::map<DNS::Types, std::unique_ptr<IType>> m_handlers;
    DNSParser::DNSptr m_pkt;
    DNSParser& m_parser;
    DNSCache& m_cache;
    DNSResolver& m_resolver;
};

#endif // DNSDISPATCHER_H
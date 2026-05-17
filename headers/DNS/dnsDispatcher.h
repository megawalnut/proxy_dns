#ifndef DNSDISPATCHER_H
#define DNSDISPATCHER_H

#include <iostream>
#include <ldns/ldns.h>
#include <map>
#include <algorithm>
#include <deque>

#include "dnsParser.h"
#include "dnsResolver.h"
#include "dnsCache.h"
#include "../Common/utils.h"

using namespace Utils;

class DNSDispatcher final {
    static constexpr inline uint32_t MAX_ERRORS = 100;
    static constexpr inline uint32_t TOP_DOMAINS_SIZE = 10;
    static constexpr inline uint32_t QUERY_TYPES_SIZE = 6;
    static constexpr inline uint32_t ERRORS_SIZE = 10;
public:
    DNSDispatcher(DNSCache& cache, DNSResolver& resolver);
    
    // main method
    DNSParser::DNSPtr dispatch(const  DNSParser::DNSPtr& packet);

    // metrics
    double getHitsPercent() const;
    uint64_t getCacheEntries() const;
    std::vector<MetricRecords::TopDomainRecord> getTopDomains();
    std::vector<MetricRecords::QueryTypeRecord> getQueryTypes();
    std::vector<MetricRecords::ErrorRecord> getRecentErrors();
    void resetDomains();

private:
    // interacting with cache
    std::vector<Cache::Record> lookupCache(const std::vector<ldns_rr*>& rr);
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

    // metrics
    void addMetrics(const std::string& domain, DNS::Types type);
    void addError(const std::string& domain, const std::string& error);

    // other
    static bool isAnswer(const DNSParser::DNSPtr& pkt);
    static std::string getFirstDomain(ldns_rr* rr);

private:
    std::mutex m_mtx;
    std::atomic<uint64_t> m_cacheHits {};
    std::atomic<uint64_t> m_cacheMiss {};
    std::map<std::string, uint64_t> m_domains;
    std::map<std::string, uint64_t> m_types;
    std::deque<MetricRecords::ErrorRecord> m_errors;

    DNSCache& m_cache;
    DNSResolver& m_resolver;
};

#endif // DNSDISPATCHER_H
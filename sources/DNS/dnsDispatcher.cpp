#include "../../headers/DNS/dnsDispatcher.h"

#include <sstream>

using namespace Utils;

DNSDispatcher::DNSDispatcher(DNSCache& cache, DNSResolver& resolver) 
    :
    m_cache(cache),
    m_resolver(resolver)
{}


// ---------------------------------- MAIN METHOD ------------------------------------
DNSParser::DNSPtr DNSDispatcher::dispatch(const DNSParser::DNSPtr& clientPacket) {
    // checking qr
    if(isAnswer(clientPacket)) {
        return DNSParser::DNSPtr(ldns_pkt_clone(clientPacket.get()));
    }
    std::cout << "DNSDispatcher::dispatch: Packet - is question" << std::endl;

    // forming skeleton
    DNSParser::DNSPtr aPacket = basePacket(clientPacket);
    if(!aPacket) {
        std::cerr << "DNSDispatcher::dispatch: Failed create base packet" << std::endl;
        addError("unknown", "FORMERR");
        return makeErrPacket(clientPacket, LDNS_RCODE_FORMERR);
    }

    // get questions from client
    std::vector<ldns_rr*> resources = getQuestionRRs(clientPacket);
    if(resources.empty()) {
        std::cerr << "DNSDispatcher::dispatch: Question empty" << std::endl;
        addError("unknown", "FORMERR");
        return makeErrPacket(aPacket, LDNS_RCODE_FORMERR);
    }

    // add metrics
    std::string errDomain = getFirstDomain(resources[0]);
    const ldns_rr_type type = ldns_rr_get_type(resources[0]);
    auto t =  DNS::fromDNSType(type);
    addMetrics(errDomain, t);

    // check cache
    std::vector<Cache::Record> result = lookupCache(resources);
    if(!result.empty()) {
        ++m_cacheHits;
        return makeSucPacket(aPacket, result);
    }

    ++m_cacheMiss;
        
    // resolving + creating answer
    DNSResolver::Packet res;
    res = m_resolver.resolve(clientPacket);
    if(res.status != Parse::Status::Ok) {
        std::cerr << "DNSDispatcher::dispatch: Failed resolving" << std::endl;
        addError(errDomain, res.error);
        return makeErrPacket(aPacket, LDNS_RCODE_SERVFAIL);
    }

    aPacket = std::move(res.packet);

    //  get answer from resolving packet
    resources.clear();
    resources = getAnswerRRs(aPacket);
    if(resources.empty()) {
        std::cerr << "DNSDispatcher::dispatch: Answer empty" << std::endl;
        addError(errDomain, "NXDOMAIN");
        return makeErrPacket(aPacket, LDNS_RCODE_NXDOMAIN);
    }

    // add to cache
    if(!addToCache(resources)) {
        std::cerr << "DNSDispatcher::dispatch: Failed to adding in cache" << std::endl;
    }

    return aPacket;
}


// ------------------------------------- CACHE ---------------------------------------
std::vector<Cache::Record> DNSDispatcher::lookupCache(const std::vector<ldns_rr*>& rrs) {
    std::vector<Cache::Record> result;
    result.reserve(rrs.size());

    for(const auto& rr : rrs) {
        // get type
        const ldns_rr_type type = ldns_rr_get_type(rr);
        auto t = DNS::fromDNSType(type);
        if(t == DNS::Types::Unknown) {
            std::cerr << "Dispatcher::lookupCache: Type don't support or unknown" << std::endl;
            return {};
        }
        
        // get domain
        const ldns_rdf* domain = ldns_rr_owner(rr);
        if(!domain) {
            std::cerr << "Dispatcher::lookupCache: Invalid domain" << std::endl;
            return {};
        }

        char* cName = ldns_rdf2str(domain);
        if(!cName) {
            return {};
        }

        std::string name(cName);
        free(cName);

        // trim
        if(!name.empty() && name.back() == '.') {
            name.pop_back();
        }

        // create key & find cache record
        Cache::Key key;
        key.domain = name;
        key.type = t;

        auto rec = m_cache.get(key);
        if(!rec) {
            std::cout << "Dispatcher::lookupCache: Not found in cache" << std::endl;
            return {};
        }
        result.push_back(rec.value());
    }
    return result;
}
bool DNSDispatcher::addToCache(const std::vector<ldns_rr*>& rrs) {
    if(rrs.empty()) {
        std::cerr << "DNSDispatcher::addToCache: Answers empty" << std::endl;
        return false;
    }

    bool allOk = true;
    for(const auto& rr : rrs) {
        // get type
        const ldns_rr_type type = ldns_rr_get_type(rr);
        auto t =  DNS::fromDNSType(type);
        if(t == DNS::Types::Unknown) {
            std::cerr << "Dispatcher::addToCache: Type don't support or unknown" << std::endl;
            continue;
        }
        
        // get domain
        const ldns_rdf* domain = ldns_rr_owner(rr);
        if(!domain) {
            std::cerr << "Dispatcher::addToCache: Invalid domain" << std::endl;
            allOk = false;
            continue;
        }

        char* cName = ldns_rdf2str(domain);
        if(!cName) {
            allOk = false;
            continue;
        }

        std::string name(cName);
        free(cName);

        // trim
        if(!name.empty() && name.back() == '.') {
            name.pop_back();
        }

        // create key & find cache record
        Cache::Key key;
        key.domain = name;
        key.type = t;

        // create record
        Cache::Record rec;

        // set key
        rec.key = key;
        
        // set ttl
        rec.created = std::chrono::steady_clock::now();
        rec.ttl = ldns_rr_ttl(rr);

        // set class
        rec.classType = DNS::fromDNSClassType(ldns_rr_get_class(rr));
        
        // adding rdata
        std::vector<std::string> rdata;

        const std::size_t count = ldns_rr_rd_count(rr);
        for(std::size_t i = 0; i < count; ++i) {
            const ldns_rdf* rdf = ldns_rr_rdf(rr, i);
            if(!rdf) {
                std::cerr << "DNSDispatcher::addToCache: Invalid rdata" << std::endl;
                allOk = false;
                continue;
            }

            char* cName2 = ldns_rdf2str(rdf);
            if(!cName2) {
                allOk = false;
                continue;
            }
            rdata.emplace_back(cName2);
            free(cName2);
        }

        rec.rdata = rdata;

        m_cache.put(rec);
    }
    return allOk;
}


// --------------------------------- FORMING PACKET ----------------------------------
DNSParser::DNSPtr DNSDispatcher::basePacket(const DNSParser::DNSPtr& pkt) const {
    if(!pkt) {
        std::cerr << "DNSDispatcher::basePacket: Invalid packet" << std::endl;
        return DNSParser::DNSPtr(nullptr);
    }
    
    // ---------------------- PACKET ------------------------
    // base receive packet
    ldns_pkt* base = ldns_pkt_new();
    if(!base) {
        std::cerr << "DNSDispatcher::basePacket: Failed to allocate packet" << std::endl;
        return DNSParser::DNSPtr(nullptr);
    }

    // ---------------------- HEADER ------------------------
    // set flags
    ldns_pkt_set_id(base, ldns_pkt_id(pkt.get()));
    ldns_pkt_set_qr(base, true);    // 1 - receive
    ldns_pkt_set_opcode(base, ldns_pkt_get_opcode(pkt.get()));
    ldns_pkt_set_aa(base, false);   // were are not authority
    ldns_pkt_set_tc(base, false);   // packet is divided
    ldns_pkt_set_rd(base, ldns_pkt_rd(pkt.get()));
    ldns_pkt_set_ra(base, true);    // we supporting recursing
    ldns_pkt_set_ad(base, false);
    ldns_pkt_set_cd(base, ldns_pkt_cd(pkt.get()));
    ldns_pkt_set_rcode(base, LDNS_RCODE_NOERROR);
    
    // --------------------- QUESTION ----------------------
    // copy question
    ldns_rr_list* questions = ldns_pkt_question(pkt.get());
    if(!questions) {
        std::cerr << "DNSDispatcher::basePacket:Questions empty" << std::endl;
        ldns_pkt_free(base);
        return DNSParser::DNSPtr(nullptr);
    }

    ldns_rr_list* copy = ldns_rr_list_clone(questions);
    if(!copy) {
        std::cerr << "DNSDispatcher::basePacket: Failed to allocate question" << std::endl;
        ldns_pkt_free(base);
        return DNSParser::DNSPtr(nullptr);
    }
    ldns_pkt_set_question(base, copy);
    ldns_pkt_set_qdcount(base, ldns_rr_list_rr_count(copy));

    // --------------------- ANSWER ----------------------
    // FILL ANSWER LATER!
    // -------------------- AUTHORITY --------------------
    // FILL AUTHORITY LATER!
    // ------------------- ADDITIONAL --------------------
    // FILL ADDITIONAL LATER!
    
    // --------------------- RETURN ----------------------
    return DNSParser::DNSPtr(base);
}
DNSParser::DNSPtr DNSDispatcher::makeSucPacket(const DNSParser::DNSPtr& basePacket,
                                               const std::vector<Cache::Record>& rrs) const {
    // ---------------------- PACKET ------------------------
    if(!basePacket) {
        std::cerr << "DNSDispatcher::makeSucPacket: Invalid packet" << std::endl;
        return DNSParser::DNSPtr(nullptr);
    }

    auto base = ldns_pkt_clone(basePacket.get());
    if(!base) {
        std::cerr << "DNSDispatcher::makeSucPacket: Failed to clone packet" << std::endl;
        return makeErrPacket(basePacket, LDNS_RCODE_FORMERR);
    }
    
    // --------------------- HEADER -------------------------
    // all flags set in basePacket()

    // --------------------- ANSWER -------------------------
    ldns_rr_list* answer = ldns_rr_list_new();
    if(!answer) {
        std::cerr << "DNSDispatcher::makeSucPacket: Failed to allocate answer" << std::endl;
        ldns_pkt_free(base);
        return makeErrPacket(basePacket, LDNS_RCODE_FORMERR);
    }

    for(const auto& rr : rrs) {
        ldns_rr* built = buildRr(rr);
        if(built) {
            ldns_rr_list_push_rr(answer, built);
        }
    }

    ldns_pkt_set_answer(base, answer);
    ldns_pkt_set_ancount(base, ldns_rr_list_rr_count(answer));

    // -------------------- AUTHORITY -----------------------
    ldns_pkt_set_authority(base, ldns_rr_list_new());
    
    // -------------------- ADDITIONAL ----------------------
    ldns_pkt_set_additional(base, ldns_rr_list_new());

    return DNSParser::DNSPtr(base);
}
DNSParser::DNSPtr DNSDispatcher::makeErrPacket(const DNSParser::DNSPtr& basePacket, 
                                ldns_pkt_rcode  rcode) const {
    // ---------------------- PACKET ------------------------
    if(!basePacket) {
        std::cerr << "DNSDispatcher::makeErrPacket: Invalid packet" << std::endl;
        return DNSParser::DNSPtr(nullptr);
    }

    auto base = ldns_pkt_clone(basePacket.get());
    if(!base) {
        std::cerr << "DNSDispatcher::makeErrPacket: Failed to clone packet" << std::endl;
        return DNSParser::DNSPtr(nullptr);
    }

    // ---------------------- HEADER ------------------------
    // set flags
    ldns_pkt_set_rcode(base, rcode);    // set rcode
    ldns_pkt_set_qr(base, true);
    ldns_pkt_set_aa(base, false);

    // --------------------- ANSWER -------------------------
    ldns_pkt_set_answer(base, ldns_rr_list_new());

    // -------------------- AUTHORITY -----------------------
    ldns_pkt_set_authority(base, ldns_rr_list_new());
    
    // -------------------- ADDITIONAL ----------------------
    ldns_pkt_set_additional(base, ldns_rr_list_new());

    return DNSParser::DNSPtr(base);
}

    // -------------------------- ADDITIONAL FOR FORM ---------------------------------
std::vector<ldns_rr*> DNSDispatcher::getQuestionRRs(const DNSParser::DNSPtr& pkt) const {
    // get question chapter
    ldns_rr_list* questions = ldns_pkt_question(pkt.get());
    if(!questions) {
        std::cerr << "Dispatcher::getQuestionRRs: Questions empty" << std::endl;
        return {};
    }

    // get questions count
    const std::size_t questionsCount = ldns_rr_list_rr_count(questions);
    
    std::vector<ldns_rr*> resources;
    resources.reserve(questionsCount);

    for(std::size_t i = 0; i < questionsCount; ++i) {
        // struct resource
        //-+-+-+-+-+-+-+-+-+
        //|      NAME      |
        //|      TYPE      |
        //|      CLASS     |
        //|       TTL      |
        //|     RDLENGTH   |
        //|      RDATA     |
        //-+-+-+-+-+-+-+-+-+
        resources.emplace_back(ldns_rr_list_rr(questions, i));
    }
    return resources;
}
std::vector<ldns_rr*> DNSDispatcher::getAnswerRRs(const DNSParser::DNSPtr& pkt) const {
    // get answers chapter
    ldns_rr_list* answers = ldns_pkt_answer(pkt.get());
    if(!answers) {
        std::cerr << "Dispatcher::getAnswerRRs: Answers empty" << std::endl;
        return {};
    }

    // get answers count
    const std::size_t answersCount = ldns_rr_list_rr_count(answers);
    
    std::vector<ldns_rr*> resources;
    resources.reserve(answersCount);

    for(std::size_t i = 0; i < answersCount; ++i) {
        // struct resource
        //-+-+-+-+-+-+-+-+-+
        //|      NAME      |
        //|      TYPE      |
        //|      CLASS     |
        //|       TTL      |
        //|     RDLENGTH   |
        //|      RDATA     |
        //-+-+-+-+-+-+-+-+-+
        resources.emplace_back(ldns_rr_list_rr(answers, i));
    }
    return resources;
}
/*static*/
ldns_rr* DNSDispatcher::buildRr(const Cache::Record& rr) {
    ldns_rr* newRr = ldns_rr_new();
    if(!newRr) {
        std::cerr << "DNSDispatcher::buildRr: Failed to allocate resource" << std::endl;
        return {};
    }

    // get owner
    std::string domain = rr.key.domain;
    if(domain.back() != '.') {
        domain += '.';
    }

    ldns_rdf* owner = ldns_dname_new_frm_str(domain.c_str());
    if(!owner) {
        ldns_rr_free(newRr);
        std::cerr << "DNSDispatcher::buildRr: Failed to allocate owner" << std::endl;
        return {};
    }

    // set owner
    ldns_rr_set_owner(newRr, owner);

    // set type
    ldns_rr_set_type(newRr, toDNSType(rr.key.type));

    // set class
    ldns_rr_set_class(newRr, toDNSClassType(rr.classType));

    // set ttl
    ldns_rr_set_ttl(newRr, rr.ttl);

    // set rdata
    if(!addRData(newRr, rr)) {
        ldns_rr_free(newRr);
        std::cerr << "DNSDispatcher::createAnswer: Failed add rdata" << std::endl;
        return {};
    }

    return newRr;
}
/*static*/
bool DNSDispatcher::addRData(ldns_rr* newRr, const Cache::Record& rr) {
    // set rdata
    switch (rr.key.type) {
        case DNS::Types::A: {
            for(const auto& ip : rr.rdata) {
                struct in_addr addr{};
                if(inet_pton(AF_INET, ip.c_str(), &addr) != 1) {
                    std::cerr << "DNSDispatcher::buildRr: Failed adding ipv4 address" << std::endl;
                    return false;
                }

                void* data = malloc(4);
                if(!data) return false;
                memcpy(data, &addr, 4);

                ldns_rdf* rdf = ldns_rdf_new(LDNS_RDF_TYPE_A, 4, data);
                if(!rdf) {
                    std::cerr << "DNSDispatcher::buildRr: Failed to allocate A rdata" << std::endl;
                    return false;
                }

                ldns_rr_push_rdf(newRr, rdf);
            }
            break;
        }
        case DNS::Types::AAAA: {
            for(const auto& ip : rr.rdata) {
                struct in6_addr addr6{};
                if(inet_pton(AF_INET6, ip.c_str(), &addr6) != 1) {
                    std::cerr << "DNSDispatcher::buildRr: Failed adding ipv6 address" << std::endl;
                    return false;
                }

                void* data = malloc(16);
                if(!data) return false;
                memcpy(data, &addr6, 16);

                ldns_rdf* rdf = ldns_rdf_new(LDNS_RDF_TYPE_AAAA, 16, data);
                if(!rdf) {
                    std::cerr << "DNSDispatcher::buildRr: Failed to allocate AAAA data" << std::endl;
                    return false;
                }

                ldns_rr_push_rdf(newRr, rdf);
            }
            break;
        }
        case DNS::Types::CNAME:
        case DNS::Types::NS: {
            for(const auto& name : rr.rdata) {
                ldns_rdf* rdf = ldns_dname_new_frm_str(name.c_str());
                if(!rdf) {
                    std::cerr << "DNSDispatcher::buildRr: Failed to allocate CNAME/NS data" << std::endl;
                    return false;
                }

                ldns_rr_push_rdf(newRr, rdf);
            }
            break;
        }
        case DNS::Types::MX: {
            for(const auto& mx : rr.rdata) {
                std::istringstream iss(mx);

                int priority = 0;
                std::string host;

                iss >> priority >> host;

                ldns_rdf* pref = ldns_native2rdf_int16(LDNS_RDF_TYPE_INT16, priority);
                ldns_rdf* exch = ldns_dname_new_frm_str(host.c_str());
                if(!pref || !exch) {
                    return false;
                }

                ldns_rr_push_rdf(newRr, pref);
                ldns_rr_push_rdf(newRr, exch);
            }
            break;
        }
        case DNS::Types::TXT: {
            for(const auto& txt : rr.rdata) {
                ldns_rdf* rdf = ldns_rdf_new_frm_str(LDNS_RDF_TYPE_STR, txt.c_str());
                if(!rdf) {
                    std::cerr << "DNSDispatcher::buildRr: Failed to allocate TXT data" << std::endl;
                    return false;
                }

                ldns_rr_push_rdf(newRr, rdf);
            }
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}


// ------------------------------------- METRICS ---------------------------------------
double DNSDispatcher::getHitsPercent() const {
    if(m_cacheHits + m_cacheMiss == 0) {
        return {};
    }
    return  static_cast<double>(m_cacheHits) / 
            static_cast<double>(m_cacheHits + m_cacheMiss) * 100.0;
}
uint64_t DNSDispatcher::getCacheEntries() const {
    return m_cache.getCacheEntries();
}
std::vector<MetricRecords::TopDomainRecord> DNSDispatcher::getTopDomains() {
    std::lock_guard lock{ m_mtx };

    // copy for sort
    std::vector<std::pair<std::string, uint64_t>> sorted(m_domains.begin(), m_domains.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    uint64_t total = 0;
    for (auto& [k, v] : sorted) total += v;

    std::vector<MetricRecords::TopDomainRecord> result;
    for (std::size_t i = 0; i < std::min(sorted.size(), std::size_t(TOP_DOMAINS_SIZE)); ++i) {
        double percent = total > 0 ? 100.0 * sorted[i].second / total : 0.0;
        result.emplace_back(MetricRecords::TopDomainRecord{sorted[i].first, percent, sorted[i].second});
    }

    if(m_domains.size() >= MAX_DOMAINS) {
        m_domains.clear();
    }

    return result;
}
std::vector<MetricRecords::QuerryTypeRecord> DNSDispatcher::getQuerryTypes() {
    std::lock_guard lock{m_mtx};

    // copy for sort
    std::vector<std::pair<std::string, uint64_t>> sorted(m_types.begin(), m_types.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    uint64_t total = 0;
    for (auto& [k, v] : sorted) total += v;

    std::vector<MetricRecords::QuerryTypeRecord> result;
    for (std::size_t i = 0; i < std::min(sorted.size(), std::size_t(QUERRY_TYPES_SIZE)); ++i) {
        double percent = total > 0 ? 100.0 * sorted[i].second / total : 0.0;
        result.emplace_back(MetricRecords::QuerryTypeRecord{sorted[i].first, percent});
    }

    if(m_types.size() >= MAX_TYPES) {
        m_types.clear();
    }

    return result;
}
std::vector<MetricRecords::ErrorRecord> DNSDispatcher::getRecentErrors() {
    std::lock_guard lock{ m_mtx };
    return { m_errors.cbegin(), m_errors.cend() };
}
void DNSDispatcher::addMetrics(const std::string& domain, DNS::Types type) {
    std::lock_guard lock{ m_mtx };
    ++m_domains[domain];
    ++m_types[DNS::typeToStr(type)];
}
void DNSDispatcher::addError(const std::string& domain, const std::string& error) {
    std::lock_guard lock{ m_mtx };
    if(m_errors.size() >= ERRORS_SIZE) {
        m_errors.pop_front();
    }
    double time = std::chrono::duration<double, std::milli>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_errors.emplace_back(MetricRecords::ErrorRecord{time, domain, error});
}


// -------------------------------------- OTHER ----------------------------------------
/*static*/
bool DNSDispatcher::isAnswer(const DNSParser::DNSPtr& pkt) {
    return ldns_pkt_qr(pkt.get());
}
/*static*/
std::string DNSDispatcher::getFirstDomain(ldns_rr* rr) {
    // get domain
    const ldns_rdf* domain = ldns_rr_owner(rr);
    if(!domain) {
        std::cerr << "Dispatcher::lookupCache: Invalid domain" << std::endl;
        return "unknown";
    }

    char* cName = ldns_rdf2str(domain);
    if(!cName) {
        return "unknown";
    }

    std::string name(cName);
    free(cName);

    // trim
    if(!name.empty() && name.back() == '.') {
        name.pop_back();
    }
    
    return name;
}
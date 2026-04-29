#include "../../headers/DNS/dnsDispatcher.h"

#include <sstream>

using namespace Utils;

DNSDispatcher::DNSDispatcher(DNSCache& cache, DNSResolver& resolver) 
    :
    m_cache(cache),
    m_resolver(resolver)
{}

DNSParser::DNSPtr DNSDispatcher::dispatch(const DNSParser::DNSPtr& pkt) {
    // checking qr
    if(isAnswer(pkt)) {
        perror("DNSDispatcher::dispatch: Packet - is answer");
        return {};
    }
    std::cout << "DNSDispatcher::dispatch: Packet - is question" << std::endl;

    // get questions
    std::vector<ldns_rr> resources = getQuestionRRs(pkt);

    // check cache
    std::vector<Cache::Record> result = lookupCache(resources);
    if(!result.empty()) {
        return std::move(createAnswer(result));
    }
        
    // resolving + creating answer
    DNSParser::DNSPkt aPacket = m_resolver.resolve(pkt);
    if(aPacket.first != Parse::Status::Ok) {
        return {};
    }

    //  get questions
    resources.clear();
    resources = getQuestionRRs(aPacket.second);

    // add to cache
    if(!addToCache(resources)) {
        std::cerr << "DNSDispatcher::dispatch: Failed to add in cache" << std::endl;
    }

    return std::move(aPacket.second);
}

std::vector<ldns_rr> DNSDispatcher::getQuestionRRs(const DNSParser::DNSPtr& pkt) const {
    // get question chapter
    ldns_rr_list* questions = ldns_pkt_question(pkt.get());
    if (!questions) {
        return {};
    }

    // get questions count
    std::size_t questionsCount = ldns_rr_list_rr_count(questions);
    
    std::vector<ldns_rr> resources;
    resources.reserve(questionsCount);

    for (std::size_t i = 0; i < questionsCount; ++i) {
        // struct resource
        //-+-+-+-+-+-+-+-+-+
        //|      NAME      |
        //|      TYPE      |
        //|      CLASS     |
        //|       TTL      |
        //|     RDLENGTH   |
        //|      RDATA     |
        //-+-+-+-+-+-+-+-+-+
        const ldns_rr* rr = ldns_rr_list_rr(questions, i);
        if (rr) {
            resources.push_back(*rr);
        }
    }
    return resources;
}

std::vector<Cache::Record> DNSDispatcher::lookupCache(const std::vector<ldns_rr>& rrs) {
    if(rrs.empty()) {
        return {};
    }

    std::vector<Cache::Record> res;
    res.reserve(rrs.size());

    for(const auto& rr : rrs) {
        // get type
        const ldns_rr_type type = ldns_rr_get_type(&rr);
        
        // get domain
        const ldns_rdf* domain = ldns_rr_owner(&rr);
        if(!domain) {
            return {};
        }

        char* c_name = ldns_rdf2str(domain);
        if (!c_name) {
            return {};
        }

        std::string name(c_name);
        free(c_name);

        if (!name.empty() && name.back() == '.') {
            name.pop_back();
        }

        // create key & find cache record
        Cache::Key key;
        key.domain = name;
        key.type = DNSDispatcher::maptype(type);

        auto rec = m_cache.get(key);
        if(!rec) {
            std::cout << "Dispatcher::lookupCache: Not found in cache" << std::endl;
            return {};
        }
        rec->key = key;
        res.push_back(rec.value());
    }
    return res;
}

bool DNSDispatcher::addToCache(const std::vector<ldns_rr>& rrs) {
    if(rrs.empty()) {
        return false;
    }

    bool allOk = true;
    for(const auto& rr : rrs) {
        // get type
        const ldns_rr_type type = ldns_rr_get_type(&rr);
        
        // get domain
        const ldns_rdf* domain = ldns_rr_owner(&rr);
        if(!domain) {
            allOk = false;
            continue;
        }

        char* c_name = ldns_rdf2str(domain);
        if (!c_name) {
            allOk = false;
            continue;
        }

        std::string name(c_name);
        free(c_name);

        if (!name.empty() && name.back() == '.') {
            name.pop_back();
        }

        // create key & find cache record
        Cache::Key key;
        key.domain = name;
        key.type = DNSDispatcher::maptype(type);

        // create record
        Cache::Record rec;
        rec.key = key;
        rec.created = std::chrono::steady_clock::now();
        
        // get ttl
        rec.ttl = ldns_rr_ttl(&rr);
        
        // adding
        std::vector<std::string> rdata;

        std::size_t count = ldns_rr_rd_count(&rr);
        for(std::size_t i = 0; i < count; ++i) {
            ldns_rdf* rdf = ldns_rr_rdf(&rr, i);
            if(!rdf) {
                std::cerr << "DNSDispatcher::addToCache: Invalid rdata" << std::endl;
                allOk = false;
                continue;
            }

            char* c_name = ldns_rdf2str(rdf);
            if (!c_name) {
                allOk = false;
                continue;
            }
            rdata.push_back(c_name);
            free(c_name);
        }

        rec.rdata = rdata;

        m_cache.put(rec);
    }
    return allOk;
}

DNSParser::DNSPtr DNSDispatcher::createAnswer(const std::vector<Cache::Record>& rrs) const {
    ldns_pkt* pkt = ldns_pkt_new();

    if (!pkt) {
        std::cerr << "createAnswer: failed to allocate packet\n";
        return {};
    }

    // this is answer
    ldns_pkt_set_qr(pkt, true);

    // set status ok
    ldns_pkt_set_rcode(pkt, LDNS_RCODE_NOERROR);

    // answers list
    ldns_rr_list* answer = ldns_rr_list_new();
    if (!answer) {
        ldns_pkt_free(pkt);
        return {};
    }

    for (const auto& rr : rrs) {
        ldns_rr* built = buildRr(rr);
        if(built) {
            ldns_rr_list_push_rr(answer, built);
        }
    }

    ldns_pkt_set_answer(pkt, answer);

    return DNSParser::DNSPtr(pkt);
}

/*static*/
ldns_rr* DNSDispatcher::buildRr(const Cache::Record& rr) {
    ldns_rr* newRr = ldns_rr_new();

    // owner
    ldns_rdf* owner = ldns_dname_new_frm_str(rr.key.domain.c_str());
    ldns_rr_set_owner(newRr, owner);

    // type + class + ttl
    ldns_rr_set_type(newRr, mapBack(rr.key.type));
    ldns_rr_set_class(newRr, LDNS_RR_CLASS_IN);
    ldns_rr_set_ttl(newRr, rr.ttl);

    switch (rr.key.type) {
        case DNS::Types::A: {
            for (const auto& ip : rr.rdata) {
                struct in_addr addr{};
                inet_pton(AF_INET, ip.c_str(), &addr);

                ldns_rdf* rdf = ldns_rdf_new(
                    LDNS_RDF_TYPE_A,
                    4,
                    &addr
                );

                ldns_rr_push_rdf(newRr, rdf);
            }
            break;
        }
        case DNS::Types::AAAA: {
            struct in6_addr addr6{};
            inet_pton(AF_INET6, rr.rdata[0].c_str(), &addr6);

            ldns_rdf* rdf = ldns_rdf_new(
                LDNS_RDF_TYPE_AAAA,
                16,
                &addr6
            );

            ldns_rr_push_rdf(newRr, rdf);
            break;
        }
        case DNS::Types::CNAME:
        case DNS::Types::NS: {
            for (const auto& name : rr.rdata) {
                ldns_rdf* rdf = ldns_dname_new_frm_str(name.c_str());
                ldns_rr_push_rdf(newRr, rdf);
            }
            break;
        }
        case DNS::Types::MX: {
            for (const auto& mx : rr.rdata) {
                std::istringstream iss(mx);

                int priority = 0;
                std::string host;

                iss >> priority >> host;

                ldns_rdf* pref = ldns_native2rdf_int16(LDNS_RDF_TYPE_NONE, priority);
                ldns_rdf* exch = ldns_dname_new_frm_str(host.c_str());

                ldns_rr_push_rdf(newRr, pref);
                ldns_rr_push_rdf(newRr, exch);
            }
            break;
        }
        case DNS::Types::TXT: {
            for (const auto& txt : rr.rdata) {
                ldns_rdf* rdf = ldns_rdf_new_frm_str(
                    LDNS_RDF_TYPE_STR,
                    txt.c_str()
                );

                ldns_rr_push_rdf(newRr, rdf);
            }
            break;
        }

        default:
            break;
    }

    return newRr;
}

/*static*/
bool DNSDispatcher::isAnswer(const DNSParser::DNSPtr& pkt) {
    bool isAnswer = ldns_pkt_qr(pkt.get());
    return isAnswer ? true : false;
}

/*static*/
DNS::Types DNSDispatcher::maptype(ldns_rr_type type) {
    switch(type) {
        case LDNS_RR_TYPE_A:     return DNS::Types::A;
        case LDNS_RR_TYPE_AAAA:  return DNS::Types::AAAA;
        case LDNS_RR_TYPE_MX:    return DNS::Types::MX;
        case LDNS_RR_TYPE_TXT:   return DNS::Types::TXT;
        case LDNS_RR_TYPE_CNAME: return DNS::Types::CNAME;
        case LDNS_RR_TYPE_NS:    return DNS::Types::NS;
        default:                 return DNS::Types::Unknown;
    };
}

/*static*/
ldns_rr_type DNSDispatcher::mapBack(DNS::Types type) {
    switch(type) {
        case DNS::Types::A:       return LDNS_RR_TYPE_A;
        case DNS::Types::AAAA:    return LDNS_RR_TYPE_AAAA;
        case DNS::Types::MX:      return LDNS_RR_TYPE_MX;
        case DNS::Types::TXT:     return LDNS_RR_TYPE_TXT;
        case DNS::Types::CNAME:   return LDNS_RR_TYPE_CNAME;
        case DNS::Types::NS:      return LDNS_RR_TYPE_NS;
        case DNS::Types::Unknown:
        default:                  return LDNS_RR_TYPE_UNSPEC;
    };
}
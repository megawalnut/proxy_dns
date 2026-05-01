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

    // get questions from client
    std::vector<ldns_rr*> resources = getQuestionRRs(clientPacket);
    if(resources.empty()) {
        std::cerr << "DNSDispatcher::dispatch: Question empty" << std::endl;
        return makeErrPacket(aPacket, LDNS_RCODE_FORMERR);
    }

    // check cache
    std::vector<Cache::Record> result = lookupCache(resources);
    if(!result.empty()) {
        return makeSucPacket(aPacket, result);
    }
        
    // resolving + creating answer
    Parse::Status ok = Parse::Status::Err;
    std::tie(ok, aPacket) = m_resolver.resolve(clientPacket);
    if(ok != Parse::Status::Ok) {
        std::cerr << "DNSDispatcher::dispatch: Failed resolving" << std::endl;
        return makeErrPacket(aPacket, LDNS_RCODE_SERVFAIL);
    }

    //  get questions from resolving packet
    resources.clear();
    resources = getAnswerRRs(aPacket);

    // add to cache
    if(!addToCache(resources)) {
        std::cerr << "DNSDispatcher::dispatch: Failed to adding in cache" << std::endl;
    }

    return aPacket;
}


// ------------------------------------- CACHE ---------------------------------------
std::vector<Cache::Record> DNSDispatcher::lookupCache(const std::vector<ldns_rr*>& rrs) const {
    std::vector<Cache::Record> result;
    result.reserve(rrs.size());

    for(const auto& rr : rrs) {
        // get type
        const ldns_rr_type type = ldns_rr_get_type(rr);
        auto t = DNSDispatcher::fromDNSType(type);
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

        char* c_name = ldns_rdf2str(domain);
        if(!c_name) {
            return {};
        }

        std::string name(c_name);
        free(c_name);

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
        std::cerr << "DNSDispatcher::addToCache: Questions empty" << std::endl;
        return false;
    }

    bool allOk = true;
    for(const auto& rr : rrs) {
        // get type
        const ldns_rr_type type = ldns_rr_get_type(rr);
        auto t =  DNSDispatcher::fromDNSType(type);
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

        char* c_name = ldns_rdf2str(domain);
        if(!c_name) {
            allOk = false;
            continue;
        }

        std::string name(c_name);
        free(c_name);

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
        rec.classType = fromDNSClassType(ldns_rr_get_class(rr));
        
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

            char* c_name = ldns_rdf2str(rdf);
            if(!c_name) {
                allOk = false;
                continue;
            }
            rdata.emplace_back(c_name);
            free(c_name);
        }

        rec.rdata = rdata;

        m_cache.put(rec);
    }
    return allOk;
}

// --------------------------------- FORMING PACKET ----------------------------------
DNSParser::DNSPtr DNSDispatcher::basePacket(const DNSParser::DNSPtr& pkt) const {
    if(!pkt) {
        std::cerr << "DNSDispatcher::basePacket: QPacket empty" << std::endl;
        return {};
    }
    
    // ---------------------- PACKET ------------------------
    // base receive packet
    ldns_pkt* base = ldns_pkt_new();
    if(!base) {
        std::cerr << "DNSDispatcher::basePacket: Failed to allocate packet" << std::endl;
        return {};
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
        return {};
    }

    ldns_rr_list* copy = ldns_rr_list_clone(questions);
    if(!copy) {
        std::cerr << "DNSDispatcher::basePacket: Failed to allocate question" << std::endl;
        ldns_pkt_free(base);
        return {};
    }
    ldns_pkt_set_question(base, copy);

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
        return {};
    }

    auto base = ldns_pkt_clone(basePacket.get());
    if(!base) {
        std::cerr << "DNSDispatcher::makeSucPacket: Failed to clone packet" << std::endl;
        return {};
    }
    
    // --------------------- ANSWER -------------------------
    ldns_rr_list* answer = ldns_rr_list_new();
    if(!answer) {
        std::cerr << "DNSDispatcher::makeSucPacket: Failed to allocate answer" << std::endl;
        ldns_pkt_free(base);
        return {};
    }

    for(const auto& rr : rrs) {
        ldns_rr* built = buildRr(rr);
        if(built) {
            ldns_rr_list_push_rr(answer, built);
        }
    }

    ldns_pkt_set_answer(base, answer);

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
        return {};
    }

    auto base = ldns_pkt_clone(basePacket.get());
    if(!base) {
        std::cerr << "DNSDispatcher::makeErrPacket: Failed to clone packet" << std::endl;
        return {};
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
    // get question chapter
    ldns_rr_list* answers = ldns_pkt_answer(pkt.get());
    if(!answers) {
        std::cerr << "Dispatcher::getAnswerRRs: Answers empty" << std::endl;
        return {};
    }

    // get questions count
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


// -------------------------------------- OTHER ----------------------------------------
/*static*/
bool DNSDispatcher::isAnswer(const DNSParser::DNSPtr& pkt) {
    return ldns_pkt_qr(pkt.get());
}
/*static*/
DNS::Types DNSDispatcher::fromDNSType(ldns_rr_type type) {
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
ldns_rr_type DNSDispatcher::toDNSType(DNS::Types type) {
    switch(type) {
        case DNS::Types::A:       return LDNS_RR_TYPE_A;
        case DNS::Types::AAAA:    return LDNS_RR_TYPE_AAAA;
        case DNS::Types::MX:      return LDNS_RR_TYPE_MX;
        case DNS::Types::TXT:     return LDNS_RR_TYPE_TXT;
        case DNS::Types::CNAME:   return LDNS_RR_TYPE_CNAME;
        case DNS::Types::NS:      return LDNS_RR_TYPE_NS;
        case DNS::Types::Unknown:
        default:                  return LDNS_RR_TYPE_ANY;
    };
}
/*static*/
DNS::ClassTypes DNSDispatcher::fromDNSClassType(ldns_rr_class type) {
    switch(type) {
        case LDNS_RR_CLASS_IN: return DNS::ClassTypes::Internet;
        default:               return DNS::ClassTypes::Unknown;
    };
}
/*static*/
ldns_rr_class DNSDispatcher::toDNSClassType(DNS::ClassTypes type) {
    switch(type) {
        case DNS::ClassTypes::Internet: return LDNS_RR_CLASS_IN;
        case DNS::ClassTypes::Unknown:
        default:                        return LDNS_RR_CLASS_ANY;
    };
}
#include "../../headers/DNS/dnsDispatcher.h"

using namespace Utils;

DNSDispatcher::DNSDispatcher() {
    // m_handlers[DNS::Types::A]     = std::make_unique<A>();
    // m_handlers[DNS::Types::AAAA]  = std::make_unique<AAAA>();
    // m_handlers[DNS::Types::MX]    = std::make_unique<MX>();
    // m_handlers[DNS::Types::TXT]   = std::make_unique<TXT>();
    // m_handlers[DNS::Types::CNAME] = std::make_unique<CNAME>();
    // m_handlers[DNS::Types::NS]    = std::make_unique<NS>();
    // std::cout << "Dispatcher:: The dispetcher was created" << std::endl;
}

void DNSDispatcher::dispatch(const DNSParser::DNSptr& pkt) {
    bool isResponse = ldns_pkt_qr(pkt.get());
    if(isResponse) {
        std::cout << "DNSDispatcher::dispatch: Packet - is response" << std::endl;
        return;
    }
    std::cout << "DNSDispatcher::dispatch: Packet - is question" << std::endl;

    // get question chapter
    ldns_rr_list* questions = ldns_pkt_question(pkt.get());
    if (!questions) {
        return;
    }

    // get questions count
    std::size_t questionsCount = ldns_rr_list_rr_count(questions);
    for (std::size_t i = 0; i < questionsCount; ++i) {
        // struct resource
        //-+-+-+-+-+-+-+-+-+-
        //|      NAME      |
        //|      TYPE      |
        //|      CLASS     |
        //|       TTL      |
        //|     RDLENGTH   |
        //|      RDATA     |
        //-+-+-+-+-+-+-+-+-+-
        ldns_rr* resource = ldns_rr_list_rr(questions, i);

        //DNS::Types type = maptype(ldns_rr_get_type(resource));

        auto rec = lookupCache(resource);
        if(rec) {
            return rec;
        }

        
        
    }
}

// static
// DNS::Types DNSDispatcher::maptype(ldns_rr_type type) {
//     switch(type) {
//         case LDNS_RR_TYPE_A:     return DNS::Types::A;
//         case LDNS_RR_TYPE_AAAA:  return DNS::Types::AAAA;
//         case LDNS_RR_TYPE_MX:    return DNS::Types::MX;
//         case LDNS_RR_TYPE_TXT:   return DNS::Types::TXT;
//         case LDNS_RR_TYPE_CNAME: return DNS::Types::CNAME;
//         case LDNS_RR_TYPE_NS:    return DNS::Types::NS;
//         default:                 return DNS::Types::Unknown;
//     };
// }

std::optional<Cache::Record> DNSDispatcher::lookupCache(const ldns_rr* rr) {
    if(!rr) {
        return std::nullopt;
    }

    Cache::Key key;

    // get type
    const ldns_rr_type type = ldns_rr_get_type(rr);
    
    // get domain
    const ldns_rdf* domain = ldns_rr_owner(rr);
    if(!domain) {
        return std::nullopt;
    }

    char* c_name = ldns_rdf2str(domain);
    if (!c_name) {
        return std::nullopt;
    }

    std::string name(c_name);
    free(c_name);

    if (!name.empty() && name.back() == '.') {
        name.pop_back();
    }

    // create key & find cache record
    key.domain = name;
    key.type = DNSDispatcher::maptype(type);

    auto rec = m_cache.get(key);
    if(rec) {
        std::cout << "Dispatcher::lookupCache: Find in cache";
        return rec;
    }
    return std::nullopt;
}
#include "../../headers/DNS/dnsDispatcher.h"

DNSDispatcher::DNSDispatcher() {
    m_handlers[DNSCachingTypes::A]     = std::make_unique<A>();
    m_handlers[DNSCachingTypes::AAAA]  = std::make_unique<AAAA>();
    m_handlers[DNSCachingTypes::MX]    = std::make_unique<MX>();
    m_handlers[DNSCachingTypes::TXT]   = std::make_unique<TXT>();
    m_handlers[DNSCachingTypes::CNAME] = std::make_unique<CNAME>();
    m_handlers[DNSCachingTypes::NS]    = std::make_unique<NS>();
    std::cout << "Dispatcher:: The dispetcher was created" << std::endl;
}

void DNSDispatcher::dispatch(const DNSParser::DNSptr& pkt) const {
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

        DNSCachingTypes type = maptype(ldns_rr_get_type(resource));

        auto it = m_handlers.find(type);
        if(it == m_handlers.end()) {
            std::cerr << "DNSDispatcher::dispatch: Failed type :" << static_cast<uint32_t>(type) << std::endl;
            continue;
        }
        it->second->handle(resource);
    }
}

// --------------- Private ----------------
DNSDispatcher::DNSCachingTypes DNSDispatcher::maptype(ldns_rr_type type) {
    switch(type) {
        case LDNS_RR_TYPE_A:     return DNSDispatcher::DNSCachingTypes::A;
        case LDNS_RR_TYPE_AAAA:  return DNSDispatcher::DNSCachingTypes::AAAA;
        case LDNS_RR_TYPE_MX:    return DNSDispatcher::DNSCachingTypes::MX;
        case LDNS_RR_TYPE_TXT:   return DNSDispatcher::DNSCachingTypes::TXT;
        case LDNS_RR_TYPE_CNAME: return DNSDispatcher::DNSCachingTypes::CNAME;
        case LDNS_RR_TYPE_NS:    return DNSDispatcher::DNSCachingTypes::NS;
        default:                 return DNSDispatcher::DNSCachingTypes::Unknown;
    };
}
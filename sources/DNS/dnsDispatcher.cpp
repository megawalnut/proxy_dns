#include "../../headers/DNS/dnsDispatcher.h"

using namespace Utils;

DNSDispatcher::DNSDispatcher(DNSParser& parser, DNSCache& cache, DNSResolver& resolver) 
    : 
    m_parser(parser),
    m_cache(cache),
    m_resolver(resolver)  
{}

void DNSDispatcher::dispatch(const uint8_t* packet, std::size_t size) {
    // parse packet
    if(!parsePacket(packet, size)) {
        perror("Failed parse packet");
        return;
    }

    // checking qr
    if(!isResponce()) {
        perror("DNSDispatcher::dispatch: Packet - is response");
        return;
    }
    std::cout << "DNSDispatcher::dispatch: Packet - is question" << std::endl;

    // create answer chapter
    ldns_rr_list* ansQuestions;

    // get question chapter
    ldns_rr_list* questions = ldns_pkt_question(m_pkt.get());
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

        // DNS::Types type = maptype(ldns_rr_get_type(resource));


        // check cache
        auto rec = lookupCache(resource);
        if(rec) {
            addQuestions(resource, ansQuestions);
        }
        
        // resolving
        if(!m_resolver.resolve(packet, size)) {
            return;
        }

        // parse resolve packet
        if(!parsePacket(m_resolver.getReceive(), m_resolver.getReceiveSize())) {
            perror("Failed parse packet");
            return;
        }

        // add to cache

    }
}

bool DNSDispatcher::parsePacket(const uint8_t* packet, std::size_t size) const {
    // parsing to DNS packet
    Parse::Status ok = Parse::Status::Err;
    std::tie(ok, m_pkt) = m_parser.parse(packet, size);
    if(ok != Parse::Status::Ok) {
        perror("Main::main: Recvfrom failed");
        return false;
    }
    return true;
}

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
        std::cout << "Dispatcher::lookupCache: Find in cache" << std::endl;
        return rec;
    }
    return std::nullopt;
}

bool DNSDispatcher::addToCache() {
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
        std::cout << "Dispatcher::lookupCache: Find in cache" << std::endl;
        return rec;
    }
    return std::nullopt;
}

bool DNSDispatcher::isResponce() const {
    bool isResponse = ldns_pkt_qr(m_pkt.get());
    return isResponse ? true : false;
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
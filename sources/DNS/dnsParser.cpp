#include "../../headers/DNS/dnsParser.h"

// converting the byte stream into a basic dns structure, see RFC1035
/*static*/
DNSParser::DNSPkt DNSParser::deserialize(const std::vector<uint8_t>& packet) {
    ldns_pkt *pkt = nullptr;

    if(ldns_wire2pkt(&pkt, packet.data(), packet.size()) != LDNS_STATUS_OK) {
        std::cerr << "DNSParser::deserialize: Failed deserialize packet" << std::endl;
        return { Utils::Parse::Status::Err, DNSPtr(nullptr) };
    }

    std::cout << "DNSParser::deserialize: Success deserialize" << std::endl;
    return { Utils::Parse::Status::Ok, DNSPtr(pkt) };
}

/*static*/
DNSParser::DNSWire DNSParser::serialize(const DNSPtr& packet) {
    uint8_t* wire = nullptr;
    std::size_t wireSize = 0;

    ldns_status ok = ldns_pkt2wire(&wire, packet.get(), &wireSize);
    if(ok != LDNS_STATUS_OK || !wire || wireSize == 0) {
        std::cerr << "DNSParser::serialize: Failed serialize packet" << std::endl;
        return { Utils::Parse::Status::Err, {} };
    }

    std::cout << "DNSParser::serialize: Success serialize" << std::endl;
    std::vector<uint8_t> vec(wire, wire + wireSize);
    free(wire);
    
    return { Utils::Parse::Status::Ok, vec };
}
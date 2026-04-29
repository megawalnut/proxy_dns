#include "../../headers/DNS/dnsParser.h"

// converting the byte stream into a basic dns structure, see RFC1035 
/*static*/
DNSParser::DNSPkt DNSParser::deserialize(const std::vector<uint8_t>& packet) { 
    ldns_pkt *pkt = nullptr; 

    if (ldns_wire2pkt(&pkt, packet.data(), packet.size()) != LDNS_STATUS_OK) { 
        std::cerr << "DNSParser::deserialize: Faildel to parse packet!" << std::endl; 
        return { Utils::Parse::Status::Err, nullptr }; 
    } 

    std::cout << "DNSParser::deserialize: Success parse" << std::endl; 
    return { Utils::Parse::Status::Ok, DNSPtr(pkt) }; 
}

/*static*/
std::vector<uint8_t> DNSParser::serialize(const DNSPtr& packet) { 
    uint8_t* wire = nullptr;
    std::size_t wireSize = 0;

    ldns_status ok = ldns_pkt2wire(&wire, packet.get(), &wireSize);
    if(ok != LDNS_STATUS_OK || !wire || wireSize == 0) {
        std::cerr << "DNSParser::serialize: Failed serialize packet" << std::endl;
        return {};
    }

    std::vector<uint8_t> vec(wire, wire + wireSize);
    free(wire);
    
    return vec;
}
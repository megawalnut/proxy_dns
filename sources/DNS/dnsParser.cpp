#include "../../headers/DNS/dnsParser.h"

// converting the byte stream into a basic dns structure, see RFC1035 
DNSParser::PktDNS DNSParser::parse(const uint8_t* packet, std::size_t size) const { 
    ldns_pkt *pkt = nullptr; 

    if (ldns_wire2pkt(&pkt, packet, size) != LDNS_STATUS_OK) { 
        std::cerr << "DNSParser::parse: Faildel to parse packet!" << std::endl; 
        return { Utils::Parse::Status::Err, nullptr }; 
    } 

    std::cout << "DNSParser::parse: Success parse" << std::endl; 
    return { Utils::Parse::Status::Ok, DNSptr(pkt) }; 
}
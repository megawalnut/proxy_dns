#ifndef DNSPARSER_H
#define DNSPARSER_H

#include <iostream>
#include <ldns/ldns.h>
#include <memory>

#include "../utils.h"

class DNSParser final {
public:
    //deleter form ldns_packet
    struct PktDeleter {
        void operator()(ldns_pkt* pkt) const {
            if(pkt) {
                ldns_pkt_free(pkt);
            }
        }
    };

    using DNSptr = std::unique_ptr<ldns_pkt, PktDeleter>;
    using PktDNS = std::pair<Utils::Parse::Status, DNSptr>;

    PktDNS parse(const uint8_t* packet, std::size_t size) const;
};

#endif // DNSPARSER_H